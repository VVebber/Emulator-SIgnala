#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QGraphicsPathItem;
class QGraphicsScene;
//class Q

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void onLineAddresTextChanged(const QString &arg1);
  void readFroClient();
  void deleteSocket();
  void onConnectToServerClicked();
  void onSendRequestClicked();

  void onCheckBoxClicked();

private:
  Ui::MainWindow *m_ui;
  QTcpSocket* m_socket;
  QPainterPath* m_waves;
  QGraphicsPathItem* m_pathWaves;
  QGraphicsScene* m_scene;

private:
  void createSocket();
  void sendToClient(QJsonObject message);
  void settingCoordinateSystems();
};
#endif // MAINWINDOW_H


