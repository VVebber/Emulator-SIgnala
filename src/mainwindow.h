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
    void deleteSocket();
    void onConnectToServerClicked();
    void onSendRequestClicked();

private:
    Ui::MainWindow *m_ui;
    QTcpSocket* m_socket;
    QPainterPath* m_waves;
    QGraphicsPathItem* m_pathWaves;
    QGraphicsScene* m_scene;

private:
    void createSocket();
    void sendToClient();
    void settingCoordinateSystems();
};
#endif // MAINWINDOW_H


