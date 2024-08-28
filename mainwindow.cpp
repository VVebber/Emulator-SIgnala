#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SignalData = nullptr;
    /*Соединение*/
    socket = new QTcpSocket;
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::ReadToClient);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    /*Система кординат*/
    scene = new QGraphicsScene(this);           //Создание окла
    scene->setSceneRect(-100, -100, 200, 200);  //Размеры 200x200
    scene->addLine(-100,0,100,0);               //Линия для x
    scene->addLine(0,-100,0,100);               //Линия для y
    ui->WeveDisplay->setScene(scene);
    /*Волны*/
    Waves = new QPainterPath();
    pathWaves = new QGraphicsPathItem;
    scene->addItem(pathWaves);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete socket;

    delete Waves;
    delete pathWaves;

    delete scene;

    if(SignalData){
        if(SignalData->isOpen())
            SignalData->close();
        delete SignalData;
    }
}

//Окно ввода ip
void MainWindow::on_lineAddres_textChanged(const QString &arg1)
{
    QHostAddress address(arg1);
    QString state = "0";
    if(arg1 == "..."){
        state = "2";
    }
    else if(QAbstractSocket::IPv4Protocol == address.protocol()){
        state = "1";
    }

    ui->lineAddres->setProperty("state", state);
    style()->polish(ui->lineAddres);
    ui->lineAddres->update();
}

/*TCP/IP*/
void MainWindow::ReadToClient(){
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_6_2);
    if(in.status() == QDataStream::Ok){
        QPoint Point;
        in >> Point;
        //*Волны, установка начальной позиции
        if(Waves->elementCount() == 0){
            Waves->moveTo(Point);
        }// Удаление, волня(когда полностью отрисована)
        else if(Waves->elementCount() == 200){
            Waves->moveTo(Point);
            Waves->clear();
        }
        else
            Waves->lineTo(Point);
        pathWaves->setPath(*Waves);

        qDebug() <<"Size"<<Waves->elementCount();

        QTextStream WriteSignalData(SignalData);
        WriteSignalData<<Point.x() <<":"<<Point.y()<<"\n";
    } else
        qDebug() <<"Data error";
}

void MainWindow::SendToClient() {
    QString str = ui->TypeSignal->currentText();
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << str;
    socket->write(Data);
    qDebug() <<"Запрос отправлен";
}
/*=====*/


//Кнопка подключения
void MainWindow::on_ConnectToServer_clicked()
{
    socket->connectToHost(ui->lineAddres->text(), ui->spinPort->value());
    if (socket->waitForConnected(3000)) {//3 секунды
        qDebug() << "Подключение установлено";
        QMessageBox::about(this, "Status", "Полключено");
    } else {
        qDebug() << "Не подключено";
    }
}


void MainWindow::on_SendRequest_clicked()
{

    if(socket->isValid()){
        if(SignalData){
            SignalData = new QFile(ui->lineFileLinck->text());
            if(!SignalData->open(QIODevice::WriteOnly | QIODevice::Text))
                QMessageBox::critical(0, "Файл не открыт", "Невозможно открыть файл для записи: " + SignalData->errorString());
        }

        QTextStream WriteSignalData(SignalData);
        WriteSignalData<<"TypeSignal: " << ui->TypeSignal->currentText()<<"\n";
        /*Волны*/
        scene->removeItem(pathWaves);
        Waves->clear();
        delete Waves;
        Waves = new QPainterPath();
        pathWaves->setPath(*Waves);
        scene->addItem(pathWaves);

        qDebug() << "press";
        SendToClient();
    } else ui->label->setText("нес соединения");
}

