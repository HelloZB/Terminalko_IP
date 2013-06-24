#include "widget.h"
#include "ui_widget.h"
//#include <QDebug>
#include <QMessageBox>
#include <QSettings>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    loadSettings();
    if(ipList.isEmpty()) {
        QString ipAddress;
        QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
        // use the first non-localhost IPv4 address

        for (int i = 0; i < ipAddressesList.size(); ++i) {
            ipList<<ipAddressesList.at(i).toString();
            if (ipAddress.isEmpty() && ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()) {
                ipAddress = ipAddressesList.at(i).toString();
                break;
            }
        }
        // if we did not find one, use IPv4 localhost
        if (ipAddress.isEmpty())
            ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
        ui->leIP->setText(ipAddress);
    }
    socket=new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(vyvidReadData()));
    connect(socket, SIGNAL(disconnected()), this , SLOT(discReaktsiya()));
    connect(socket, SIGNAL(connected()), this, SLOT(zednavs()));

    connect(ui->leSend, SIGNAL(returnPressed()), this, SLOT(writeToServer()));


    if(complList.isEmpty())
        complList<<"ATID"<<"ATCH"<<"ATPL"<<"ATRE"<<"ATCN"<<"ATAC"<<"ATFR"<<"ATAD"<<"ATVR"<<"ATSH"<<"ATSL";

    myIPCompl=new QCompleter(ipList,this);
    myIPCompl->setCaseSensitivity(Qt::CaseInsensitive);
    ui->leIP->setCompleter(myIPCompl);

    myCompl=new QCompleter(complList, this);
        myCompl->setCaseSensitivity(Qt::CaseInsensitive);

    ui->leSend->setCompleter(myCompl);
}
//-----------------------------------------------------------------------------------------------------------
Widget::~Widget()
{
    saveSettings();
    delete ui;
}
//-----------------------------------------------------------------------------------------------------------
void Widget::discReaktsiya()
{
    ui->pbConDisc->setChecked(0);
    ui->leSend->setEnabled(0);
    ui->plainTextEdit->appendHtml(tr("<i> Disconnected.</i>"));
    //socket->close();
}
//-----------------------------------------------------------------------------------------------------------
void Widget::vyvidReadData()
{
    quint16 qaitTime=ui->spinBox->value();
    QByteArray readArr=socket->readAll();
    while (readArr.right(2)!="\r\n" && socket->waitForReadyRead(qaitTime)) {
        readArr+=socket->readAll();
    }

    if(!readArr.isEmpty()){
        ui->plainTextEdit->moveCursor(QTextCursor::End);
        QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
        textCharFormat.setFontItalic(0);
        textCharFormat.setFontWeight(0);
        ui->plainTextEdit->setCurrentCharFormat(textCharFormat);

        ui->plainTextEdit->insertPlainText(readArr);
        ui->plainTextEdit->moveCursor(QTextCursor::End);
    }
     //   ui->plainTextEdit->appendHtml(tr("<p>%1</p>").arg(readArr));
}
//----------------------------------------------------
void Widget::on_pbConDisc_clicked(bool checked)
{
    if(checked){
        addToIpCompl();
        socket->connectToHost(ui->leIP->text(), ui->sbPort->value());
        if(!socket->waitForConnected(ui->spinBox->value())){
            ui->pbConDisc->setChecked(0);
            ui->plainTextEdit->appendHtml(tr("<i><b>Not connected!</b></i>"));
        }

    }else{
        socket->close();
        //socket->disconnect();
    }
}


