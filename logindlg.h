#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include <QStandardItem>


namespace Ui {
class LoginDlg;
}

class LoginDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDlg(QWidget *parent = 0);
    ~LoginDlg();

    int Level;
    int VerifyvalidUser(QList <QStandardItem*>ItemList[3]);

public slots:
    void onLogin_Clicked();

signals:



private:
    Ui::LoginDlg *ui;
};

#endif // LOGINDLG_H
