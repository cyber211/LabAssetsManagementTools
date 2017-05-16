#include "updatecalverdatedlg.h"
#include "ui_updatecalverdatedlg.h"
#include <QStandardItem>
#include "mainwindow.h"
#include "main.h"
#include <QMessageBox>

extern bool RemoteDB;

UpdateCalVerDateDlg::UpdateCalVerDateDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateCalVerDateDlg)
{
    ui->setupUi(this);

    //QSqlDatabase dbCalVer;
    if(QSqlDatabase::contains("CalVerDateDB"))
    {
      dbCalVer = QSqlDatabase::database("CalVerDateDB");      
    }
    else
    {
      dbCalVer = QSqlDatabase::addDatabase("QSQLITE","CalVerDateDB");
    }
    qDebug() << dbCalVer;
    //dbCalVer = QSqlDatabase::addDatabase("QSQLITE");

    if(RemoteDB)
    {
        QString DBString ="Q:\\Asia\\Shanghai\\workgrps\\Engineering\\public\\test report\\LabEquipmentsDatabase\\Equipments.db";
        dbCalVer.setDatabaseName(DBString);
    }
    else
    {
       dbCalVer.setDatabaseName("Equipments.db"); // 数据库名与路径, 此时是放在同目录下
    }

    connect(ui->pushButton_Update,SIGNAL(clicked()),this,SLOT(Update_ButtonClicked()));
    connect(ui->pushButton_Cancel,SIGNAL(clicked()),this,SLOT(Cancel_ButtonClicked()));
    connect(ui->tableView_History,SIGNAL(clicked(const QModelIndex &)),this,SLOT(TableViewHistory_Clicked()));
}

UpdateCalVerDateDlg::~UpdateCalVerDateDlg()
{
    delete ui;
}

void UpdateCalVerDateDlg::update_UpdateDateUI(CalOrVer CalVerCat,QString IGSN,int UserLevel)
{
    if(UserLevel == 3)
    {
        ui->pushButton_Update->setDisabled(true);
    }
    QString SelectString;
    CalVerCatHistory = CalVerCat;
    qDebug() << CalVerCatHistory;

     if(CalVerCat == Calibration_Equipment)
     {
         SelectString = "SELECT * FROM CalEquipments WHERE IG_SN = '";
     }
     else if(CalVerCat == Verify_Equipment)
     {
         SelectString = "SELECT * FROM VerifyEquipments WHERE IG_SN = '";
     }

     SelectString.append(IGSN);
     SelectString.append("'");

     qDebug() << SelectString;

     //update tableView
     TableModelCalver = new QStandardItemModel;

     ui->tableView_History->setModel(TableModelCalver);

     dbCalVer.open();
     QSqlQuery queryCalVer(dbCalVer);

     TableModelCalver->clear();

     if(queryCalVer.exec(SelectString))
     {
         qDebug("SQL exec : successful!");
     }
     else
     {
         qDebug("SQL exec : Failed!");
     }
     int columnNum = queryCalVer.record().count(); //获取每条记录中属性（即列）的个数

     //设置列数
     TableModelCalver->setColumnCount(columnNum);
     //设置标题
     int i = 0;
     for(i = 0;i < columnNum; i++)
     {
         TableModelCalver->setHeaderData(i,Qt::Horizontal,queryCalVer.record().fieldName(i));
     }
     // set fixed column width
     ui->tableView_History->setColumnWidth(0,120);
     ui->tableView_History->setColumnWidth(1,140);
     ui->tableView_History->setColumnWidth(2,140);
     ui->tableView_History->setColumnWidth(3,200);

     QStandardItem *item[columnNum];

     QList <QStandardItem*>ItemList;

     if(queryCalVer.isActive())
     {
         qDebug("queryCalVer is active!");
     }
     else
     {
         qDebug("queryCalVer is inactive!");
     }

     while((bool)queryCalVer.next())//
     {        //增加项到模型
         for(i = 0;i < columnNum; i++)
         {
           item[i] = new QStandardItem(queryCalVer.value(i).toString());//取当前记录第i个字段(从0开始计数)的内容
           ItemList << item[i];

           //获得
         }

         TableModelCalver->appendRow(ItemList);
         ItemList.clear();
     }

     //8.设置模型到View
     ui->tableView_History->setModel(TableModelCalver);//通过setModel来绑定数据源

     dbCalVer.close();

}

