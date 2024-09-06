#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsPathItem>
#include <QMainWindow>
#include <QMessageBox>
#include <QTcpServer>
#include <QVector>
#include <QTextStream>

#include <QTcpSocket>
#include <QFile>
#include <QStyle>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void onLineAddresTextChanged(const QString &arg1);

    void readToClient();

    void onConnectToServerClicked();

    void onSendRequestClicked();

private:
    Ui::MainWindow *m_ui;//

    QTcpSocket* m_socket;

    QByteArray m_data;               //

    QPainterPath* m_waves;           //

    QGraphicsPathItem* m_pathWaves;  //

    QGraphicsScene* m_scene;

    QFile* m_signalData;

private:
    void sendToClient();
};
#endif // MAINWINDOW_H


