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
    //[clazy-connect-by-name], Название методов, делала IDE
    void on_lineAddres_textChanged(const QString &arg1);

    void ReadToClient();

    void on_ConnectToServer_clicked();

    void on_SendRequest_clicked();

private:
    Ui::MainWindow *ui;

    QTcpSocket* socket;            //
    QByteArray Data;               //

    QPainterPath* Waves;           // Для отрисовки волны
    QGraphicsPathItem* pathWaves;  // Для отчистки волны

    QGraphicsScene* scene;         // Для вывода волны на экран

    QFile* SignalData;
    void SendToClient();
};
#endif // MAINWINDOW_H


