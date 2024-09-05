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

    void on_lineAddres_textChanged(const QString &arg1);

    void ReadToClient();

    void on_ConnectToServer_clicked();

    void on_SendRequest_clicked();

private:
    Ui::MainWindow *ui;

    QTcpSocket* m_socket;              //

    QByteArray m_data;               //

    QPainterPath* m_waves;           // Для отрисовки волны

    QGraphicsPathItem* m_pathWaves;  // Для отчистки волны

    QGraphicsScene* m_scene;           // Для вывода волны на экран

    QFile* m_signalData;
    void SendToClient();
};
#endif // MAINWINDOW_H


