#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QTextEdit;
QT_END_NAMESPACE

class ClientWorker;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

signals:
    void sendRequest(const QString& request);
    void subscribeRequest(const QString& topic);
    void unsubscribeRequest(const QString& topic);
    void subscribeAllRequest();
    void unsubscribeAllRequest();
    void resetRequest();


private slots:
    void onSendClicked();
    void onSubscribeClicked();
    void onUnsubscribeClicked();
    void handleReply(const QString& reply);
    void handleError(const QString& error);

private:
    QLineEdit*    m_input;
    QPushButton*  m_sendButton;
    QPushButton*  m_subButton;
    QPushButton*  m_unsubButton;
    QPushButton*  m_resetButton;
    QPushButton*  m_subAllButton;
    QTextEdit*    m_log;

    ClientWorker*   m_worker;
    QThread         m_workerThread;
};

#endif // MAINWINDOW_H
