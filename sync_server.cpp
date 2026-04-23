#include "sync_server.h"

#include "bankmanager.h"
#include "sync_state.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QHostAddress>

static QByteArray compactJsonLine(const QJsonObject &obj)
{
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
}

static QJsonObject errorObj(const QString &code, const QString &message)
{
    QJsonObject e;
    e["code"] = code;
    e["message"] = message;
    return e;
}

SyncServer::SyncServer(BankManager *bankManager, QObject *parent)
    : QObject(parent)
    , m_bankManager(bankManager)
    , m_persistencePath("account_info.json")
{
    connect(&m_server, &QTcpServer::newConnection, this, &SyncServer::onNewConnection);
}

bool SyncServer::start(quint16 port, QString *errorMessage)
{
    if (m_server.isListening()) return true;

    if (!m_server.listen(QHostAddress::Any, port)) {
        if (errorMessage) *errorMessage = m_server.errorString();
        return false;
    }

    emit runningChanged(true);
    return true;
}

void SyncServer::stop()
{
    if (!m_server.isListening()) return;

    const auto sockets = m_clients.keys();
    for (QTcpSocket *socket : sockets) {
        socket->disconnect(this);
        socket->disconnectFromHost();
        socket->deleteLater();
    }
    m_clients.clear();

    m_server.close();
    emit runningChanged(false);
}

bool SyncServer::isRunning() const
{
    return m_server.isListening();
}

quint16 SyncServer::port() const
{
    return m_server.serverPort();
}

qint64 SyncServer::version() const
{
    return m_version;
}

void SyncServer::setPersistencePath(const QString &path)
{
    m_persistencePath = path;
}

void SyncServer::notifyExternalMutation()
{
    bumpVersionAndBroadcast();
    saveNow();
}

void SyncServer::onNewConnection()
{
    while (m_server.hasPendingConnections()) {
        QTcpSocket *socket = m_server.nextPendingConnection();
        m_clients.insert(socket, ClientState{});

        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() { onSocketReadyRead(socket); });
        connect(socket, &QTcpSocket::disconnected, this, [this, socket]() { onSocketDisconnected(socket); });
    }
}

void SyncServer::onSocketReadyRead(QTcpSocket *socket)
{
    auto it = m_clients.find(socket);
    if (it == m_clients.end()) return;

    it->buffer.append(socket->readAll());

    while (true) {
        const int nl = it->buffer.indexOf('\n');
        if (nl < 0) break;

        const QByteArray line = it->buffer.left(nl).trimmed();
        it->buffer.remove(0, nl + 1);
        if (line.isEmpty()) continue;
        handleLine(socket, line);
    }
}

void SyncServer::onSocketDisconnected(QTcpSocket *socket)
{
    m_clients.remove(socket);
    socket->deleteLater();
}

