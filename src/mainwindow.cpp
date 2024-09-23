#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QGraphicsPathItem>
#include <QMessageBox>
#include <QTextStream>
#include <QTcpServer>
#include <iostream>
#include <QStyle>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <errno.h>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);
  m_socket = nullptr;
  m_waves = nullptr;
  m_pathWaves = nullptr;
  m_scene = nullptr;

  connect(m_ui->ConnectToServer,&QPushButton::clicked, this, &MainWindow::onConnectToServerClicked);
  connect(m_ui->SendRequest,&QPushButton::clicked, this, &MainWindow::onSendRequestClicked);

  m_ui->lineAddres->setText("192.168.0.45");

  settingCoordinateSystems();
}

MainWindow::~MainWindow()
{
  delete m_ui;
  delete m_waves;
  delete m_pathWaves;

  delete m_scene;

  if(m_socket != nullptr)
  {
    m_socket->close();
    delete m_socket;
  }
}

void MainWindow::createSocket()
{
  m_socket = new QTcpSocket;

  connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::readFroClient);
  connect(m_socket, &QTcpSocket::disconnected, this, &MainWindow::deleteSocket);
}

void MainWindow::deleteSocket()
{
  disconnect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::readFroClient);
  disconnect(m_socket, &QTcpSocket::disconnected, this, &MainWindow::deleteSocket);

  if(m_ui->textBrowser)
    m_ui->textBrowser->append("disconnecting the connection");
  m_socket->close();
  m_socket->deleteLater();
  m_socket = nullptr;
}

void MainWindow::settingCoordinateSystems()
{
  connect(m_ui->lineAddres, &QLineEdit::textChanged, this, &MainWindow::onLineAddresTextChanged);

  m_scene = new QGraphicsScene(this);
  m_scene->setSceneRect(-100, -100, 200, 200);
  m_scene->addLine(-100,0,100,0);
  m_scene->addLine(0,-100,0,100);
  m_ui->WeveDisplay->setScene(m_scene);

  m_waves = new QPainterPath();
  m_pathWaves = new QGraphicsPathItem;
  m_scene->addItem(m_pathWaves);
}

void MainWindow::onLineAddresTextChanged(const QString &arg1)
{
  QHostAddress address(arg1);
  QString state = "0";
  if(arg1 == "...")
  {
    state = "2";
  }
  else
  {
    if(QAbstractSocket::IPv4Protocol == address.protocol())
    {
      state = "1";
    }
  }

  m_ui->lineAddres->setProperty("state", state);
  style()->polish(m_ui->lineAddres);
  m_ui->lineAddres->update();
}

void MainWindow::readFroClient()
{
  QTcpSocket* clientSocket = (QTcpSocket*)sender();
  QDataStream in(clientSocket);
  in.setVersion(QDataStream::Qt_5_15);
  if(in.status() == QDataStream::Ok){
    QPoint Point;
    in >> Point;

    if(m_waves->elementCount() == 0)
    {
      m_waves->moveTo(Point);
    }
    else if(m_waves->elementCount() == 200)
    {
      m_waves->moveTo(Point);
      m_waves->clear();
    }
    else
    {
      m_waves->lineTo(Point);
    }
    m_pathWaves->setPath(*m_waves);
  }
  else
  {
    qDebug() <<"Data error";
  }
}

void MainWindow::sendToClient()
{
  QString typeSignal = m_ui->TypeSignal->currentText();
  QByteArray data;
  data.clear();
  QDataStream out(&data, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_15);
  out << typeSignal;
  m_socket->write(data);
  qDebug() <<"Request sent";
}

void MainWindow::onConnectToServerClicked()
{
  if(m_socket)
  {

    if(m_socket->peerAddress().toString() != m_ui->lineAddres->text() ||
       m_socket->peerPort() != m_ui->spinPort->value())
    {
      m_ui->textBrowser->append("reconnecting to the server");
      deleteSocket();
    }
  }
  if(!m_socket)
  {
    createSocket();
  }


  if(!m_socket->isOpen())
  {
    m_socket->connectToHost(m_ui->lineAddres->text(), m_ui->spinPort->value());

    //m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    if (m_socket->waitForConnected(1000))
    {
      m_ui->textBrowser->append(QString("Подключение установлено: ip %1, port %2")
                                .arg(m_ui->lineAddres->text())
                                .arg(m_ui->spinPort->value()));

      int intSocketDescriptor = m_socket->socketDescriptor();

      int keepAlive = 1;
      if (setsockopt(intSocketDescriptor, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive)) == -1) {
        qWarning() << "1Failed to set keep_alive."<<strerror(errno);
      }

      int keepIdle = 10;
      if (setsockopt(intSocketDescriptor, IPPROTO_TCP, TCP_KEEPALIVE, &keepIdle, sizeof(keepIdle)) == -1) {
        qWarning() << "2Failed to set TCP_KEEPIDLE."<<strerror(errno);
      }

      int keepInterval = 5;
      if (setsockopt(intSocketDescriptor, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(keepInterval)) == -1) {
        qWarning() << "3Failed to set TCP_KEEPINTVL."<<strerror(errno);
      }

      int keepCount = 2;
      if (setsockopt(intSocketDescriptor, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(keepCount)) == -1) {
        qWarning() << "4Failed to set TCP_KEEPCNT."<<strerror(errno);
      }
    }
    else
    {
      m_socket->close();
      m_ui->textBrowser->append(QString("Не подключено"));
    }
  }
  else
  {
    QMessageBox::about(this, "Status", "Полключено");
  }
}

void MainWindow::onSendRequestClicked()
{
  if(m_socket != nullptr)
  {
    if(m_socket->isOpen())
    {
      m_scene->removeItem(m_pathWaves);
      m_waves->clear();
      delete m_waves;
      m_waves = new QPainterPath();
      m_pathWaves->setPath(*m_waves);
      m_scene->addItem(m_pathWaves);

      qDebug() << "press";
      sendToClient();
    }
  }
  else
  {
    m_ui->textBrowser->append("Unable to send request, no connection.");
  }
}

