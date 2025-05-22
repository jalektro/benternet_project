#include "mainwindow.h"
#include "clientworker.h"

#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_input(new QLineEdit(this))
    , m_sendButton(new QPushButton(tr("Send"), this))
    , m_subButton(new QPushButton(tr("Subscribe"), this))
    , m_unsubButton(new QPushButton(tr("Unsubscribe"), this))
    , m_subAllButton(new QPushButton(tr("Subscribe All"), this))
    , m_log(new QTextEdit(this))
    , m_worker(new ClientWorker("benternet.pxl-ea-ict.be", 24042, 24041))
{
    // Build UI
    auto* central = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(central);

    // First row: input + send
    auto* row1 = new QHBoxLayout;
    row1->addWidget(m_input);
    row1->addWidget(m_sendButton);
    mainLayout->addLayout(row1);

    // Second row: sub / unsub / sub-all
    auto* row2 = new QHBoxLayout;
    row2->addWidget(m_subButton);
    row2->addWidget(m_unsubButton);
    row2->addWidget(m_subAllButton);
    mainLayout->addLayout(row2);

    // Log area
    mainLayout->addWidget(m_log);

    setCentralWidget(central);
    m_log->setReadOnly(true);

    // Move worker to its thread
    m_worker->moveToThread(&m_workerThread);
    m_workerThread.start();

    // Wire up
    connect(m_sendButton, &QPushButton::clicked,
            this, &MainWindow::onSendClicked);
    connect(m_subButton, &QPushButton::clicked,
            this, &MainWindow::onSubscribeClicked);
    connect(m_unsubButton, &QPushButton::clicked,
            this, &MainWindow::onUnsubscribeClicked);

    // Subscribe All toggle setup
    m_subAllButton->setCheckable(true);
    connect(m_subAllButton, &QPushButton::toggled, this, [this](bool c){
        if (c) {
            m_log->append(tr("Subscribing to ALL topics"));
            emit subscribeAllRequest();
            m_subAllButton->setText(tr("Unsubscribe All"));
        } else {
            m_log->append(tr("Unsubscribing from ALL topics"));
            emit unsubscribeAllRequest();
            m_subAllButton->setText(tr("Subscribe All"));
        }
    });

    connect(this, &MainWindow::sendRequest,
            m_worker, &ClientWorker::sendRequest);
    connect(this, &MainWindow::subscribeRequest,
            m_worker, &ClientWorker::subscribeTopic);
    connect(this, &MainWindow::unsubscribeRequest,
            m_worker, &ClientWorker::unsubscribeTopic);
    connect(this, &MainWindow::subscribeAllRequest,
            m_worker, &ClientWorker::subscribeAll);
    connect(this, &MainWindow::unsubscribeAllRequest,
            m_worker, &ClientWorker::unsubscribeAll);

    connect(m_worker, &ClientWorker::replyReceived,
            this, &MainWindow::handleReply);
    connect(m_worker, &ClientWorker::errorOccurred,
            this, &MainWindow::handleError);

    // Cleanup
    connect(this, &QObject::destroyed,
            m_worker, &ClientWorker::deleteLater);
}

MainWindow::~MainWindow() {
    m_workerThread.quit();
    m_workerThread.wait();
}

void MainWindow::onSendClicked() {
    QString txt = m_input->text().trimmed();
    if (txt.isEmpty()) {
        QMessageBox::warning(this, tr("Input Error"),
                             tr("Please enter a service?>payload or topic."));
        return;
    }
    m_log->append(tr("Sending: %1").arg(txt));
    emit sendRequest(txt);
}

void MainWindow::onSubscribeClicked() {
    QString topic = m_input->text().trimmed();
    if (topic.isEmpty()) {
        QMessageBox::warning(this, tr("Input Error"),
                             tr("Enter topic to subscribe."));
        return;
    }
    m_log->append(tr("Subscribing to topic: %1").arg(topic));
    emit subscribeRequest(topic);
}

void MainWindow::onUnsubscribeClicked() {
    QString topic = m_input->text().trimmed();
    if (topic.isEmpty()) {
        QMessageBox::warning(this, tr("Input Error"),
                             tr("Enter topic to unsubscribe."));
        return;
    }
    m_log->append(tr("Unsubscribing from topic: %1").arg(topic));
    emit unsubscribeRequest(topic);
}

void MainWindow::handleReply(const QString& reply) {
    m_log->append(tr("Received: %1").arg(reply));
}

void MainWindow::handleError(const QString& error) {
    QMessageBox::critical(this, tr("ZMQ Error"), error);
}

