#ifndef SYNC_SERVER_H
#define SYNC_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <QJsonObject>

class BankManager;

class SyncServer : public QObject
{
    Q_OBJECT

public:
    explicit SyncServer(BankManager *bankManager, QObject *parent = nullptr);

    bool start(quint16 port, QString *errorMessage = nullptr);
    void stop();
    bool isRunning() const;

    quint16 port() const;
    qint64 version() const;

    void setPersistencePath(const QString &path);

    // Call this when the local UI mutates the same BankManager while server is running.
    void notifyExternalMutation();

signals:
    void runningChanged(bool running);
    void stateMutated(); // triggered for remote mutations

private:
    struct ClientState {
        QByteArray buffer;
        bool subscribed = false;
    };

    BankManager *m_bankManager;
    QTcpServer m_server;
    QHash<QTcpSocket*, ClientState> m_clients;
    QString m_persistencePath;

    qint64 m_version = 1;

    void onNewConnection();
    void onSocketReadyRead(QTcpSocket *socket);
    void onSocketDisconnected(QTcpSocket *socket);

    void handleLine(QTcpSocket *socket, const QByteArray &line);
    void sendResponse(QTcpSocket *socket, int reqId, bool ok, const QJsonObject &data, const QJsonObject &error);
    void sendPush(QTcpSocket *socket, const QString &type, const QJsonObject &data);
    void broadcastChanged();
    void bumpVersionAndBroadcast();

    bool saveNow();
};

#endif // SYNC_SERVER_H
