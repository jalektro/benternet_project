#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <vector>
#include <zmq.hpp>


class ClientWorker : public QObject {
    Q_OBJECT

public:
    explicit ClientWorker(const QString& host, int subPort, int pubPort, QObject* parent = nullptr);
    ~ClientWorker();

public slots:
    // Enqueue any request of the form "service?>payload"
    void sendRequest(const QString& request);
    void subscribeAll();
    void unsubscribeAll();
    void subscribeTopic(const QString& topic);
    void unsubscribeTopic(const QString& topic);

signals:
    // Emitted when any reply arrives (topics starting with "service!>")
    void replyReceived(const QString& reply);
    // Emitted on any ZMQ error
    void errorOccurred(const QString& errorMsg);


private slots:
    // Poll SUB socket non-blocking every timeout ms
    void pollIncoming();

private:
    zmq::context_t   m_context;
    zmq::socket_t    m_pusher;      // PUSH socket to send requests
    zmq::socket_t    m_subscriber;  // SUB socket to receive replies
    QTimer           m_pollTimer;
};

#endif // CLIENTWORKER_H
