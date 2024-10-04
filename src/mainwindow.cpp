#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "protocoljson.h"
#include "protocolxml.h"
#include "protocol.h"
#include "command.h"

#include <QGraphicsPathItem>
#include <QJsonDocument>
#include <QMessageBox>
#include <QTextStream>
#include <QJsonArray>
#include <QTcpServer>
#include <QStyle>
#include <QString>
#include <QList>
#include <QtXml>

#include <sys/socket.h>
#include <unistd.h>
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
  connect(m_ui->checkBox,&QPushButton::clicked, this, &MainWindow::onCheckBoxClicked);
  connect(m_ui->comboBox, qOverload<int>(&QComboBox::activated), this, &MainWindow::onComboBoxActivated);

  m_ui->lineAddres->setText("192.168.0.45");
  settingCoordinateSystems();

  m_protocol = new ProtocolJSON;
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

  connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::readFromServer);
  connect(m_socket, &QTcpSocket::disconnected, this, &MainWindow::deleteSocket);
}

void MainWindow::deleteSocket()
{
  disconnect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::readFromServer);
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


void MainWindow::readFromServer()
{
  QList<float> points;

  m_protocol->addData(m_socket->readAll());
  Command command = m_protocol->getNextCommand();
  while(command.isValid())
  {
    switch (command.getCommandType())
    {
    case Command::CommandType::PointGraphing:
      for(int i = 0; i < command.size(); i++){
        points.push_back(command.atVariableData(i));
      }
      drawPoint(points);
      break;
    default:
      qDebug() <<"the request is not understood";
      break;
    }

    command = m_protocol->getNextCommand();
  }
}

void MainWindow::drawPoint(QList<float> points)
{
  QPoint point;
  if(points.size()%2 != 0)
  {
    qDebug() << "problem with points";
    points.removeLast();
  }

  for(size_t i = 0; i < points.size(); i+=2)
  {
    point.setX(points.at(i));
    point.setY(points.at(i + 1));
    if(m_waves->elementCount() == 200)
    {
      m_waves->moveTo(point);
      m_waves->clear();
    }
    else
    {
      m_waves->lineTo(point);
    }
    m_pathWaves->setPath(*m_waves);
  }
}

void MainWindow::onConnectToServerClicked()
{
  if(m_socket)
  {
    m_ui->textBrowser->append("reconnecting to the server");
    deleteSocket();
    m_ui->ConnectToServer->setText("Connect");
    m_ui->comboBox->setDisabled(false);
    return;
  }

  createSocket();
  m_ui->ConnectToServer->setText("Disconnect");
  m_ui->comboBox->setDisabled(true);

  m_socket->connectToHost(m_ui->lineAddres->text(), m_ui->spinPort->value());

  if (m_socket->waitForConnected(1000))
  {
    m_ui->textBrowser->append(QString("Подключение установлено: ip %1, port %2")
                              .arg(m_ui->lineAddres->text())
                              .arg(m_ui->spinPort->value()));

    int intSocketDescriptor = m_socket->socketDescriptor();

    int keepAlive = 1;
    if (setsockopt(intSocketDescriptor, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive)) == -1)
    {
      qWarning() << "1Failed to set keep_alive."<<strerror(errno);
    }

    int keepIdle = 10;
    if (setsockopt(intSocketDescriptor, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(keepIdle)) == -1)
    { //for mac TCP_KEEPALIVE
      qWarning() << "2Failed to set TCP_KEEPIDLE."<<strerror(errno);
    }

    int keepInterval = 5;
    if (setsockopt(intSocketDescriptor, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(keepInterval)) == -1)
    {
      qWarning() << "3Failed to set TCP_KEEPINTVL."<<strerror(errno);
    }

    int keepCount = 2;
    if (setsockopt(intSocketDescriptor, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(keepCount)) == -1)
    {
      qWarning() << "4Failed to set TCP_KEEPCNT."<<strerror(errno);
    }
  }
  else
  {
    m_socket->close();
    deleteSocket();
    m_ui->textBrowser->append(QString("Не подключено"));
    m_ui->ConnectToServer->setText("Connect");
    m_ui->comboBox->setDisabled(false);
  }
}

void MainWindow::onSendRequestClicked()
{
  if(m_socket != nullptr)
  {
    if(m_socket->isOpen())
    {
      //clear pointer
      m_scene->removeItem(m_pathWaves);
      m_waves->clear();
      delete m_waves;
      m_waves = new QPainterPath();
      m_pathWaves->setPath(*m_waves);
      m_scene->addItem(m_pathWaves);

      auto data = m_protocol->encode(Command::CommandType::TypeSignalSetting, m_ui->typeSignal->currentText());
      m_socket->write(data.mid(0, 10));
      m_socket->flush();
      m_socket->write(data.mid(10, -1));
    }
  }
  else
  {
    m_ui->textBrowser->append("Unable to send request, no connection.");
  }
}

void MainWindow::onCheckBoxClicked()
{
  if(m_socket != nullptr)
  {
    m_socket->write(m_protocol->encode(Command::CommandType::DrawStartOrFin, ""));
  }
}

void MainWindow::onComboBoxActivated(int index)
{
  if(m_ui->comboBox->currentText() == "XML")
  {
    delete m_protocol;
    m_protocol = new ProtocolXML;
  }
  else
  {
    delete m_protocol;
    m_protocol = new ProtocolJSON;
  }
}

