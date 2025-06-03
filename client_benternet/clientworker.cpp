#include "clientworker.h"

ClientWorker::ClientWorker(const QString& host, int subPort, int pubPort, QObject* parent)
    : QObject(parent)
    , m_context(1)
    , m_pusher(m_context, ZMQ_PUSH)
    , m_subscriber(m_context, ZMQ_SUB)
{
    try {
        // Build endpoints, e.g. "tcp://benternet.pxl-ea-ict.be:24042"
        QString subEndpoint = QString("tcp://%1:%2").arg(host).arg(subPort);
        QString pubEndpoint = QString("tcp://%1:%2").arg(host).arg(pubPort);

        m_subscriber.connect(subEndpoint.toStdString());



        m_pusher.connect(pubEndpoint.toStdString());
    }
    catch (const zmq::error_t& e) {
        emit errorOccurred(QStringLiteral("ZMQ init error: %1").arg(e.what()));
        return;
    }

    // Poll incoming every 100 ms
    connect(&m_pollTimer, &QTimer::timeout, this, &ClientWorker::pollIncoming);
    m_pollTimer.start(100);
}


void ClientWorker::subscribeAll() {
    try {
        m_subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    } catch (const zmq::error_t& e) {
        emit errorOccurred(QStringLiteral("Subscribe-all error: %1").arg(e.what()));
    }
}

void ClientWorker::unsubscribeAll() {
    try {
        m_subscriber.setsockopt(ZMQ_UNSUBSCRIBE, "", 0);
    } catch (const zmq::error_t& e) {
        emit errorOccurred(QStringLiteral("Unsubscribe-all error: %1").arg(e.what()));
    }
}

void ClientWorker::subscribeTopic(const QString& topic) {
    try {
        QByteArray utf8 = topic.toUtf8();
        m_subscriber.setsockopt(ZMQ_SUBSCRIBE,
                                utf8.constData(),
                                utf8.size());
    } catch( const zmq::error_t& e) {
        emit errorOccurred(QStringLiteral("Subscribe error : %1").arg(e.what()));
    }
}

void ClientWorker::unsubscribeTopic(const QString& topic) {
    try {
        QByteArray utf8 = topic.toUtf8();
        m_subscriber.setsockopt(ZMQ_UNSUBSCRIBE,
                                utf8.constData(),
                                utf8.size());
    } catch( const zmq::error_t& e) {
        emit errorOccurred(QStringLiteral("Unsubscribe error : %1").arg(e.what()));
    }
}


void ClientWorker::resetStats() {
    try{
        QByteArray cmd = QByteArrayLiteral("beer?>reset");
        zmq::message_t msg(cmd.constData(), cmd.size());
        if (!m_pusher.send(msg)) {
            emit errorOccurred(QStringLiteral("Failed to send reset command"));
            }
        } catch (const zmq::error_t& e) {
        emit errorOccurred(QStringLiteral("Reset push error: %1").arg(e.what()));
        }
}

ClientWorker::~ClientWorker() {
    m_pollTimer.stop();
    // sockets & context will clean up automatically
}

void ClientWorker::sendRequest(const QString& request) {
    try {
        QByteArray utf8 = request.toUtf8();
        zmq::message_t msg(utf8.constData(), utf8.size());
        bool ok = m_pusher.send(msg);
        if (!ok) {
            emit errorOccurred(QStringLiteral("Failed to send request"));
        }
    }
    catch (const zmq::error_t& e) {
        emit errorOccurred(QStringLiteral("ZMQ send error: %1").arg(e.what()));
    }
}

void ClientWorker::pollIncoming() {
    zmq::message_t msg;
    // Use DONTWAIT so we don't block
    auto result = m_subscriber.recv(&msg, ZMQ_DONTWAIT);
    if (result) {
        QString reply = QString::fromUtf8(static_cast<char*>(msg.data()), msg.size());
        emit replyReceived(reply);
    }
}
