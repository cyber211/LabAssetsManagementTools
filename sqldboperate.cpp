#include "sqldboperate.h"
#include "sqldboperate.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QObject>
#include <QString>
#include <QtSql>
#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>

SQLdbOperate::SQLdbOperate()
{


}
SQLdbOperate::~SQLdbOperate()
{

}

void SQLdbOperate::DBOpen()
{



}

void SQLdbOperate::DBclose()
{
  db.close();//关闭数据库

}

void updateDataTOtableview(QString SQLString)
{
    ui->tableView->setModel(TableModel);//通过setModel来绑定数据源

    QSqlQuery query(db);


    query.exec(SQLString);
    int columnNum = query.record().count(); //获取每条记录中属性（即列）的个数
    //设置列数
    TableModel->setColumnCount(columnNum);
    //设置标题
    int i = 0;
    for(i = 0;i < columnNum; i++)
    {
        TableModel->setHeaderData(i,Qt::Horizontal,query.record().fieldName(i));
    }
    QStandardItem *item[columnNum];
    QList <QStandardItem*>ItemList;

    while((bool)query.next())
    {        //增加项到模型
        for(i = 0;i < columnNum; i++)
        {
          item[i] = new QStandardItem(query.value(i).toString());//取第i条记录第1个字段(从0开始计数)的结果
          ItemList << item[i];
          //获得
        }
        TableModel->appendRow(ItemList);
        ItemList.clear();

    }
    //8.设置模型到View
    Ui_MainWindow::tableView->setModel(TableModel);//通过setModel来绑定数据源
    Ui_MainWindow::tableView->resizeColumnsToContents();
}
