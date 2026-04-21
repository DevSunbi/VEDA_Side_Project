#ifndef SYNC_CLIENT_H
#define SYNC_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHash>
#include <QJsonObject>
#include <functional>

class SyncClient : public QObject
{
    Q_OBJECT

public:
    explicit SyncClient(QObject *parent = nullptr);

    void connectToHost(const QString &host, quint16 port);
    void disconnectFromHost();
    bool isConnected() const;

    int requestState(std::function<void(bool ok, const QJsonObject &data, const QJsonObject &error)> cb);
    int subscribe(std::function<void(bool ok, const QJsonObject &data, const QJsonObject &error)> cb);

    int createAccount(const QString &accountNumber,
                      const QString &password,
                      qint64 initialBalance,
                      std::function<void(bool ok, const QJsonObject &data, const QJsonObject &error)> cb);

    int deleteAccount(const QString &accountNumber,
                      const QString &password,
                      std::function<void(bool ok, const QJsonObject &data, const QJsonObject &error)> cb);

    int deposit(const QString &accountNumber,
                const QString &password,
                qint64 amount,
                const QString &memo,
                std::function<void(bool ok, const QJsonObject &data, const QJsonObject &error)> cb);

    int withdraw(const QString &accountNumber,
                 const QString &password,
                 qint64 amount,
                 const QString &memo,
                 std::function<void(bool ok, const QJsonObject &data, const QJsonObject &error)> cb);

    int transfer(const QString &fromAccountNumber,
                 const QString &password,
                 const QString &toAccountNumber,
                 qint64 amount,
                 const QString &memo,
                 std::function<void(bool ok, const QJsonObject &data, const QJsonObject &error)> cb);

signals:
    void connected();
    void disconnected();
    void socketError(const QString &message);
    void changed(qint64 version);

private:
    QTcpSocket m_socket;
    QByteArray m_buffer;
    int m_nextReqId = 1;

    using Callback = std::function<void(bool, const QJsonObject&, const QJsonObject&)>;
    QHash<int, Callback> m_pending;

    void onReadyRead();
    void handleLine(const QByteArray &line);

    int sendRequest(const QString &type, const QJsonObject &data, Callback cb);
};

#endif // SYNC_CLIENT_H
