#include "logindlg.h"
#include "ui_logindlg.h"
#include "mainwindow.h"
#include "main.h"
#include <QStandardItem>
#include <qmessagebox.h>

extern bool RemoteDB;

LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
{
    ui->setupUi(this);

    if(RemoteDB)
    {
        this->setWindowTitle("Login   -- Remote Database(Q Disk)");
    }
    else
    {
        this->setWindowTitle("Login   -- Local Database");
    }

    connect(ui->pushButton_Login,SIGNAL(clicked()),this,SLOT(onLogin_Clicked()));
    ui->lineEdit_Username->setText("Guest");
    ui->lineEdit_Pwd->setText("Guest");


}

LoginDlg::~LoginDlg()
{
    delete ui;
}


void LoginDlg::onLogin_Clicked()
{
    QSqlDatabase dbUserTable;
    if(QSqlDatabase::contains("UserTableDB"))
    {
      dbUserTable = QSqlDatabase::database("UserTableDB");
    }
    else
    {
      dbUserTable = QSqlDatabase::addDatabase("QSQLITE","UserTableDB");
    }
    qDebug() << dbUserTable;
    //dbCalVer = QSqlDatabase::addDatabase("QSQLITE");

    if(RemoteDB)
    {   //  Q DISK CAN BE CHANGED TO THE ABSOLUTE PATH: "Q:" = "\\\\global.tektronix.net"
        QString DBString ="Q:\\Asia\\Shanghai\\workgrps\\Engineering\\public\\test report\\LabEquipmentsDatabase\\Equipments.db";
        dbUserTable.setDatabaseName(DBString);
        qDebug() << DBString;
    }
    else
    {
        dbUserTable.setDatabaseName("Equipments.db"); // 数据库名与路径, 此时是放在同目录下
        qDebug("Equipments.db");
    }


    dbUserTable.open();
    //取出到list中 用于对比

    QSqlQuery queryUser(dbUserTable);

/*
    if(queryUser.exec("SELECT COUNT(UserName) FROM UserTable"))
    {
        qDebug("SQL exec : successful!");
    }
    else
    {
        qDebug("SQL exec : Failed!");
    }
    queryUser.next();
    int UserNum = queryUser.value(0).toInt(); //获取记录个数
*/
    QStandardItem *item[3];
    QList <QStandardItem*>ItemList[3];


    if(queryUser.isActive())
    {
        qDebug("queryUser is active!");
    }
    else
    {
        qDebug("queryUser is inactive!");
    }

    if(queryUser.exec("SELECT * FROM UserTable"))
    {
        qDebug("SQL exec : successful!");
    }
    else
    {
        qDebug("SQL exec : Failed!");
    }

    while((bool)queryUser.next())//
    {

        item[0] = new QStandardItem(queryUser.value(0).toString());//取当前记录第i个字段(从0开始计数)的内容
        item[1]  = new QStandardItem(queryUser.value(1).toString());//取当前记录第i个字段(从0开始计数)的内容
        item[2]= new QStandardItem(queryUser.value(2).toString());

        ItemList[0] << item[0];//username
        ItemList[1] << item[1];//pwd
        ItemList[2] << item[2];//level
        //qDebug()<<item[0]->data(Qt::DisplayRole).toString()<<item[1]->data(Qt::DisplayRole).toString()<<item[2]->data(Qt::DisplayRole).toString();

    }

    // 判断用户名和密码是否正确，如果错误则弹出警告对话框
    Level = VerifyvalidUser(ItemList);
    if(Level != 0)
    {
        //emit signal to mainwindow to update ui , 动态增加菜单项， 按钮工具栏按钮变灰，根据传送的权限级别Level
        this->accept();
    }
    else
    {
        // 清空内容并定位光标
        ui->lineEdit_Username->clear();
        ui->lineEdit_Pwd->clear();
        ui->lineEdit_Username->setFocus();
    }
}


// return 0: verify fail,other pass
// return 1: verify pass and the value is the user level: admin
// return 2: verify pass and the value is the user level: READ-WRITE
// return 3: verify pass and the value is the user level: READ-WRITE
int LoginDlg::VerifyvalidUser(QList <QStandardItem*>ItemList[3])
{
    // UserName
    //ItemList[0].at(0) = UserName
        // 判断用户名和密码是否正确，如果错误则弹出警告对话框
    QString UserFilled,PwdFilled,UserName,pwd,strLevel;
    UserFilled = ui->lineEdit_Username->text().trimmed();
    PwdFilled = ui->lineEdit_Pwd->text();

    int i, Level;
    for(i = 0; i<ItemList[0].count();i++)
    {
      UserName = ItemList[0].at(i)->data(Qt::DisplayRole).toString();

      if(UserName == UserFilled)
      {
          pwd = ItemList[1].at(i)->data(Qt::DisplayRole).toString();
          if(pwd == PwdFilled)
          {
              qDebug(" Verify Pass");
              strLevel = ItemList[2].at(i)->data(Qt::DisplayRole).toString();
              Level = strLevel.toInt(0,10);

              return Level;
          }
          else
          {
              QMessageBox::warning(this, tr("Warning"),tr("The Password is not match the username!"),QMessageBox::Yes);
              return 0;
          }
      }
      else
      {
          continue;
      }
    }

    QMessageBox::warning(this, tr("Warning"),tr("No such User Name!"),QMessageBox::Yes);
    return 0;


}
