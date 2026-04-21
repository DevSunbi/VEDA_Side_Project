#include "sync_client.h"

#include <QJsonDocument>
#include <QJsonObject>

static QByteArray compactJsonLine(const QJsonObject &obj)
{
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
}

SyncClient::SyncClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_socket, &QTcpSocket::connected, this, &SyncClient::connected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &SyncClient::disconnected);
    connect(&m_socket, &QTcpSocket::readyRead, this, &SyncClient::onReadyRead);

    connect(&m_socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        emit socketError(m_socket.errorString());
    });
}

void SyncClient::connectToHost(const QString &host, quint16 port)
{
    m_socket.connectToHost(host, port);
}

void SyncClient::disconnectFromHost()
{
    m_socket.disconnectFromHost();
}

bool SyncClient::isConnected() const
{
    return m_socket.state() == QAbstractSocket::ConnectedState;
}

int SyncClient::requestState(Callback cb)
{
    return sendRequest("state.get", QJsonObject{}, std::move(cb));
}

int SyncClient::subscribe(Callback cb)
{
    return sendRequest("subscribe", QJsonObject{}, std::move(cb));
}

int SyncClient::createAccount(const QString &accountNumber,
                              const QString &password,
                              qint64 initialBalance,
                              Callback cb)
{
    QJsonObject data;
    data["accountNumber"] = accountNumber;
    data["password"] = password;
    data["initialBalance"] = static_cast<qint64>(initialBalance);
    return sendRequest("account.create", data, std::move(cb));
}

int SyncClient::deleteAccount(const QString &accountNumber,
                              const QString &password,
                              Callback cb)
{
    QJsonObject data;
    data["accountNumber"] = accountNumber;
    data["password"] = password;
    return sendRequest("account.delete", data, std::move(cb));
}

int SyncClient::deposit(const QString &accountNumber,
                        const QString &password,
                        qint64 amount,
                        const QString &memo,
                        Callback cb)
{
    QJsonObject data;
    data["accountNumber"] = accountNumber;
    data["password"] = password;
    data["amount"] = static_cast<qint64>(amount);
    data["memo"] = memo;
    return sendRequest("deposit", data, std::move(cb));
}

int SyncClient::withdraw(const QString &accountNumber,
                         const QString &password,
                         qint64 amount,
                         const QString &memo,
                         Callback cb)
{
    QJsonObject data;
    data["accountNumber"] = accountNumber;
    data["password"] = password;
    data["amount"] = static_cast<qint64>(amount);
    data["memo"] = memo;
    return sendRequest("withdraw", data, std::move(cb));
}

int SyncClient::transfer(const QString &fromAccountNumber,
                         const QString &password,
                         const QString &toAccountNumber,
                         qint64 amount,
                         const QString &memo,
                         Callback cb)
{
    QJsonObject data;
    data["fromAccountNumber"] = fromAccountNumber;
    data["password"] = password;
    data["toAccountNumber"] = toAccountNumber;
    data["amount"] = static_cast<qint64>(amount);
    data["memo"] = memo;
    return sendRequest("transfer", data, std::move(cb));
}

void SyncClient::onReadyRead()
{
    m_buffer.append(m_socket.readAll());

    while (true) {
        const int nl = m_buffer.indexOf('\n');
        if (nl < 0) break;
        const QByteArray line = m_buffer.left(nl).trimmed();
        m_buffer.remove(0, nl + 1);
        if (line.isEmpty()) continue;
        handleLine(line);
    }
}

void SyncClient::handleLine(const QByteArray &line)
{
    QJsonParseError parseError{};
    const QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) return;

    const QJsonObject msg = doc.object();
    const QString type = msg.value("type").toString();

    if (type == "resp") {
        const int reqId = msg.value("reqId").toInt(-1);
        const bool ok = msg.value("ok").toBool(false);
        const QJsonObject data = msg.value("data").toObject();
        const QJsonObject error = msg.value("error").toObject();

        auto it = m_pending.find(reqId);
        if (it != m_pending.end()) {
            Callback cb = std::move(it.value());
            m_pending.erase(it);
            if (cb) cb(ok, data, error);
        }
        return;
    }

    if (type == "changed") {
        const QJsonObject data = msg.value("data").toObject();
        const qint64 version = data.value("version").toVariant().toLongLong();
        emit changed(version);
        return;
    }

    if (type == "error") {
        const QJsonObject err = msg.value("error").toObject();
        emit socketError(err.value("message").toString("Unknown error"));
        return;
    }
}

int SyncClient::sendRequest(const QString &type, const QJsonObject &data, Callback cb)
{
    const int reqId = m_nextReqId++;

    QJsonObject msg;
    msg["type"] = type;
    msg["reqId"] = reqId;
    msg["data"] = data;

    if (cb) m_pending.insert(reqId, std::move(cb));

    m_socket.write(compactJsonLine(msg));
    return reqId;
}

