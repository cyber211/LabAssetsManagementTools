#include "moreinfordlg.h"
#include "ui_moreinfordlg.h"
#include "mainwindow.h"
#include "main.h"
#include <QFileDialog>
//#include <QAxWidget>

extern bool RemoteDB;

MoreInforDlg::MoreInforDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoreInforDlg)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;


    setWindowFlags(flags);

    if(QSqlDatabase::contains("MoreInfoDB"))
    {
      MoreInfoDB = QSqlDatabase::database("MoreInfoDB");
    }
    else
    {
      MoreInfoDB = QSqlDatabase::addDatabase("QSQLITE","MoreInfoDB");
    }
    qDebug() << MoreInfoDB;

    if(RemoteDB)
    {
        QString DBString ="Q:\\Asia\\Shanghai\\workgrps\\Engineering\\public\\test report\\LabEquipmentsDatabase\\Equipments.db";
        MoreInfoDB.setDatabaseName(DBString);
    }
    else
    {
        MoreInfoDB.setDatabaseName("Equipments.db"); // 数据库名与路�?, 此时是放在同目录�?
    }
}

MoreInforDlg::~MoreInforDlg()
{
    delete ui;
}

void MoreInforDlg::updateUI_MoreInfrDlg(QString SQLReceive)
{

    TableModelMoreInfo = new QStandardItemModel;

    ui->tableView_MoreInfo->setModel(TableModelMoreInfo);

    MoreInfoDB.open();
    QSqlQuery queryMoreInfo(MoreInfoDB);

    TableModelMoreInfo->clear();

    if(queryMoreInfo.exec(SQLReceive))
    {
        qDebug("SQL exec : successful!");
    }
    else
    {
        qDebug("SQL exec : Failed!");
    }
    int columnNum = queryMoreInfo.record().count();

    TableModelMoreInfo->setColumnCount(columnNum);
    int i = 0;
    for(i = 0;i < columnNum; i++)
    {
        TableModelMoreInfo->setHeaderData(i,Qt::Horizontal,queryMoreInfo.record().fieldName(i));
    }

    QStandardItem *item[columnNum];

    QList <QStandardItem*>ItemList;

    if(queryMoreInfo.isActive())
    {
        qDebug("queryCalVer is active!");
    }
    else
    {
        qDebug("queryCalVer is inactive!");
    }

    while((bool)queryMoreInfo.next())//
    {
        for(i = 0;i < columnNum; i++)
        {
          item[i] = new QStandardItem(queryMoreInfo.value(i).toString());
          ItemList << item[i];
        }

        TableModelMoreInfo->appendRow(ItemList);
        ItemList.clear();
    }
    ui->tableView_MoreInfo->setModel(TableModelMoreInfo);
    ui->tableView_MoreInfo->resizeColumnsToContents();
}

void MoreInforDlg::on_pushButton_Cancel_clicked()
{
    this->close();
}

void MoreInforDlg::on_pushButton_Export_clicked()
{
    QAbstractItemModel* View;
    QString Filename;

    View = ui->tableView_MoreInfo->model();

    QFileDialog saveDlg;
    Filename = saveDlg.getSaveFileName(this, tr("Save File"),"./export/*.xlsx",
                                                  tr("Excel File (*.xlsx *.xls *.csv)"));

    qDebug() << Filename;
    if (Filename.isEmpty())
    {
        qDebug("Please fill the file name.");
        return;
    }
    else
    {
        emit Signal_ExportfromMoreInfor(View,Filename);
    }
 //   QAxWidget excel("Excel.Application");
 //   excel.setProperty("Visible", true);
 //   QAxObject *workbooks = excel.querySubObject("WorkBooks");
  //  workbooks->dynamicCall("Open (const QString&)", Filename);



}
