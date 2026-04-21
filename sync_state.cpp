#include "sync_state.h"

#include "bankmanager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSet>

namespace SyncState {

static QString setError(QString *out, const QString &msg)
{
    if (out) *out = msg;
    return msg;
}

QJsonObject exportState(BankManager *bankManager)
{
    QJsonArray accountsArray;
    for (const auto &acc : bankManager->accountModel()->accounts()) {
        QJsonObject o;
        o["id"] = acc.id;
        o["name"] = acc.name;
        o["accountNumber"] = acc.accountNumber;
        o["password"] = acc.password;
        o["bankName"] = acc.bankName;
        o["initialBalance"] = static_cast<qint64>(acc.initialBalance);
        o["currentBalance"] = static_cast<qint64>(acc.currentBalance);
        o["allowOverdraft"] = acc.allowOverdraft;
        o["status"] = acc.status;
        o["createdAt"] = static_cast<qint64>(acc.createdAt.toMSecsSinceEpoch());
        accountsArray.append(o);
    }

    QJsonArray txArray;
    for (const auto &tx : bankManager->transactionModel()->transactions()) {
        QJsonObject o;
        o["id"] = tx.id;
        o["accountId"] = tx.accountId;
        o["amount"] = static_cast<qint64>(tx.amount);
        o["type"] = static_cast<int>(tx.type);
        o["status"] = static_cast<int>(tx.status);
        o["memo"] = tx.memo;
        o["counterpartyAccount"] = tx.counterpartyAccount;
        o["occurredAt"] = static_cast<qint64>(tx.occurredAt.toMSecsSinceEpoch());
        o["transferGroupId"] = tx.transferGroupId;
        txArray.append(o);
    }

    QJsonObject root;
    root["accounts"] = accountsArray;
    root["transactions"] = txArray;
    return root;
}

bool importState(BankManager *bankManager, const QJsonObject &root, QString *errorMessage)
{
    if (!root.contains("accounts") || !root.contains("transactions")) {
        setError(errorMessage, "Invalid state: missing accounts/transactions.");
        return false;
    }

    const QJsonArray accountsArray = root["accounts"].toArray();
    QSet<int> validAccountIds;
    for (int i = 0; i < accountsArray.size(); ++i) {
        const QJsonObject o = accountsArray[i].toObject();
        Account acc;
        acc.id = o["id"].toInt(-1);
        acc.name = o["name"].toString();
        acc.accountNumber = o["accountNumber"].toString();
        acc.password = o["password"].toString();
        acc.bankName = o["bankName"].toString();
        acc.initialBalance = o["initialBalance"].toVariant().toLongLong();
        acc.currentBalance = o["currentBalance"].toVariant().toLongLong();
        acc.allowOverdraft = o["allowOverdraft"].toBool(false);
        acc.status = o["status"].toString("활성");
        acc.createdAt = QDateTime::fromMSecsSinceEpoch(o["createdAt"].toVariant().toLongLong());

        bankManager->restoreAccount(acc);
        if (acc.id >= 0) validAccountIds.insert(acc.id);
    }

    const QJsonArray txArray = root["transactions"].toArray();
    for (int i = 0; i < txArray.size(); ++i) {
        const QJsonObject o = txArray[i].toObject();
        Transaction tx;
        tx.id = o["id"].toInt(-1);
        tx.accountId = o["accountId"].toInt(-1);

        // Self-healing: ignore orphan transactions.
        if (!validAccountIds.contains(tx.accountId)) continue;

        tx.amount = o["amount"].toVariant().toLongLong();
        tx.type = static_cast<TransactionType>(o["type"].toInt(0));
        tx.status = static_cast<TransactionStatus>(o["status"].toInt(0));
        tx.memo = o["memo"].toString();
        tx.counterpartyAccount = o["counterpartyAccount"].toString();
        tx.occurredAt = QDateTime::fromMSecsSinceEpoch(o["occurredAt"].toVariant().toLongLong());
        tx.transferGroupId = o.contains("transferGroupId") ? o["transferGroupId"].toInt(-1) : -1;

        bankManager->restoreTransaction(tx);
    }

    bankManager->recalcAllBalances();
    return true;
}

bool loadFromFile(BankManager *bankManager, const QString &path, QString *errorMessage)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        setError(errorMessage, "Failed to open state file for read.");
        return false;
    }

    const QByteArray bytes = file.readAll();
    file.close();

    QJsonParseError parseError{};
    const QJsonDocument doc = QJsonDocument::fromJson(bytes, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        setError(errorMessage, "Invalid JSON state file.");
        return false;
    }

    return importState(bankManager, doc.object(), errorMessage);
}

bool saveToFile(BankManager *bankManager, const QString &path, QString *errorMessage)
{
    const QJsonObject root = exportState(bankManager);

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        setError(errorMessage, "Failed to open state file for write.");
        return false;
    }

    file.write(QJsonDocument(root).toJson());
    file.close();
    return true;
}

} // namespace SyncState
