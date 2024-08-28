#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket;
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::ReadToClient);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    scene = new QGraphicsScene(this);           //Создание окла
    scene->setSceneRect(-100, -100, 200, 200);  //Размеры 200x200
    scene->addLine(-100,0,100,0);               //Линия для x
    scene->addLine(0,-100,0,100);               //Линия для y
    Waves = new QPainterPath();

    ui->WeveDisplay->setScene(scene);

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

        if(Waves->elementCount() == 0){
            Waves->moveTo(Point);
        }
        else
            Waves->lineTo(Point);
        pathWaves->setPath(*Waves);

        QTextStream WriteSignalData(SignalData);
        WriteSignalData<<Point.x() <<":"<<Point.y()<<"\n";

        qDebug() <<"size" << Waves->elementCount();
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
    if (socket->waitForConnected(3000)) {  // Ожидаем подключения в течение 3 секунд
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

        if(Waves->elementCount() == 0 || Waves->elementCount() >= 200){//=>200, иногда размер 200 или 201
            QTextStream WriteSignalData(SignalData);
            WriteSignalData<<"TypeSignal: " << ui->TypeSignal->currentText()<<"\n";

            scene->removeItem(pathWaves);  // Удаление волны из сцены

            Waves->clear();                // Отчиска волдны
            delete Waves;
            Waves = new QPainterPath();

            pathWaves->setPath(*Waves);    // Добавление волны в объект для отрисовки
            scene->addItem(pathWaves);     // Добавление объекта

            SendToClient();                // Отправка сообщения
        } else ui->label->setText("Подождите, процесс не завершен");
    } else ui->label->setText("нес соединения");
}

