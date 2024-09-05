#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_signalData = nullptr;
    /*Соединение*/
    m_socket = new QTcpSocket;
    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::ReadToClient);
    connect(m_socket, &QTcpSocket::disconnected, m_socket, &QTcpSocket::deleteLater);
    /*Система кординат*/
    m_scene = new QGraphicsScene(this);           //Создание окла
    m_scene->setSceneRect(-100, -100, 200, 200);  //Размеры 200x200
    m_scene->addLine(-100,0,100,0);               //Линия для x
    m_scene->addLine(0,-100,0,100);               //Линия для y
    ui->WeveDisplay->setScene(m_scene);
    /*Волны*/
    m_waves = new QPainterPath();
    m_pathWaves = new QGraphicsPathItem;
    m_scene->addItem(m_pathWaves);
}

MainWindow::~MainWindow()
{
    delete ui;
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

//Окно ввода ip
void MainWindow::on_lineAddres_textChanged(const QString &arg1)
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

    ui->lineAddres->setProperty("state", state);
    style()->polish(ui->lineAddres);
    ui->lineAddres->update();
}

/*TCP/IP*/
void MainWindow::ReadToClient(){
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_5_15);
    if(in.status() == QDataStream::Ok){
        QPoint Point;
        in >> Point;
        //*Волны, установка начальной позиции
        if(m_waves->elementCount() == 0){
            m_waves->moveTo(Point);
        }// Удаление, волня(когда полностью отрисована)
        else if(m_waves->elementCount() == 200){
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

void MainWindow::SendToClient() {
    QString typeSignal = ui->TypeSignal->currentText();
    m_data.clear();
    QDataStream out(&m_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << typeSignal;
    m_socket->write(m_data);
    qDebug() <<"Запрос отправлен";
}
/*=====*/


//Кнопка подключения
void MainWindow::on_ConnectToServer_clicked()
{
    m_socket->connectToHost(ui->lineAddres->text(), ui->spinPort->value());
    if (m_socket->waitForConnected(3000))
    {
        qDebug() << "Подключение установлено";
        QMessageBox::about(this, "Status", "Полключено");
    }
    else
    {
        qDebug() << "Не подключено";
    }
}


void MainWindow::on_SendRequest_clicked()
{
    if(m_socket->isValid())
    {
        if(m_signalData)
        {
            m_signalData = new QFile(ui->lineFileLinck->text());
            if(!m_signalData->open(QIODevice::WriteOnly | QIODevice::Text)){
                QMessageBox::critical(0, "Файл не открыт", "Невозможно открыть файл для записи: " + m_signalData->errorString());
            }
        }

        QTextStream writeSignalData(m_signalData);
        writeSignalData<<"TypeSignal: " << ui->TypeSignal->currentText()<<"\n";
        /*Волны*/
        m_scene->removeItem(m_pathWaves);
        m_waves->clear();
        delete m_waves;
        m_waves = new QPainterPath();
        m_pathWaves->setPath(*m_waves);
        m_scene->addItem(m_pathWaves);

        qDebug() << "press";
        SendToClient();
    }
    else
    {
        ui->label->setText("нет соединения");
    }
}