//---------------------------------------------------------------------
void Widget::writeToServer()
{
    QByteArray data=ui->leSend->text().toLocal8Bit();
    switch(ui->comboBox->currentIndex()){
    case 0: {data+="\r\n"; break ; }
    case 1: {data+="\r"; break ; }
    case 2: {data+="\n"; break ; }
    default: { break ; }
    }

    socket->write(data);

    ui->plainTextEdit->moveCursor(QTextCursor::End);

    QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
    textCharFormat.setFontItalic(0);
    textCharFormat.setFontWeight(QFont::Bold);
    ui->plainTextEdit->setCurrentCharFormat(textCharFormat);

    ui->plainTextEdit->insertPlainText("\r\n"+ui->leSend->text()+"\r\n");
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    //ui->plainTextEdit->appendHtml(tr("<b>%1</b>").arg(ui->leSend->text()));

    QString str2=ui->leSend->text().toUpper();
    if(!str2.isEmpty() && str2.left(3)!="+++" && str2.left(3)!="&&&") {


        for(qint32 i=0; i<complList.count(); i++){
            if(str2==complList[i].toUpper()){
                complList.removeAt(i);
                break;
            }

        }
    }
     selPosition=0;
    QTimer::singleShot(50, this, SLOT(clearLine()));

}

//-----------------------------------------------------------------------------------------------------------
void Widget::zednavs()
{
    ui->leSend->setEnabled(1);
    ui->pbConDisc->setChecked(1);
    ui->plainTextEdit->appendHtml(tr("<i> Connected! IP: %1, Port: %2</i><br>").arg(ui->leIP->text()).arg(ui->sbPort->value()));
    ui->leSend->setFocus();
}

//-----------------------------------------------------------------------------------------------------------
void Widget::clearLine()
{
    if(ui->leSend->isEnabled()){
        if(!ui->leSend->text().isEmpty() && ui->leSend->text().left(3)!="+++" && ui->leSend->text().left(3)!="&&&"){
            delete myCompl;
            if(selPosition)
                complList.insert(0,ui->leSend->text().left(ui->leSend->text().count()-1));
            else
                complList.insert(0,ui->leSend->text());

            myCompl=new QCompleter(complList, this);
            myCompl->setCaseSensitivity(Qt::CaseInsensitive);
            ui->leSend->setCompleter(myCompl);
        }
        ui->leSend->clear();
        if(ui->leSend->isEnabled())
            ui->leSend->setFocus();
    }

}

//-----------------------------------------------------------------------------------------------------------

void Widget::on_leSend_textChanged(const QString &arg1)
{
    if(arg1.right(1)==ui->leNone->text())
        {
            QByteArray data=arg1.left(arg1.count()-1).toLocal8Bit();
            if(!data.isEmpty() && data.left(3)!="+++" && data.left(3)!="&&&") {

                QString str=arg1.left(arg1.count()-1).toUpper();
                for(qint32 i=0; i<complList.count(); i++){
                    if(str==complList[i].toUpper()){
                        complList.removeAt(i);
                        break;
                    }

                }
            }

            ui->plainTextEdit->moveCursor(QTextCursor::End);
            QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
            textCharFormat.setFontItalic(0);
            textCharFormat.setFontWeight(QFont::Bold);
            ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
            ui->plainTextEdit->insertPlainText("\r\n"+data+"\r\n");
            ui->plainTextEdit->moveCursor(QTextCursor::End);


            switch (ui->comboBox_2->currentIndex()) {
            case 0: { data+="\r\n"; break; }
            case 1: { data+="\r" ; break; }
            case 2: { data+="\n"; break; }
            }
            socket->write(data);
            selPosition=1;
            QTimer::singleShot(50, this, SLOT(clearLine()));
        }
        else {
            if(ui->checkBox->isChecked() &&( arg1.left(3)=="+++" || arg1.left(3)=="&&&")){
                QByteArray data=arg1.toLocal8Bit();
                ui->plainTextEdit->moveCursor(QTextCursor::End);
                QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
                textCharFormat.setFontItalic(0);
                textCharFormat.setFontWeight(QFont::Bold);
                ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
                ui->plainTextEdit->insertPlainText("\r\n"+data+"\r\n");
                ui->plainTextEdit->moveCursor(QTextCursor::End);
                data+="\r\n";
                socket->write(data);
                ui->leSend->clear();
            }
        }
}
//-----------------------------------------------------------------------------------------------------------
void Widget::on_toolButton_toggled(bool checked)
{
    if(checked){
        ui->widget->setHidden(0);
    } else {
        ui->widget->hide();
    }

}
//-----------------------------------------------------------------------------------------------------------