void UpdateCalVerDateDlg::TableViewHistory_Clicked()
{
    int TableCurRow,TableCurCol;
    TableCurRow = ui->tableView_History->currentIndex().row();
    QVariant datatemp[4];
    QModelIndex indextemp;

    //method 3 获得当前行的信息并添加到下面
    QAbstractItemModel *modessl = ui->tableView_History->model();

    for(TableCurCol = 0; TableCurCol<4; TableCurCol++)
    {
       indextemp = modessl->index(TableCurRow,TableCurCol);//遍历第row行的3列
       datatemp[TableCurCol]  = modessl->data(indextemp);//这个是单元格的值
    }
    ui->lineEdit_IGSN->setText(datatemp[0].toString());
    ui->dateEdit_CalDate->setDate(datatemp[1].toDate());
    ui->dateEdit_DueDate->setDate(datatemp[2].toDate());
    ui->lineEdit_CakVerPeriod->setText(datatemp[3].toString());

}

void UpdateCalVerDateDlg::Update_ButtonClicked()
{
    qDebug("Update_ButtonClicked");

    ui->pushButton_Update->setDisabled(true);

    QString InsertSQLString,SQLString_Update;

    QStandardItem *item[4];
    //QList <QStandardItem*>ItemList;

    item[0] = new QStandardItem(ui->lineEdit_IGSN->text());

    item[1] = new QStandardItem();
    item[1]->setData(ui->dateEdit_CalDate->date());

    item[2] = new QStandardItem();
    item[2]->setData(ui->dateEdit_DueDate->date());

    item[3] = new QStandardItem(ui->lineEdit_CakVerPeriod->text());


    if(item[0]->data(Qt::DisplayRole).toString().isEmpty())
    {
        QMessageBox Msg_warning;
        Msg_warning.setWindowTitle("Warning!");
        Msg_warning.setText("Please Fill correctly!");

        Msg_warning.setStandardButtons(QMessageBox::Ok);

        Msg_warning.setIcon(QMessageBox::Warning);
        Msg_warning.exec();

    }
    else
    {
       // for(int i = 0;i<4;i++)
       // {
       //    ItemList << item[i];
       // }
        //直接向数据库里面存储//没有刷新主窗口的tableview，考虑用信号-槽的机制
        if(CalVerCatHistory == Calibration_Equipment)
        {
            InsertSQLString = "INSERT INTO CalEquipments VALUES('";
        }
        else if(CalVerCatHistory == Verify_Equipment)
        {
            InsertSQLString = "INSERT INTO VerifyEquipments VALUES('";
        }
        InsertSQLString.append(item[0]->data(Qt::DisplayRole).toString());

        InsertSQLString.append("',DATE('");
        InsertSQLString.append(item[1]->data().toString());
        InsertSQLString.append("'),DATE('");
        InsertSQLString.append(item[2]->data().toString());
        InsertSQLString.append("'),'");
        InsertSQLString.append(item[3]->data(Qt::DisplayRole).toString());
        InsertSQLString.append("')");

        qDebug() << InsertSQLString;

        dbCalVer.open();
        QSqlQuery queryCalVer2(dbCalVer);
        if(queryCalVer2.exec(InsertSQLString))
        {
          qDebug("SQL:Update cal/ver date successfully!");
        }
        else
        {
          qDebug("SQL:Update cal/ver date failed!");
          QSqlError SQLerror;
          SQLerror = dbCalVer.lastError();
          qDebug() << SQLerror;
        }

        update_UpdateDateUI(CalVerCatHistory,ui->lineEdit_IGSN->text(),3);

        // Connect()
       // emit  Signal_UpdateDate(ItemList);
        //this->close();

        qDebug("Update SUMMARY DATE date");
     // UPDATE SummaryTable SET CalDate = DATE(''),DueDate = DATE('') WHERE IG_SN = ''

        SQLString_Update = "UPDATE SummaryTable SET CalDate = DATE('";
        SQLString_Update.append(item[1]->data().toString());

        SQLString_Update.append("'),DueDate = DATE('");
        SQLString_Update.append(item[2]->data().toString());

        SQLString_Update.append("') WHERE IG_SN = '");
        SQLString_Update.append(item[0]->data(Qt::DisplayRole).toString());
        SQLString_Update.append("'");

        dbCalVer.open();
        QSqlQuery queryCalVer3(dbCalVer);
        qDebug() << SQLString_Update;
        if(queryCalVer3.exec(SQLString_Update))
        {
            qDebug("Update summary cal/ver equipments date successful!");
        }
        else
        {
            qDebug("Update summary cal/ver equipments date Failed!");
        }

      // Mainwindow TABVIEW REFRESH
        emit Signal_UpdateTableViewfromCalVerDateDlg();

    }
}

void UpdateCalVerDateDlg::Cancel_ButtonClicked()
{
    qDebug("Cancel_ButtonClicked");
    this->close();
}

