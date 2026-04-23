#ifndef SYNC_STATE_H
#define SYNC_STATE_H

#include <QString>
#include <QJsonObject>

class BankManager;

namespace SyncState {

QJsonObject exportState(BankManager *bankManager);
bool importState(BankManager *bankManager, const QJsonObject &root, QString *errorMessage = nullptr);

bool loadFromFile(BankManager *bankManager, const QString &path, QString *errorMessage = nullptr);
bool saveToFile(BankManager *bankManager, const QString &path, QString *errorMessage = nullptr);

} // namespace SyncState

#endif // SYNC_STATE_H

