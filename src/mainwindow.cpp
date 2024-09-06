#include "mainwindow.h"
#include "./ui_mainwindow.h"

//#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    m_signalData = nullptr;
    //connect to server
    m_socket = new QTcpSocket;
    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::readToClient);
    connect(m_socket, &QTcpSocket::disconnected, m_socket, &QTcpSocket::deleteLater);

    connect(m_ui->ConnectToServer,&QPushButton::clicked, this, &MainWindow::onConnectToServerClicked);
    connect(m_ui->SendRequest,&QPushButton::clicked, this, &MainWindow::onSendRequestClicked);
    connect(m_ui->lineAddres, &QLineEdit::textChanged, this, &MainWindow::onLineAddresTextChanged);

    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-100, -100, 200, 200);
    m_scene->addLine(-100,0,100,0);
    m_scene->addLine(0,-100,0,100);
    m_ui->WeveDisplay->setScene(m_scene);
    /**/
    m_waves = new QPainterPath();
    m_pathWaves = new QGraphicsPathItem;
    m_scene->addItem(m_pathWaves);
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_socket;

    delete m_waves;
    delete m_pathWaves;

    delete m_scene;

    if(m_signalData)
    {
        if(m_signalData->isOpen())
        {
            m_signalData->close();
        }
        delete m_signalData;
    }
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
        if(QAbstractSocket::IPv4Protocol == address.protocol()){
            state = "1";
        }
    }

    m_ui->lineAddres->setProperty("state", state);
    style()->polish(m_ui->lineAddres);
    m_ui->lineAddres->update();
}

/*TCP/IP*/
void MainWindow::readToClient(){
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_5_15);
    if(in.status() == QDataStream::Ok){
        QPoint Point;
        in >> Point;

        if(m_waves->elementCount() == 0){
            m_waves->moveTo(Point);
        }
        else if(m_waves->elementCount() == 200)
        {
            m_waves->moveTo(Point);
            m_waves->clear();
        }
        else
            m_waves->lineTo(Point);
        m_pathWaves->setPath(*m_waves);

        qDebug() <<"Size"<<m_waves->elementCount();

        QTextStream WriteSignalData(m_signalData);
        WriteSignalData<<Point.x() <<":"<<Point.y()<<"\n";
    } else
        qDebug() <<"Data error";
}

void MainWindow::sendToClient() {
    QString typeSignal = m_ui->TypeSignal->currentText();
    m_data.clear();
    QDataStream out(&m_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << typeSignal;
    m_socket->write(m_data);
    qDebug() <<"Запрос отправлен";
}
/*=====*/

void MainWindow::onConnectToServerClicked()
{
    if(!m_socket->isOpen())
    {
        m_socket->connectToHost(m_ui->lineAddres->text(), m_ui->spinPort->value());

        if (m_socket->waitForConnected(1000))
        {
            qDebug() << "Подключение установлено";
            QMessageBox::about(this, "Status", "Полключено");
        }
        else
        {
            m_socket->close();
            qDebug() << "Не подключено";
        }
    }
    else
    {
        QMessageBox::about(this, "Status", "Полключено");
    }
}


void MainWindow::onSendRequestClicked()
{
    if(m_socket->isValid())
    {
        if(m_signalData)
        {
            m_signalData = new QFile(m_ui->lineFileLinck->text());
            if(!m_signalData->open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QMessageBox::critical(0, "Файл не открыт", "Невозможно открыть файл для записи: " + m_signalData->errorString());
            }
        }

        QTextStream writeSignalData(m_signalData);
        writeSignalData<<"TypeSignal: " << m_ui->TypeSignal->currentText()<<"\n";

        m_scene->removeItem(m_pathWaves);
        m_waves->clear();
        delete m_waves;
        m_waves = new QPainterPath();
        m_pathWaves->setPath(*m_waves);
        m_scene->addItem(m_pathWaves);

        qDebug() << "press";
        sendToClient();
    }
    else
    {
        m_ui->label->setText("нет соединения");
    }
}

