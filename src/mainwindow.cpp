#include "mainwindow.h"
#include "./ui_mainwindow.h"
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

  m_ui->lineAddres->setText("127.0.0.1");
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
  QList<float> points;
  if(m_ui->comboBox->currentText() == "XML")
  {
    QDomDocument main;
    main.setContent(m_socket->readAll());
    QDomElement box = main.documentElement();
    QString command = box.elementsByTagName("command").at(0).toElement().text();
    QDomNodeList variableDataNodes = box.elementsByTagName("VariableData").at(0).childNodes();

    QString textMessege = box.toText().data();
    if(command == "point for graphing function")
    {
      for (int i = 0; i < variableDataNodes.size(); ++i)
      {
        points.push_back(variableDataNodes.at(i).toElement().childNodes().at(0).toText().data().toInt());
      }
      drawPoint(points);
    }
  }
  else
  {
    QJsonObject message;
    QJsonDocument document;
    document = QJsonDocument::fromJson(m_socket->readAll());

    message = document.object();
    qDebug() <<"read "<<message;

    if(message.contains("command"))
    {
      QString command = message["command"].toString();
      if(command == "point for graphing function")
      {
        for(size_t i = 0; i < message["VariableData"].toArray().size(); i++)
        {
          points.push_back(message["VariableData"].toArray().at(i).toDouble());
        }
        drawPoint(points);
      }
    }
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
    if (setsockopt(intSocketDescriptor, IPPROTO_TCP, TCP_KEEPALIVE, &keepIdle, sizeof(keepIdle)) == -1)
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

    sendToClient("type of protocol");
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
      m_scene->removeItem(m_pathWaves);
      m_waves->clear();
      delete m_waves;
      m_waves = new QPainterPath();
      m_pathWaves->setPath(*m_waves);
      m_scene->addItem(m_pathWaves);

      qDebug() << "press";
      sendToClient("setting the type of signal", m_ui->typeSignal->currentText());
    }
  }
  else
  {
    m_ui->textBrowser->append("Unable to send request, no connection.");
  }
}


void MainWindow::sendToClient(QString commandText, QString VariableData)
{
  if(m_ui->comboBox->currentText() == "XML")
  {
    QDomDocument XMLDocument;
    QDomElement root = XMLDocument.createElement("task");

    QDomElement command = XMLDocument.createElement("command");
    QDomText comm = XMLDocument.createTextNode(commandText);

    root.appendChild(command);
    command.appendChild(comm);

    QDomElement variableData = XMLDocument.createElement("VariableData");
    QDomText variableDataText = XMLDocument.createTextNode(VariableData);

    root.appendChild(variableData);
    variableData.appendChild(variableDataText);

    XMLDocument.appendChild(root);

    qDebug()<<XMLDocument.toByteArray(0);

    m_socket->write(XMLDocument.toByteArray());
  }
  else
  {
    QJsonObject message;
    message["command"] = commandText;
    if(!VariableData.isEmpty())
    {
      message["VariableData"] = VariableData;
    }
    QJsonDocument JSONDocument(message);
    m_socket->write(JSONDocument.toJson());
    qDebug() <<"Request sent: "<< message;
  }
}

void MainWindow::onCheckBoxClicked()
{
  /*
   *     QDomDocument XMLDocument;
    QDomElement root = XMLDocument.createElement("task");

    <?xml encoding="utf-8" version="1.0" ?>
    <Task>
      <commad>command name</command>
      <VariableData>awadw</VariableData>
      <Variableaw>awadw</VariableDaawd>
    </Task>
  */

  if(m_socket != nullptr)
  {
    //qDebug() << "pressed draw point";
    QJsonObject typeSignal;
    typeSignal["command"] = "setting draw point";
    sendToClient("setting draw point");
    qDebug()<<typeSignal;
  }
}