void Widget::on_toolButton_2_clicked()
{
    QMessageBox::about(this, tr("About Terminalko_IP"),
                           tr("<b>Simple TCP terminal.</b><br>"
                              "build date 2013-05-24<br>")+QString(QString::fromUtf8("©"))+tr(" Bogdan Zikranets, 2013"));
}
//-----------------------------------------------------------------------------------------------------------
void Widget::loadSettings()
{
    QSettings setting ("Hello_ZB","Qt_Terminalko_IP");
    setting.beginGroup("Terminalko_IP");
    QRect rect = setting.value("position").toRect();
    if(rect.x()< 10)
        rect.setX(10);
    if(rect.y()<30)
        rect.setY(31);
     setGeometry(rect);

     ui->sbPort->setValue(setting.value("portyaka").toInt());
     ui->leIP->setText(setting.value("IPishnyk").toString());
     ui->comboBox->setCurrentIndex(setting.value("crlfN").toInt());
     ui->leNone->setText(setting.value("fnKey").toString());


     int index=setting.value("crlfFN").toInt();
     if(index>0)
         ui->comboBox_2->setCurrentIndex(--index);
     else {
         ui->comboBox_2->setCurrentIndex(ui->comboBox_2->count()-1);
         ui->sbPort->setValue(35546);
         ui->leNone->setText("\\");
     }

     ui->toolButton->setChecked(setting.value("vis").toBool());
     ui->checkBox->setChecked(!setting.value("atcc").toBool());
     complList=setting.value("list").toStringList();
     ipList=setting.value("listIP").toStringList();
     ui->spinBox->setValue(setting.value("wait").toInt());

    setting.endGroup();
}

//-----------------------------------------------------------------------------------------------------------
void Widget::saveSettings()
{
    QSettings setting ("Hello_ZB","Qt_Terminalko_IP");
    setting.beginGroup("Terminalko_IP");
    setting.setValue("position", this->geometry());
    setting.setValue("portyaka",ui->sbPort->value());
    setting.setValue("IPishnyk", ui->leIP->text());
    setting.setValue("crlfN", ui->comboBox->currentIndex());

    setting.setValue("crlfFN", ui->comboBox_2->currentIndex()+1);
    setting.setValue("fnKey", ui->leNone->text());
    setting.setValue("vis", ui->toolButton->isChecked());
    setting.setValue("atcc", !ui->checkBox->isChecked());
    while(complList.count()>200)
        complList.removeLast();
    setting.setValue("list", complList);
    while(ipList.count()>200)
        ipList.removeLast();
    setting.setValue("listIP", ipList);
    setting.setValue("wait",ui->spinBox->value());
    setting.endGroup();
}

//-----------------------------------------------------------------------------------------------------------

void Widget::on_leIP_returnPressed()
{
    if(!ui->leIP->text().isEmpty()){


        if(ui->pbConDisc->isChecked())
            socket->close();
        addToIpCompl();
        socket->connectToHost(ui->leIP->text(), ui->sbPort->value());
        if(!socket->waitForConnected(ui->spinBox->value())){
            ui->pbConDisc->setChecked(0);
            ui->plainTextEdit->appendHtml(tr("<i><b>Not connected!</b></i>"));
        }
    }
}

//-------------------------------------------------------------------------------------------------------------
void Widget::addToIpCompl()
{


    ui->plainTextEdit->appendHtml(tr("<i>Connecting...</i>"));
    for(int i=0; i<ipList.size(); i++){
        if(ui->leIP->text()==ipList.at(i)){
            ipList.removeAt(i);
            break;
        }
    }
    QTimer::singleShot(50,this,SLOT(clearLine2()));

}
//-------------------------------------------------------------------------------------------------------------
void Widget::clearLine2()
{
    delete myIPCompl;
    ipList.insert(0,ui->leIP->text());
    myIPCompl=new QCompleter(ipList,this);
    myIPCompl->setCaseSensitivity(Qt::CaseInsensitive);
    ui->leIP->setCompleter(myIPCompl);
}
