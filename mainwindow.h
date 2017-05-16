#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QStringListModel>
#include <QStringList>
#include <QTcpSocket>
#include <QtSql>
#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>
#include "addnewitem.h"
#include "addnewequipment.h"

#include "typedef.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int userlevel, QWidget *parent = 0);
    ~MainWindow();

    QSqlDatabase db;
    QStandardItemModel *TableModel;
    QStringListModel *Listmodel;

    CalOrVer CalVerCat;
    int UserLevel;

    QString SQLString_ConsumableMaterial,SQLString_EquipmentAll,SQLString_EquipmentCAL,SQLString_EquipmentVER,SQLString_EquipmentOther;

    void dbOpen();
    void dbClose();
    void updateData2Tableview(QString SQLString);
    void ExportToCsv(QAbstractItemModel* view, QString fileName);

    //void readWelcome(QTcpSocket & socket);
    //void smtpCommunication(QTcpSocket & socket);
    //void communication(QTcpSocket & socket, const char *msg);
    void SendEmail();




signals:
   void Signal_ModifyUI_Item(QList <QString>,DB_Update_Catgroy);
   void Signal_ModifyUI_Equipment(QList <QString>,DB_Update_Catgroy);
   void Signal_UpdateDateUI(CalOrVer,QString,int);
   void Signal_UpdateViewMoreInforDlgUI(QString);


private slots:
    void ListView_itemClicked();
    void TableView_itemClicked();
    void Addnew_itemClicked();
    void Modify_itemClicked();
    void Delete_Clicked();
    void UpdateVerCalDate_ButtonClicked();
    void Search_ButtonClicked();

    void AboutThis();
    //void AboutQT();

    void on_pushButton_Export_clicked();
    void on_pushButton_ViewMoreInfo_clicked();

public slots://
 //   void updateSQLdb(int, QList <QStandardItem*>);//ItemList 用于存放从add对话框传递过来的字段数据;
    void updateSQLdb_AddnewItem(int, QList<QString>);//ItemList 用于存放从add对话框传递过来的字段数据;
    void updateSQLdb_AddnewEquipment(int, QList <QStandardItem*>);//ItemList 用于存放从add对话框传递过来的字段数据;
    void updateSQLdb_ModifyItem(int, QList<QString>);//ItemList 用于存放从add对话框传递过来的字段数据;
    void updateSQLdb_ModifyEquipment(int, QList <QStandardItem*>);//ItemList 用于存放从add对话框传递过来的字段数据;
    //void updateSQLdb_UpdateDate(QList <QStandardItem*>);
    void Slot_UpdateTableViewfromCalVerDateDlg();
    void ExportToXlsx(QAbstractItemModel* view, QString fileName);


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
