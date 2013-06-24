#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork>
#include <QCompleter>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
public slots:

    
private slots:
    void vyvidReadData();
    void discReaktsiya();
    void on_pbConDisc_clicked(bool checked);
    void writeToServer();
    void zednavs();
    void clearLine();
    void loadSettings();
    void saveSettings();
    void addToIpCompl();
    void clearLine2();

    void on_leSend_textChanged(const QString &arg1);

    void on_toolButton_toggled(bool checked);

    void on_toolButton_2_clicked();

    void on_leIP_returnPressed();

private:
    Ui::Widget *ui;
    QTcpSocket *socket;
    QStringList complList;
    QCompleter *myCompl;

    QStringList ipList;
    QCompleter *myIPCompl;
    bool  selPosition;
};

#endif // WIDGET_H