void SyncServer::handleLine(QTcpSocket *socket, const QByteArray &line)
{
    QJsonParseError parseError{};
    const QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        // Best effort: response without reqId
        QJsonObject push;
        push["type"] = "error";
        push["error"] = errorObj("invalid_json", "Invalid JSON.");
        socket->write(compactJsonLine(push));
        return;
    }

    const QJsonObject msg = doc.object();
    const QString type = msg.value("type").toString();
    const int reqId = msg.value("reqId").toInt(-1);
    const QJsonObject data = msg.value("data").toObject();

    if (type.isEmpty() || reqId < 0) {
        QJsonObject push;
        push["type"] = "error";
        push["error"] = errorObj("invalid_request", "Missing type/reqId.");
        socket->write(compactJsonLine(push));
        return;
    }

    if (type == "hello") {
        QJsonObject respData;
        respData["server"] = "SideProject_Account";
        respData["version"] = m_version;
        sendResponse(socket, reqId, true, respData, QJsonObject{});
        return;
    }

    if (type == "subscribe") {
        m_clients[socket].subscribed = true;
        QJsonObject respData;
        respData["subscribed"] = true;
        respData["version"] = m_version;
        sendResponse(socket, reqId, true, respData, QJsonObject{});
        return;
    }

    if (type == "state.get") {
        QJsonObject respData = SyncState::exportState(m_bankManager);
        respData["version"] = m_version;
        sendResponse(socket, reqId, true, respData, QJsonObject{});
        return;
    }

    auto findAccountIdByNumber = [this](const QString &accNum) -> int {
        for (const auto &acc : m_bankManager->accountModel()->accounts()) {
            if (acc.accountNumber == accNum) return acc.id;
        }
        return -1;
    };

    auto checkPasswordById = [this](int accountId, const QString &password) -> bool {
        for (const auto &acc : m_bankManager->accountModel()->accounts()) {
            if (acc.id == accountId) return acc.password == password;
        }
        return false;
    };

    if (type == "account.create") {
        const QString accNum = data.value("accountNumber").toString().trimmed();
        const QString password = data.value("password").toString().trimmed();
        const qint64 initialBalance = data.value("initialBalance").toVariant().toLongLong();

        if (accNum.isEmpty() || password.isEmpty() || initialBalance < 0) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("invalid_request", "Invalid account fields."));
            return;
        }

        const bool ok = m_bankManager->addAccount(accNum, password, "", initialBalance);
        if (!ok) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("duplicate_account", "Account already exists."));
            return;
        }

        bumpVersionAndBroadcast();
        saveNow();

        QJsonObject respData;
        respData["version"] = m_version;
        sendResponse(socket, reqId, true, respData, QJsonObject{});
        emit stateMutated();
        return;
    }

    if (type == "account.delete") {
        const QString accNum = data.value("accountNumber").toString().trimmed();
        const QString password = data.value("password").toString().trimmed();
        if (accNum.isEmpty() || password.isEmpty()) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("invalid_request", "Invalid fields."));
            return;
        }

        const int accountId = findAccountIdByNumber(accNum);
        if (accountId < 0) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("not_found", "Account not found."));
            return;
        }

        if (!checkPasswordById(accountId, password)) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("auth_failed", "Password mismatch."));
            return;
        }

        const bool ok = m_bankManager->removeAccount(accountId);
        if (!ok) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("internal_error", "Failed to delete account."));
            return;
        }

        // Match current UI behavior: hard-delete transactions to avoid zombie rows.
        m_bankManager->transactionModel()->removeTransactionsByAccountId(accountId);

        bumpVersionAndBroadcast();
        saveNow();

        QJsonObject respData;
        respData["version"] = m_version;
        sendResponse(socket, reqId, true, respData, QJsonObject{});
        emit stateMutated();
        return;
    }

    if (type == "deposit" || type == "withdraw") {
        const QString accNum = data.value("accountNumber").toString().trimmed();
        const QString password = data.value("password").toString().trimmed();
        const qint64 amount = data.value("amount").toVariant().toLongLong();
        const QString memo = data.value("memo").toString();

        if (accNum.isEmpty() || password.isEmpty() || amount <= 0) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("invalid_request", "Invalid fields."));
            return;
        }

        const int accountId = findAccountIdByNumber(accNum);
        if (accountId < 0) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("not_found", "Account not found."));
            return;
        }

        if (!checkPasswordById(accountId, password)) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("auth_failed", "Password mismatch."));
            return;
        }

        if (type == "deposit") {
            m_bankManager->deposit(accountId, amount, memo);
        } else {
            const bool ok = m_bankManager->withdraw(accountId, amount, memo);
            if (!ok) {
                sendResponse(socket, reqId, false, QJsonObject{}, errorObj("insufficient_funds", "Insufficient funds."));
                return;
            }
        }

        bumpVersionAndBroadcast();
        saveNow();

        QJsonObject respData;
        respData["version"] = m_version;
        sendResponse(socket, reqId, true, respData, QJsonObject{});
        emit stateMutated();
        return;
    }

    if (type == "transfer") {
        const QString fromAccNum = data.value("fromAccountNumber").toString().trimmed();
        const QString password = data.value("password").toString().trimmed();
        const QString toAccNum = data.value("toAccountNumber").toString().trimmed();
        const qint64 amount = data.value("amount").toVariant().toLongLong();
        const QString memo = data.value("memo").toString();

        if (fromAccNum.isEmpty() || toAccNum.isEmpty() || password.isEmpty() || amount <= 0) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("invalid_request", "Invalid fields."));
            return;
        }

        const int fromId = findAccountIdByNumber(fromAccNum);
        const int toId = findAccountIdByNumber(toAccNum);
        if (fromId < 0 || toId < 0) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("not_found", "Account not found."));
            return;
        }
        if (fromId == toId) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("same_account", "Cannot transfer to same account."));
            return;
        }
        if (!checkPasswordById(fromId, password)) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("auth_failed", "Password mismatch."));
            return;
        }

        const bool ok = m_bankManager->transfer(fromId, toId, amount, memo);
        if (!ok) {
            sendResponse(socket, reqId, false, QJsonObject{}, errorObj("insufficient_funds", "Transfer failed."));
            return;
        }

        bumpVersionAndBroadcast();
        saveNow();

        QJsonObject respData;
        respData["version"] = m_version;
        sendResponse(socket, reqId, true, respData, QJsonObject{});
        emit stateMutated();
        return;
    }

    sendResponse(socket, reqId, false, QJsonObject{}, errorObj("unknown_type", "Unknown request type."));
}

void SyncServer::sendResponse(QTcpSocket *socket, int reqId, bool ok, const QJsonObject &data, const QJsonObject &error)
{
    QJsonObject resp;
    resp["type"] = "resp";
    resp["reqId"] = reqId;
    resp["ok"] = ok;
    if (ok) {
        resp["data"] = data;
    } else {
        resp["error"] = error;
    }
    socket->write(compactJsonLine(resp));
}

void SyncServer::sendPush(QTcpSocket *socket, const QString &type, const QJsonObject &data)
{
    QJsonObject push;
    push["type"] = type;
    push["data"] = data;
    socket->write(compactJsonLine(push));
}

void SyncServer::broadcastChanged()
{
    QJsonObject data;
    data["version"] = m_version;

    for (auto it = m_clients.begin(); it != m_clients.end(); ++it) {
        if (!it.value().subscribed) continue;
        sendPush(it.key(), "changed", data);
    }
}

void SyncServer::bumpVersionAndBroadcast()
{
    ++m_version;
    broadcastChanged();
}

bool SyncServer::saveNow()
{
    QString err;
    const bool ok = SyncState::saveToFile(m_bankManager, m_persistencePath, &err);
    Q_UNUSED(ok);
    return ok;
}
