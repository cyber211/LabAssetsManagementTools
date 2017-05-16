#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QMessageBox>
#include "addnewitem.h"
#include "addnewequipment.h"
#include "updatecalverdatedlg.h"
#include <qaction.h>
#include "logindlg.h"
#include <QModelIndex>
#include <QFileDialog>
#include "moreinfordlg.h"
#include "main.h"
#include <QStandardItemModel>
#include <QTcpSocket>
#include <QThread>
#include <iostream>

#include "xlsx/xlsxdocument.h"
#include "xlsxformat.h"
#include "xlsxabstractsheet.h"
#include "xlsxworkbook.h"
#include "xlsxworksheet.h"

//#include "./smtp/SmtpMime.h"

#include "./smtp/smtpclient.h"
#include "./smtp/mimepart.h"
#include "./smtp/mimehtml.h"
#include "./smtp/mimeattachment.h"
#include "./smtp/mimemessage.h"
#include "./smtp/mimetext.h"
#include "./smtp/mimeinlinefile.h"
#include "./smtp/mimefile.h"


extern bool RemoteDB;


MainWindow::MainWindow(int userlevel,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setCentralWidget(ui->centralWidget);

    setIconSize(QSize(64, 64));
    //setWindowIcon(QIcon(".\\images\\Title.png"));
    setWindowIcon(QIcon(":/images/Title.bmp"));

    QMenu *menu = QMainWindow::createPopupMenu();
    QList<QAction*> list = menu->actions();
    list.at(0)->setText("Display/Hide ToolBar");



    UserLevel = userlevel;
    qDebug() << UserLevel;

    QString strMainWindowTitle = "Fluke IG Lab Equipments Management ---";

    if(UserLevel == 1)// admin: add menu user
    {
        strMainWindowTitle = strMainWindowTitle + " ---Admin user";
        //setWindowTitle("Fluke IG Lab Equipments Management ---Admin user");
    }
    else if(UserLevel == 3)//  R USER, include Guest
    {
        strMainWindowTitle = strMainWindowTitle + " ---Read only user";
        //setWindowTitle("Fluke IG Lab Equipments Management ---Read only user");
        //ui->menuUser_Management->setDisabled(true);
        ui->actionAdd_New_User->setDisabled(true);
        ui->actionDelete_User->setDisabled(true);
        ui->actionModify_User_Level->setDisabled(true);
        ui->actionView_all_user_info->setDisabled(true);

        ui->AddnewEquipment->setDisabled(true);
        ui->actionAddNew->setDisabled(true);
        ui->actionAdd_new_Equipment->setDisabled(true);

        ui->DeleteButton->setDisabled(true);
        ui->actionDelete_Equipment->setDisabled(true);
        ui->actionDeleteRecord->setDisabled(true);


        ui->ModifyButton->setDisabled(true);
        ui->actionModify_Ewuipment_information->setDisabled(true);
        ui->actionModifyRecord->setDisabled(true);

        ui->actionEquipment_lent_Register->setDisabled(true);
        ui->actionEquipment_Return_Register->setDisabled(true);

    }
    else if(UserLevel == 2)// R+W  USER
    {
        strMainWindowTitle = strMainWindowTitle + " ---Read and Write user";
        //setWindowTitle("Fluke IG Lab Equipments Management ---Read and Write user");
        //ui->menuUser_Management->setDisabled(true);
        ui->actionAdd_New_User->setDisabled(true);
        ui->actionDelete_User->setDisabled(true);
        ui->actionModify_User_Level->setDisabled(true);
    }

    if(RemoteDB)
    {
        strMainWindowTitle = strMainWindowTitle + "  ---Remote DB";
    }
    else
    {
        strMainWindowTitle = strMainWindowTitle + "  ---Local DB";
    }


    setWindowTitle(strMainWindowTitle);

    TableModel = new QStandardItemModel;
    db = QSqlDatabase::addDatabase("QSQLITE");

    if(RemoteDB)
    {
        QString DBString ="Q:\\Asia\\Shanghai\\workgrps\\Engineering\\public\\test report\\LabEquipmentsDatabase\\Equipments.db";
        db.setDatabaseName(DBString);
    }
    else
    {
        db.setDatabaseName("Equipments.db"); // 数据库名与路�?, 此时是放在同目录�?
    }

    SQLString_ConsumableMaterial = "select * from ConsumablesMaterial";
    SQLString_EquipmentAll = "select EquipmentName,ModelName,IG_SN,Mfg_SN,Caldate,DueDate,Status,Spec_Description,InternalCatergory,CalOrVerify from SummaryTable";
    SQLString_EquipmentCAL = "select EquipmentName,ModelName,IG_SN,Mfg_SN,Caldate,DueDate,Status,Spec_Description,InternalCatergory,CalOrVerify from SummaryTable where CalOrVerify = 'CAL'";
    SQLString_EquipmentVER = "select EquipmentName,ModelName,IG_SN,Mfg_SN,Caldate,DueDate,Status,Spec_Description,InternalCatergory,CalOrVerify from SummaryTable where CalOrVerify = 'VER'";
    SQLString_EquipmentOther = "select EquipmentName,ModelName,IG_SN,Mfg_SN,Caldate,DueDate,Status,Spec_Description,InternalCatergory,CalOrVerify from SummaryTable where CalOrVerify NOT IN('CAL','VER')";

    ui->listView->setSpacing(2);
    QStringList Equipments;
    Equipments += "All Equipments";
    Equipments += "Equipments(Calibration)";
    Equipments += "Equipments(Verification)";
    Listmodel = new QStringListModel(Equipments);
    ui->listView->setModel(Listmodel);        //useList是个QListView
    Equipments += "Consumable Materials";
    Equipments += "Other equipments";
    Listmodel->setStringList(Equipments);

    QModelIndex listviewindex;

    listviewindex = Listmodel->index(0,0,QModelIndex());

    //QModelIndex::QModelIndex();

    ui->listView->setCurrentIndex(listviewindex);

//---------------------------------------------------------------------------------
// 1. 一开始显示all equipment的table�?

    dbOpen();
    QString SQLString = SQLString_EquipmentAll;
    updateData2Tableview(SQLString);
    dbClose();
    ui->statusBar->showMessage("All equipments were showed on the right table grid.");

    ui->mainToolBar->setIconSize(QSize(48,48));
//--------------------------------------------------------------------------------------------------------
    // Button
    connect(ui->listView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(ListView_itemClicked()));
    connect(ui->AddnewEquipment,SIGNAL(clicked()),this,SLOT(Addnew_itemClicked()));
    connect(ui->ModifyButton,SIGNAL(clicked()),this,SLOT(Modify_itemClicked()));
    connect(ui->DeleteButton,SIGNAL(clicked()),this,SLOT(Delete_Clicked()));
    connect(ui->listView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(ListView_itemClicked()));
    connect(ui->tableView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(TableView_itemClicked()));
    connect(ui->tableView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(Modify_itemClicked()));
    connect(ui->UpdateCalVerDate,SIGNAL(clicked()),this,SLOT(UpdateVerCalDate_ButtonClicked()));
    connect(ui->pushButton_Search,SIGNAL(clicked()),this,SLOT(Search_ButtonClicked()));

    // 链接QAction--ToolBar
    connect(ui->actionAddNew,SIGNAL(triggered()),this,SLOT(Addnew_itemClicked()));
    connect(ui->actionModifyRecord,SIGNAL(triggered()),this,SLOT(Modify_itemClicked()));
    connect(ui->actionDeleteRecord,SIGNAL(triggered()),this,SLOT(Delete_Clicked()));
    connect(ui->actionUpdateDate,SIGNAL(triggered()),this,SLOT(UpdateVerCalDate_ButtonClicked()));
    connect(ui->actionSearch,SIGNAL(triggered()),this,SLOT(Search_ButtonClicked()));
    connect(ui->actionExporttoExcel,SIGNAL(triggered()),this,SLOT(on_pushButton_Export_clicked()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(AboutThis()));
    connect(ui->actionView_More_information_of_Equipment,SIGNAL(triggered()),this,SLOT(on_pushButton_ViewMoreInfo_clicked()));

    // 链接QAction--menu
    connect(ui->actionAdd_new_Equipment,SIGNAL(triggered()),this,SLOT(Addnew_itemClicked()));
    connect(ui->actionModify_Ewuipment_information,SIGNAL(triggered()),this,SLOT(Modify_itemClicked()));
    connect(ui->actionDelete_Equipment,SIGNAL(triggered()),this,SLOT(Delete_Clicked()));
    connect(ui->actionView_Calibration_verificatin_date,SIGNAL(triggered()),this,SLOT(UpdateVerCalDate_ButtonClicked()));
    connect(ui->actionAbout_this_Program,SIGNAL(triggered()),this,SLOT(AboutThis()));

    connect(ui->actionSearch_2,SIGNAL(triggered()),this,SLOT(Search_ButtonClicked()));
    connect(ui->actionExport_to_Excel,SIGNAL(triggered()),this,SLOT(on_pushButton_Export_clicked()));

}

MainWindow::~MainWindow()
{
    dbClose();
    delete ui;
}



void MainWindow::dbOpen()
{
    if(!db.open())
    {
        qDebug("数据库不能打开");
    }
}


void MainWindow::dbClose()
{
    if(db.isOpen())
    {
        db.close();
    }
}

void MainWindow::updateData2Tableview(QString SQLString)
{
    ui->tableView->setModel(TableModel);//通过setModel来绑定数据源

    QSqlQuery query(db);
    TableModel->clear();
    if(query.exec(SQLString))
    {
        qDebug() << SQLString;
        qDebug("SQL exec: successful!");
    }
    else
    {
        qDebug("SQL exec: Failed!");
    }

    int columnNum = query.record().count(); //获取每条记录中属性（即列）的个数
    //设置列数
    TableModel->setColumnCount(columnNum);
    //设置标题
    int i = 0,j = 0;
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
          item[i] = new QStandardItem(query.value(i).toString());//取当前记录第i个字�?(�?0开始计�?)的内�?
          ItemList << item[i];
        }
        TableModel->appendRow(ItemList);

        ItemList.clear();
    }

    //8.设置模型到View
    ui->tableView->setModel(TableModel);//通过setModel来绑定数据源

    ui->tableView->resizeColumnsToContents();

    ui->tableView->setColumnWidth(8,110);
    ui->tableView->setColumnWidth(9,110);

    // judge the Duedate
    int RowNum = TableModel->rowCount(QModelIndex());
    qDebug() << RowNum;

   // TableModel->item(0,0)->setBackground(QBrush(QColor(255, 0, 0)));


    int tableIndex = ui->listView->currentIndex().row();
    if((tableIndex ==0)||(tableIndex ==1)||(tableIndex ==2))
    {

         QString CellValue;
         QDate Duedate,CurDate;
         CurDate = QDate::currentDate();
         QModelIndex indextemp;
        for(i = 0;i < RowNum ; i++ )
        {
            CellValue = TableModel->item(i,9)->data(Qt::DisplayRole).toString();
            //qDebug() << CellValue;
            if((CellValue == "CAL")||(CellValue == "VER"))
            {
                //Duedate = TableModel->item(i,5)->data().toDate();
                indextemp = TableModel->index(i,5);
                Duedate = TableModel->data(indextemp).toDate();
                qDebug() << Duedate << CurDate;
                if((Duedate.addDays(30) < CurDate))
                {
                    qDebug("send email true");





/*
                    QTcpSocket socket;

                    socket.connectToHost("outlookanywhere.danahertm.com", 443);

                   if (socket.waitForConnected(200))
                   {
                           qDebug() << "smtp server connected success.";
                           readWelcome(socket);
                           smtpCommunication(socket);
                           socket.close();
                   }
                   else
                   {
                           qDebug() << "connection failed.";
                   }
*/                }

                if(Duedate < CurDate)
                {
                    for(j = 0;j<columnNum ;j++)
                    {
                        TableModel->item(i,j)->setBackground(QBrush(QColor(255, 0, 0)));//set row color of tableview
                        //update status ziduan to tingyong







                    }
                }
            }
        }
    }
}

void MainWindow::TableView_itemClicked()
{
  // int row = TableIndex.row();//当前选中�?
   int ListCurROW,TableCurRow;
   ListCurROW = ui->listView->currentIndex().row();
   TableCurRow = ui->tableView->currentIndex().row();
   int col = 3;
   switch(ListCurROW)
   {
       case 0://
       case 4:
       case 1://
       case 2: col = 2; //IGSN COL;
       break;
       case 3: col = 1; //Asset SN COL;
       break;
   }
   QString CellText;

   //method 3 获得当前行的IGSN
   QAbstractItemModel *modessl = ui->tableView->model();
   QModelIndex indextemp = modessl->index(TableCurRow,col);//遍历第row行的3�?
   QVariant datatemp = modessl->data(indextemp);//这个是单元格的�?
   CellText = datatemp.toString();
   ui->lineEdit->setText(CellText);

   if(ListCurROW == 0)//Allequipment table,找到CAL，VER字段判断是cal还是ver
   {
       col = 9;
       indextemp = modessl->index(TableCurRow,col);
       datatemp = modessl->data(indextemp);//这个是单元格的�?
       CellText = datatemp.toString();
       if(CellText == "CAL")
       {
           CalVerCat = Calibration_Equipment;
       }
       else if(CellText == "VER")
       {
           CalVerCat = Verify_Equipment;
       }
       else
       {

       }
   }
}

void MainWindow::ListView_itemClicked()
{
    QModelIndex  ListIndex = ui->listView->currentIndex();

    qDebug() << ListIndex;

    QString SQLString;
    int n = ListIndex.row();
    switch(n)
    {
    case 0://All equipments
        SQLString = SQLString_EquipmentAll;
        ui->statusBar->showMessage("All equipments will be showed on the right table grid.");
        break;
    case 1://Calibration equipments
        SQLString = SQLString_EquipmentCAL;
        ui->statusBar->showMessage("Calibration equipments will be showed on the right table grid.");
        break;
    case 2://Verificatioin equipments
        SQLString = SQLString_EquipmentVER;
        ui->statusBar->showMessage("Verificatioin equipments will be showed on the right table grid.");
        break;
    case 3://consumable material
        SQLString = SQLString_ConsumableMaterial;
        ui->statusBar->showMessage("consumable material will be showed on the right table grid.");
        break;
    case 4://Other equipments
        SQLString = SQLString_EquipmentOther;
        ui->statusBar->showMessage("Other equipments will be showed on the right table grid.");
        break;
    }
    dbOpen();
    updateData2Tableview(SQLString);
    dbClose();

}

void MainWindow::Addnew_itemClicked()
{
    if((ui->listView->currentIndex().row())== -1)
    {
        QMessageBox Msg_warning;
        Msg_warning.setWindowTitle("Warning!");
        Msg_warning.setText("Please select a Database first!");

        Msg_warning.setStandardButtons(QMessageBox::Ok);

        Msg_warning.setIcon(QMessageBox::Warning);
        Msg_warning.exec();
    }
    else if((ui->listView->currentIndex().row())== 3)//add new material item,should operate ConsumableMaterial table
    {
        addNewItem* Addnewitemdialog;
        Addnewitemdialog = new addNewItem(this);
        QObject::connect(Addnewitemdialog,SIGNAL(Signal_addItem(int ,QList <QString>)),this,SLOT(updateSQLdb_AddnewItem(int,QList <QString>)));
        Addnewitemdialog->exec();
    }
    else//add new equipment,should operate summaryTable
    {
        addNewEquipment* AddnewEquipment_dialog;
        AddnewEquipment_dialog = new addNewEquipment(this);
        QObject::connect(AddnewEquipment_dialog,SIGNAL(Signal_addEquipment(int,QList<QStandardItem*>)),this,SLOT(updateSQLdb_AddnewEquipment(int,QList <QStandardItem*>)));
        AddnewEquipment_dialog->exec();
    }


}
void MainWindow::Modify_itemClicked()
{
    if((ui->listView->currentIndex().row())== -1)
    {
        QMessageBox Msg_warning;
        Msg_warning.setWindowTitle("Warning!");
        Msg_warning.setText("Please select a Database first!");

        Msg_warning.setStandardButtons(QMessageBox::Ok);

        Msg_warning.setIcon(QMessageBox::Warning);
        Msg_warning.exec();
    }
    else
    {

        int CurRow = ui->tableView->currentIndex().row();//当前选中�?

        if (CurRow == -1)
        {
            QMessageBox Msg_warning;
            Msg_warning.setWindowTitle("Warning!");
            Msg_warning.setText("Please select a item first!");

            Msg_warning.setStandardButtons(QMessageBox::Ok);

            Msg_warning.setIcon(QMessageBox::Warning);
            Msg_warning.exec();

        }
        else
        {
            QList <QString> ItemList;
            QString ItemString[16];

            QAbstractItemModel *modessl;

            QModelIndex indextemp;
            QVariant datatemp;
            int i;

            if((ui->listView->currentIndex().row())== 3)//add new material item,should operate ConsumableMaterial table
            {
                addNewItem* ModifyItemsdialog;
                ModifyItemsdialog = new addNewItem(this);
                ModifyItemsdialog->setWindowTitle("Modify the selected Material information");
                //遍历tableview，并将数据写到ItemList�?,通过信号发送给dialog

                modessl = ui->tableView->model();
                for( i = 0;i < 6; i++)
                {
                    indextemp = modessl->index(CurRow,i);//遍历第row行的0�?
                    datatemp = modessl->data(indextemp);//这个是单元格的�?
                    ItemString[i] = datatemp.toString();
                    ItemList << ItemString[i];
                }
                QObject::connect(this,SIGNAL(Signal_ModifyUI_Item(QList <QString>,DB_Update_Catgroy)),ModifyItemsdialog,SLOT(updateUI_items(QList <QString>,DB_Update_Catgroy)));
                emit Signal_ModifyUI_Item(ItemList,Modify);

                QObject::connect(ModifyItemsdialog,SIGNAL(Signal_ModifyItem(int ,QList <QString>)),this,SLOT(updateSQLdb_ModifyItem(int,QList <QString>)));

                ModifyItemsdialog->exec();
            }
            else
            {
                addNewEquipment* ModifyEquipmentdialog;
                ModifyEquipmentdialog = new addNewEquipment(this);
                ModifyEquipmentdialog->setWindowTitle("Modify the selected equipment information");

                QString SQLString;
                if(ui->listView->currentIndex().row() == 0)
                {
                    SQLString = "SELECT * FROM SummaryTable";
                }
                else if(ui->listView->currentIndex().row() == 1)
                {

                    SQLString = "SELECT * FROM SummaryTable where CalOrVerify = 'CAL'";
                }
                else if(ui->listView->currentIndex().row() == 2)
                {
                    SQLString = "SELECT * FROM SummaryTable where CalOrVerify = 'VER'";
                }
                else
                {
                    SQLString = "SELECT * FROM SummaryTable where CalOrVerify NOT IN('CAL','VER')";
                }

                QStandardItemModel *TableModel2;
                TableModel2 = new QStandardItemModel;
                dbOpen();
                QSqlQuery query(db);
                TableModel2->clear();
                if(query.exec(SQLString))
                {
                    qDebug() << SQLString;
                    qDebug("SQL exec: successful!");
                }
                else
                {
                    qDebug("SQL exec: Failed!");
                }

                int columnNum = query.record().count(); //获取每条记录中属性（即列）的个数
                //设置列数
                TableModel2->setColumnCount(columnNum);
                //设置标题
                int i = 0;
                for(i = 0;i < columnNum; i++)
                {
                    TableModel2->setHeaderData(i,Qt::Horizontal,query.record().fieldName(i));
                }
                QStandardItem *item[columnNum];

                QList <QStandardItem*>ItemList2;

                while((bool)query.next())
                {        //增加项到模型
                    for(i = 0;i < columnNum; i++)
                    {
                      item[i] = new QStandardItem(query.value(i).toString());//取当前记录第i个字�?(�?0开始计�?)的内�?
                      ItemList2 << item[i];
                    }
                    TableModel2->appendRow(ItemList2);
                    ItemList2.clear();
                }


                modessl = TableModel2;
                for( i = 0;i < 15; i++)
                {
                    indextemp = modessl->index(CurRow,i);//遍历第row行的i�?
                    datatemp = modessl->data(indextemp);//这个是单元格的�?
                    ItemString[i] = datatemp.toString();
                    ItemList << ItemString[i];
                }
                dbClose();


                QObject::connect(this,SIGNAL(Signal_ModifyUI_Equipment(QList <QString>,DB_Update_Catgroy)),ModifyEquipmentdialog,SLOT(updateUI_Equipment(QList <QString>,DB_Update_Catgroy)));
                emit Signal_ModifyUI_Equipment(ItemList,Modify);

                QObject::connect(ModifyEquipmentdialog,SIGNAL(Signal_ModifyEquipment(int,QList <QStandardItem*>)),this,SLOT(updateSQLdb_ModifyEquipment(int, QList <QStandardItem*>)));

                ModifyEquipmentdialog->exec();
            }
        }
    }
}

void MainWindow::UpdateVerCalDate_ButtonClicked()
{
    int ListCurROW,TableCurRow;
    ListCurROW = ui->listView->currentIndex().row();
    TableCurRow = ui->tableView->currentIndex().row();
    QString IGSN = ui->lineEdit->text();
    //if((((ListCurROW == 0) || (ListCurROW == 1) || (ListCurROW == 2)) && (TableCurRow !=-1)))
    if((ListCurROW == 0) || (ListCurROW == 1) || (ListCurROW == 2))
    {
        if(TableCurRow !=-1)
        {
            if(ListCurROW == 1)// cal
            {
                CalVerCat = Calibration_Equipment;
            }
            else if(ListCurROW == 2)//ver
            {
                CalVerCat = Verify_Equipment;
            }
            else if(ListCurROW == 0)//All table, 判断calorver
            {









            }
            qDebug() << CalVerCat;

            UpdateCalVerDateDlg* UpdateCalVerDateDialog;
            UpdateCalVerDateDialog = new UpdateCalVerDateDlg(this);
            QObject::connect(this,SIGNAL(Signal_UpdateDateUI(CalOrVer,QString,int)),UpdateCalVerDateDialog,SLOT(update_UpdateDateUI(CalOrVer,QString,int)));//穿送数据到UI
            QObject::connect(UpdateCalVerDateDialog,SIGNAL(Signal_UpdateTableViewfromCalVerDateDlg()),this,SLOT(Slot_UpdateTableViewfromCalVerDateDlg()));//传回来存入数据库，不用，直接在UI界面存储，因为在UI界面会开一个数据库�?

            emit Signal_UpdateDateUI(CalVerCat,IGSN,UserLevel);
            UpdateCalVerDateDialog->exec();
        }
        else
        {
            QMessageBox Msg_warning;
            Msg_warning.setWindowTitle("Warning!");
            Msg_warning.setText("Please select a Calibration or Verification Record first!");
            Msg_warning.setStandardButtons(QMessageBox::Ok);
            Msg_warning.setIcon(QMessageBox::Warning);
            Msg_warning.exec();
        }
    }
    else
    {
        QMessageBox Msg_warning;
        Msg_warning.setWindowTitle("Warning!");
        Msg_warning.setText("Please select a Calibration or Verification Database first!");

        Msg_warning.setStandardButtons(QMessageBox::Ok);

        Msg_warning.setIcon(QMessageBox::Warning);
        Msg_warning.exec();
    }
}

void MainWindow::Delete_Clicked()//用于删除某一条记�?
{
//确定删的是那个table，再确定是那一条要�?
    int CurRow_Listview = ui->listView->currentIndex().row();

    QString String_Key;
    QString SQLString,SQLString2;

    String_Key = ui->lineEdit->text();
    qDebug() << String_Key;

    if(String_Key.isEmpty())
    {
        QMessageBox Msg_warning;
        Msg_warning.setWindowTitle("Warning!");
        Msg_warning.setText("Please select a item first!");

        Msg_warning.setStandardButtons(QMessageBox::Ok);
        Msg_warning.setIcon(QMessageBox::Warning);
        Msg_warning.exec();
    }
    else
    {
        QMessageBox Msg_deletewarning;
        Msg_deletewarning.setWindowTitle("Warning!");
        Msg_deletewarning.setText("A equipment will be deleted from the database!");
        Msg_deletewarning.setInformativeText("Do you really want to delete this item?");
        Msg_deletewarning.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        Msg_deletewarning.setDefaultButton(QMessageBox::Cancel);
        Msg_deletewarning.setIcon(QMessageBox::Warning);
        int ret = Msg_deletewarning.exec();
        if(ret ==QMessageBox::Yes )
        {
            qDebug("Yes");

     /*
            QModelIndex  TableIndex = ui->tableView->currentIndex();
            int CurRow = TableIndex.row();//当前选中�?
            int CurCol;

            TableModel->removeRow(CurRow);//从tableview中删除了但是数据库还�?
            qDebug("Delete from tableview");


            QAbstractItemModel *modessl = ui->tableView->model();
            switch(CurRow_Listview)
            {
                case 0://
                case 4:
                case 1://
                case 2: CurCol = 3; //IGSN COL;
                break;
                case 3: CurCol = 1; //Asset SN COL;
                break;
            }

            QModelIndex indextemp = modessl->index(CurRow,CurCol);//遍历第row行的3�?
            QVariant datatemp = modessl->data(indextemp);//这个是单元格的�?
            String_Key = datatemp.toString();
    */

            switch(CurRow_Listview)
            {
                case 0://all Equipments----Summary table
                SQLString = "DELETE FROM SummaryTable WHERE IG_SN = ";
                break;
                case 1://Calibration Equipments----cal date table & Summary table
                SQLString = "DELETE FROM SummaryTable WHERE IG_SN = ";
                SQLString2 = "DELETE FROM CalEquipments WHERE IG_SN = ";
                break;
                case 2://Verification Equipments----Summary table & verification table
                SQLString = "DELETE FROM SummaryTable WHERE IG_SN = ";
                SQLString2 = "DELETE FROM VerifyEquipments WHERE IG_SN = ";
                break;
                case 3://Consumable material---- Consumable table
                SQLString = "DELETE FROM ConsumablesMaterial WHERE Asset_Number = ";
                break;
                case 4://Other equipments---- Summary table(except CAL AND VER)
                SQLString = "DELETE FROM SummaryTable WHERE IG_SN = ";
                break;
            }
            SQLString.append("'");
            SQLString.append(String_Key);
            SQLString.append("'");

        // 执行数据库操�?
            dbOpen();
            QSqlQuery query(db);

            if(query.exec(SQLString))
            {
                qDebug("SQL exec for delete item successful!");
            }
            else
            {
                qDebug("SQL exec for delete item failed!");
            }


            if(!SQLString2.isEmpty())
            {
                SQLString2.append("'");
                SQLString2.append(String_Key);
                SQLString2.append("'");
                qDebug() << SQLString2;
                if(query.exec(SQLString2))
                {
                    qDebug("SQL exec for delete item from database successful!");
                }
                else
                {
                    qDebug("SQL exec for delete item from database failed!");
                }


            }
            SQLString.clear();
            SQLString2.clear();

            qDebug("Delete from Sqlite OK!");
            QString temp;

            switch(CurRow_Listview)
            {
               case 0: temp = SQLString_EquipmentAll;break;
               case 1: temp = SQLString_EquipmentAll;break;
               case 2: temp = SQLString_EquipmentVER;break;
               case 3: temp = SQLString_ConsumableMaterial;break;
               case 4: temp = SQLString_EquipmentOther;break;
            }
            updateData2Tableview(temp);
            dbClose();
        }
        else
        {
            qDebug("No,CANCEL");
        }
    }
}

void MainWindow::updateSQLdb_AddnewItem(int database,QList <QString>ItemList)
{
    qDebug("add new item");
    int a;
    a = database;
    QString SQLString_Insert;

    SQLString_Insert = "INSERT INTO ConsumablesMaterial VALUES('";
    for(a = 0;a < 5 ;a++)
    {
        SQLString_Insert.append(ItemList.at(a));
        SQLString_Insert.append("','");
    }
    SQLString_Insert.append(ItemList.at(5));
    SQLString_Insert.append("')");

    // 执行数据库操�?
    dbOpen();
    QSqlQuery query(db);

    if(query.exec(SQLString_Insert))
    {
        qDebug("SQL exec for add NEW item TO database successful!");
    }
    else
    {
        qDebug("SQL exec for ADD NEW item TO database FAILED!");
    }
    updateData2Tableview(SQLString_ConsumableMaterial);
    dbClose();

}

void MainWindow::updateSQLdb_AddnewEquipment(int database, QList<QStandardItem*> ItemList)
{
    qDebug("add new equipment");
    int a;
    a = database;
    QString SQLString_Insert;


    SQLString_Insert = "INSERT INTO SummaryTable VALUES('";
    for(a = 0;a < 4 ;a++)
    {
        SQLString_Insert.append(ItemList.at(a)->data(Qt::DisplayRole).toString());
        SQLString_Insert.append("','");
    }
    SQLString_Insert.append(ItemList.at(4)->data(Qt::DisplayRole).toString());
    SQLString_Insert.append("',DATE('");


//    qDebug() << ItemList.at(5)->data();//QVariant(QDate, QDate("2014-07-28") )
//    qDebug() << ItemList.at(5)->data().toDate();//QDate("2014-07-28")
//    qDebug() << ItemList.at(5)->data().toDate().toString("yyyy-MM-dd");//"2014-07-28"
     qDebug() << ItemList.at(5)->data().toString();//"2014-07-28"

     SQLString_Insert.append(ItemList.at(5)->data().toString());
     SQLString_Insert.append("'),DATE('");
     SQLString_Insert.append(ItemList.at(6)->data().toString());
     SQLString_Insert.append("'),'");

     SQLString_Insert.append(ItemList.at(7)->data(Qt::DisplayRole).toString());
     SQLString_Insert.append("','");

     SQLString_Insert.append(ItemList.at(8)->data(Qt::DisplayRole).toString());
     SQLString_Insert.append("',DATE('");

     SQLString_Insert.append(ItemList.at(9)->data().toString());
     SQLString_Insert.append("'),'");

    for(a = 10;a < 14 ;a++)
    {
        SQLString_Insert.append(ItemList.at(a)->data(Qt::DisplayRole).toString());
        SQLString_Insert.append("','");
    }

    SQLString_Insert.append(ItemList.at(14)->data(Qt::DisplayRole).toString());
    SQLString_Insert.append("')");

    qDebug() << SQLString_Insert;

    // 执行数据库操�?
    dbOpen();
    QSqlQuery query(db);
    if(query.exec(SQLString_Insert))
    {
        qDebug("SQL exec :Insert equipments successful!");
    }
    else
    {
       qDebug("SQL exec :Insert equipments Failed!");
    }

    SQLString_Insert.clear();

    if((ui->listView->currentIndex().row())== 0)
    {
        SQLString_Insert = SQLString_EquipmentAll;
    }
    else if((ui->listView->currentIndex().row())== 1)
    {
        SQLString_Insert = SQLString_EquipmentCAL;
    }
    else if((ui->listView->currentIndex().row())== 2)
    {
        SQLString_Insert = SQLString_EquipmentVER;
    }
    else if((ui->listView->currentIndex().row())== 4)
    {
        SQLString_Insert = SQLString_EquipmentOther;
    }
    updateData2Tableview(SQLString_Insert);
    dbClose();

    SQLString_Insert.clear();

    //如果是CAL 或VER 的设备，则需要同时更�? cal和ver�? cal/ver date,due date 到calequipment和verequipment table�?
 //   SQLString_Insert = "INSERT INTO SummaryTable VALUES('";

    QString CALVERString;
    CALVERString = ItemList.at(13)->data(Qt::DisplayRole).toString();
    if( (CALVERString == "CAL") ||(CALVERString == "VER"))
    {
        if(CALVERString == "CAL")
        {
           SQLString_Insert = "INSERT INTO CalEquipments VALUES('";
        }
        else if(CALVERString == "VER")
        {
            SQLString_Insert = "INSERT INTO VerifyEquipments VALUES('";
        }
        SQLString_Insert.append(ItemList.at(3)->data(Qt::DisplayRole).toString());

        SQLString_Insert.append("',DATE('");
        SQLString_Insert.append(ItemList.at(5)->data().toString());

        SQLString_Insert.append("'),DATE('");
        SQLString_Insert.append(ItemList.at(6)->data().toString());

        SQLString_Insert.append("'),'");
        SQLString_Insert.append(ItemList.at(15)->data(Qt::DisplayRole).toString());

        SQLString_Insert.append("')");
        qDebug() << SQLString_Insert;

        dbOpen();
        QSqlQuery query2(db);
        if(query2.exec(SQLString_Insert))
        {
            qDebug("Insert cal/ver equipments date successful!");
        }
        else
        {
            qDebug("Insert cal/ver equipments date Failed!");
        }
        dbClose();
    }
    else //Other, No need to update the database
    {

    }
}


void MainWindow::updateSQLdb_ModifyItem(int database,QList <QString>ItemList)
{
    qDebug("Modify items");
    int a;
    a = database;

    if(a == 2)
    {
        qDebug("Modify ConsumableMaterial");

    }
    QString SQLString_UPDATE;
//UPDATE Person SET Address = 'Zhongshan 23', City = 'Nanjing' WHERE LastName = 'Wilson'
    SQLString_UPDATE = "UPDATE ConsumablesMaterial SET Material_SN = '";
    SQLString_UPDATE.append(ItemList.at(0));

    SQLString_UPDATE.append("',Asset_Number = '");
    SQLString_UPDATE.append(ItemList.at(1));

    SQLString_UPDATE.append("',Material_Name = '");
    SQLString_UPDATE.append(ItemList.at(2));

    SQLString_UPDATE.append("',Material_brand = '");
    SQLString_UPDATE.append(ItemList.at(3));

    SQLString_UPDATE.append("',Material_Mfr = '");
    SQLString_UPDATE.append(ItemList.at(4));

    SQLString_UPDATE.append("',Standard_Used = '");
    SQLString_UPDATE.append(ItemList.at(5));

    SQLString_UPDATE.append("' WHERE Asset_Number ='");
    SQLString_UPDATE.append(ItemList.at(1));

    SQLString_UPDATE.append("'");


    // 执行数据库操�?
    dbOpen();
    QSqlQuery query(db);
    if(query.exec(SQLString_UPDATE))
    {
        qDebug("Modify  consumable materials successful!");
    }
    else
    {
        qDebug("Modify  consumable materials Failed!");
    }
    updateData2Tableview(SQLString_ConsumableMaterial);

}

void MainWindow::updateSQLdb_ModifyEquipment(int database,QList <QStandardItem*>ItemList)
{
    qDebug("Modify equipment");
    int a;
    a = database;
    if(a == 1)
    {
        qDebug("Modify SummaryTable");

    }
    QString SQLString_Update;
//UPDATE Person SET Address = 'Zhongshan 23', City = 'Nanjing' WHERE LastName = 'Wilson'
    SQLString_Update = "UPDATE SummaryTable SET EquipmentName = '";
    SQLString_Update.append(ItemList.at(0)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("',ModelName = '");
    SQLString_Update.append(ItemList.at(1)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("',MfgName = '");
    SQLString_Update.append(ItemList.at(2)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("',IG_SN = '");
    SQLString_Update.append(ItemList.at(3)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("',Mfg_SN = '");
    SQLString_Update.append(ItemList.at(4)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("',Caldate = DATE('");
    SQLString_Update.append(ItemList.at(5)->data().toString());

    SQLString_Update.append("'),DueDate = DATE('");
    SQLString_Update.append(ItemList.at(6)->data().toString());

    SQLString_Update.append("'),Status = '");
    SQLString_Update.append(ItemList.at(7)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("',Spec_Description = '");
    SQLString_Update.append(ItemList.at(8)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("',PurchaseDate = DATE('");
    SQLString_Update.append(ItemList.at(9)->data().toString());

    SQLString_Update.append("'),PurchasePrice = '");
    SQLString_Update.append(ItemList.at(10)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("',Location = '");
    SQLString_Update.append(ItemList.at(11)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("',InternalCatergory = '");
    SQLString_Update.append(ItemList.at(12)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("',CalOrVerify = '");
    SQLString_Update.append(ItemList.at(13)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("',CalLocation = '");
    SQLString_Update.append(ItemList.at(14)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("' WHERE IG_SN = '");
    SQLString_Update.append(ItemList.at(3)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("'");

    qDebug() << SQLString_Update;

    // 执行数据库操�?
    dbOpen();

    QSqlQuery query(db);

    if(query.isActive())
    {
        qDebug("query is active!");
    }
    else
    {
        qDebug("query is inactive!");
    }


    if(query.exec(SQLString_Update))
    {
        qDebug("Modify SummaryTable successful!");
    }
    else
    {
       qDebug("Modify SummaryTable FAILED!");
    }

    SQLString_Update.clear();
    // Update 显示
    if((ui->listView->currentIndex().row())== 0)
    {
        SQLString_Update = SQLString_EquipmentAll;
    }
    else if((ui->listView->currentIndex().row())== 1)
    {
        SQLString_Update = SQLString_EquipmentCAL;
    }
    else if((ui->listView->currentIndex().row())== 2)
    {
        SQLString_Update = SQLString_EquipmentVER;
    }
    else if((ui->listView->currentIndex().row())== 4)
    {
        SQLString_Update = SQLString_EquipmentOther;
    }

    updateData2Tableview(SQLString_Update);

    qDebug() << SQLString_Update;
    qDebug("Update tableview successful!");

    SQLString_Update.clear();

    dbClose();

    //如果是CAL 或VER 的设备，则需要同时更�? cal和ver�? cal/ver date,due date
    //到calequipment和verequipment table�?

    //只更新最新的一条记录，不更改历史的cal ver 数据
    QString TEMP;

    if(ItemList.at(13)->data(Qt::DisplayRole).toString() == "CAL")
    {
       SQLString_Update = "UPDATE CalEquipments SET IG_SN = '";
       TEMP = "CalEquipments";

    }
    else if(ItemList.at(13)->data(Qt::DisplayRole).toString() == "VER")
    {
        SQLString_Update = "UPDATE VerifyEquipments SET IG_SN = '";
        TEMP = "VerifyEquipments";
    }
    SQLString_Update.append(ItemList.at(3)->data(Qt::DisplayRole).toString());

    SQLString_Update.append("',CalDate = DATE('");
    SQLString_Update.append(ItemList.at(5)->data().toString());

    SQLString_Update.append("'),DueDate = DATE('");
    SQLString_Update.append(ItemList.at(6)->data().toString());

    SQLString_Update.append("') WHERE CalDate IN (SELECT CalDate FROM ");
    SQLString_Update.append(TEMP);
    SQLString_Update.append(" WHERE IG_SN = '");
    SQLString_Update.append(ItemList.at(3)->data(Qt::DisplayRole).toString());
    SQLString_Update.append("' ORDER BY CalDate desc LIMIT 0,1)");


    //只更新最新的一条记录，不更改历史的cal ver 数据,IG_SN并非唯一�?
/*
 * UPDATE CalEquipments SET IG_SN = 'F-IG-L-070',CalDate = DATE('2016-07-19'),DueDate = DATE('2017-07-18')
 * WHERE CalDate IN
 * (SELECT CalDate FROM CalEquipments WHERE IG_SN = 'F-IG-L-070' ORDER BY CalDate desc LIMIT 0,1)
 */
    qDebug() << SQLString_Update;

    dbOpen();
    QSqlQuery query2(db);

    if(query2.exec(SQLString_Update))
    {
        qDebug("Modify CalEquipments/VerifyEquipments successful!");
    }
    else
    {
        qDebug("SQL Modify CalEquipments/VerifyEquipments failed!");
    }

    dbClose();
}

void MainWindow::Slot_UpdateTableViewfromCalVerDateDlg()
{
    QString SQLString_Update;
    if((ui->listView->currentIndex().row())== 0)
    {
        SQLString_Update = SQLString_EquipmentAll;
    }
    else if((ui->listView->currentIndex().row())== 1)
    {
        SQLString_Update = SQLString_EquipmentCAL;
    }
    else if((ui->listView->currentIndex().row())== 2)
    {
        SQLString_Update = SQLString_EquipmentVER;
    }
    else if((ui->listView->currentIndex().row())== 4)
    {
        SQLString_Update = SQLString_EquipmentOther;
    }
    dbOpen();
    updateData2Tableview(SQLString_Update);
    dbClose();

}

void MainWindow::Search_ButtonClicked()
{
    //组织search的SQL 语句
    QString SQL_Search;
    QString Search_Condition,Search_Items;//ziduan

    Search_Condition = ui->lineEdit_Search->text();

    if(Search_Condition.isEmpty())
    {
        QMessageBox Msg_warning;
        Msg_warning.setWindowTitle("Warning!");
        Msg_warning.setText("Please fill the search conditin first!");

        Msg_warning.setStandardButtons(QMessageBox::Ok);

        Msg_warning.setIcon(QMessageBox::Warning);
        Msg_warning.exec();

        return;
    }


    switch(ui->comboBox_Search->currentIndex())
    {
        case 0:Search_Items = "IG_SN";break;
        case 1:Search_Items = "ModelName";break;
        case 2:Search_Items = "Location";break;
        default: Search_Items = "IG_SN";break;
    }

    SQL_Search = "SELECT EquipmentName,ModelName,IG_SN,Mfg_SN,Caldate,DueDate,Status,Spec_Description,InternalCatergory,CalOrVerify from SummaryTable WHERE ";
    SQL_Search.append(Search_Items);
    SQL_Search.append(" = '");
    SQL_Search.append(Search_Condition);
    SQL_Search.append("'");

    qDebug() << SQL_Search;
    //UPDATE TABLEVIEW
    dbOpen();
    updateData2Tableview(SQL_Search);
    dbClose();

}

void MainWindow::AboutThis()
{

    QString AboutString;

    AboutString = "This is a equipment calibration and verify record database client\r\n\
            for Fluke CPD Product Evaluation Team\r\n\r\n \
            Release Version: V1.0\r\n\r\n\
            Designed by Bob Cao       Email:yongbo.cao@fluke.com\r\n\
            Fluke CPD Copyright (c) 2014";
    QMessageBox::about(this,"About",AboutString);

}


/*
 * void MainWindow::AboutQT()
//{

//    QMessageBox::aboutQt(this,"About QT");

//}
*/

/*
void MainWindow::updateSQLdb_UpdateDate(QList <QStandardItem*>ItemList)
{
    qDebug("Update new date");

    QString SQLString_Insert,SQLString_Update;

    if(CalVerCat == Calibration_Equipment)//CalVerCat 全局变量
    {
        SQLString_Insert = "INSERT INTO CalEquipments VALUES('";
    }
    else if(CalVerCat == Verify_Equipment)
    {
        SQLString_Insert = "INSERT INTO VerifyEquipments VALUES('";
    }
    SQLString_Insert.append(ItemList.at(0)->data(Qt::DisplayRole).toString());

    SQLString_Insert.append("',DATE('");
    SQLString_Insert.append(ItemList.at(1)->data().toString());
    SQLString_Insert.append("'),DATE('");
    SQLString_Insert.append(ItemList.at(2)->data().toString());
    SQLString_Insert.append("'),'");

    SQLString_Insert.append(ItemList.at(3)->data(Qt::DisplayRole).toString());
    SQLString_Insert.append("')");

    qDebug() << SQLString_Insert;

    // 执行数据库操�
    dbOpen();
    QSqlQuery query(db);
    if(query.exec(SQLString_Insert))
    {
        qDebug("Insert new cal/ver date successful!");
    }
    else
    {
        qDebug("Insert new cal/ver date failed!");
    }

    SQLString_Insert.clear();

    qDebug("Update SUMMARY DATE date");
 // UPDATE SummaryTable SET CalDate = DATE(''),DueDate = DATE('') WHERE IG_SN = ''
    SQLString_Update = "UPDATE SummaryTable SET CalDate = DATE('";
    SQLString_Insert.append(ItemList.at(1)->data().toString());

    SQLString_Update.append("'),DueDate = DATE('");
    SQLString_Update.append(ItemList.at(2)->data().toString());

    SQLString_Update.append("') WHERE IG_SN = '");
    SQLString_Update.append(ItemList.at(0)->data(Qt::DisplayRole).toString());
    SQLString_Update.append("'");

    QSqlQuery query2(db);
    qDebug() << SQLString_Update;
    if(query2.exec(SQLString_Update))
    {
        qDebug("Update cal/ver equipments date successful!");
    }
    else
    {
        qDebug("Update cal/ver equipments date Failed!");
    }

  // TABVIEW REFRESH
    if((ui->listView->currentIndex().row())== 0)
    {
        SQLString_Insert = SQLString_EquipmentAll;
    }
    else if((ui->listView->currentIndex().row())== 1)
    {
        SQLString_Insert = SQLString_EquipmentCAL;
    }
    else if((ui->listView->currentIndex().row())== 2)
    {
        SQLString_Insert = SQLString_EquipmentVER;
    }

    updateData2Tableview(SQLString_Insert);

    SQLString_Insert.clear();

}
*/


void MainWindow::on_pushButton_Export_clicked()
{
    QString fileName;// ����save dialog��ȡ��filename

    QFileDialog saveDlg;

    fileName = saveDlg.getSaveFileName(this, tr("Save File"),"./export/*.xlsx",
                                                  tr("Excel File (*.xlsx *.xls *.csv)"));

    qDebug() << fileName;
    if (fileName.isEmpty())
    {
        qDebug("Please fill the file name.");
        /*
         * QString strInformation;
        strInformation = "Export failed!\n\nPlease fill the file name.";

        strInformation.append(fileName);

        QMessageBox::information(this,"Tips",strInformation,QMessageBox::Ok);
        */
        return;
    }
    else
    {
        ExportToXlsx(ui->tableView->model(),fileName);
    }

}

//tableview ���� excel �������úø�ʽ������
void MainWindow::ExportToXlsx(QAbstractItemModel* view, QString fileName)
{

    SendEmail();
    // create a new file
    //QXlsx::Document xlsx;
    int i = 0;
    int j = 0;

    // open a existing file
    QXlsx::Document *xlsx;
    xlsx = new QXlsx::Document();
    xlsx->addSheet("Equipment List",QXlsx::AbstractSheet::ST_WorkSheet);

    QXlsx::Format formatTitle;
    formatTitle.setFontColor(QColor(Qt::black));
    formatTitle.setFontSize(20);
    formatTitle.setFontBold(true);
    formatTitle.setHorizontalAlignment(QXlsx::Format::AlignLeft);


    // table Title
    int index = ui->listView->currentIndex().row();
    QString TitleString;
    switch(index)
    {
        case 0:TitleString = "All Equipment List:";break;
        case 1:TitleString = "Calibration Equipment List:";break;
        case 2:TitleString = "Verification Equipment List:";break;
        case 3:TitleString = "Cosumable Material List:";break;
        case 4:TitleString = "Other Equipment List:";break;
        default:TitleString = "Equipment List:";break;
    }
    xlsx->write(1,1,TitleString,formatTitle);


    QXlsx::Format formatHeader;
    formatHeader.setFontColor(QColor(Qt::blue));
    formatHeader.setFontSize(18);
    formatHeader.setFontBold(true);
    formatHeader.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    formatHeader.setBorderStyle(QXlsx::Format::BorderThick);
    formatHeader.setPatternBackgroundColor(QColor(Qt::gray));

     int colNum = view->columnCount();
     int RowNum = view->rowCount();

     // set colum eidth

     xlsx->setColumnWidth(1,10); //Column A: No.
     xlsx->setColumnWidth(2,50); //Column A: Equipment name
     xlsx->setColumnWidth(3,20); //Column A: Model.
     xlsx->setColumnWidth(4,15); //Column A: IGSN.
     xlsx->setColumnWidth(5,20); //Column A: MFR SN.
     xlsx->setColumnWidth(6,15); //Column A: CALDATE.
     xlsx->setColumnWidth(7,15); //Column A: DUEDATE.
     xlsx->setColumnWidth(8,10); //Column A: STATUS.
     xlsx->setColumnWidth(9,50); //Column A: DESCRIPTION.
     xlsx->setColumnWidth(10,15); //Column A:INTERNAL CAT.
     xlsx->setColumnWidth(11,20); //Column A: CAL OR VER.

   // table Header
     xlsx->write(2,1,"No.",formatHeader);

     for (i = 1;i < colNum + 1; i++)
     {
          xlsx->write(2,i+1,view->headerData(i-1,Qt::Horizontal,Qt::DisplayRole).toString(),formatHeader);
     }

        // table text
       QXlsx::Format formatText;
       formatText.setFontColor(QColor(Qt::black));
       formatText.setFontSize(12);
       //formatText.setFontBold(false);
       formatText.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
       formatText.setBorderStyle(QXlsx::Format::BorderThin);
       //formatText.setFontUnderline(QXlsx::Format::FontUnderlineSingle);
      // formatText.setFillPattern(QXlsx::Format::PatternLightUp);

       // Format range
       //xlsx.setRowFormat(1, RowNum, formatText);  //Whole row format
       //xlsx.setColumnFormat(1, colNum, formatTitle);//Whole col format
       //xlsx.write("A5", QDate(2013, 8, 29)); //date

       //QAbstractItemModel *modessl = ui->tableView->model();
       QAbstractItemModel *modessl = view;
       QModelIndex indextemp;
       int SequenceNum;
       for (i = 2;i < RowNum + 2 ; i++)
       {
           SequenceNum = i-1;
           xlsx->write(i+1,1,SequenceNum,formatText);
           for (j = 2; j < colNum + 2; j++)
           {
               indextemp = modessl->index(i-2,j-2);
               xlsx->write(i+1,j,modessl->data(indextemp).toString(),formatText);

           }
       }

       xlsx->saveAs(fileName);

       QString strInformation;
       strInformation = "Export successful!\r\n\r\nPlease find the exported excel file at the following Location:\r\n\r\n";

       strInformation.append(fileName);

       QMessageBox::information(this,"Tips",strInformation,QMessageBox::Ok);

       //ShellExecute(NULL,QString("Open").toStdWString().c_str(),fileName.toStdWString().c_str(),NULL,NULL,SW_SHOWMAXIMIZED);

}


//tableview ���� csv
void MainWindow::ExportToCsv(QAbstractItemModel* view, QString fileName)
{
    qDebug("enter into file save");


    QFile file(fileName);

    qDebug() << fileName;

    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        int cc = view->columnCount();

        qDebug() << cc;

        QStringList list;
        for (int i=0;i < cc;i++)
        {
            list<< view->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString();
        }
        stream << list.join(",")<<"\r\n";

        qDebug() << list << ","<< "\r\n";

        for (int i=0;i<view->rowCount();i++)
        {
            list.clear();
            for (int j=0;j<cc;j++)
            {
                list<<view->index(i,j).data().toString();
            }
            qDebug() << list << ","<< "\r\n";
            stream<< list.join(",")<<"\r\n";

        }
    }
}

void MainWindow::on_pushButton_ViewMoreInfo_clicked()
{

    MoreInforDlg* ViewMoreInforDlg;
    ViewMoreInforDlg = new MoreInforDlg(this);

    QString STRsend;

    if((ui->listView->currentIndex().row())== 0)
    {
        STRsend = "SELECT * FROM SummaryTable";
    }
    else if((ui->listView->currentIndex().row())== 1)
    {
        STRsend = "SELECT * FROM SummaryTable where CalOrVerify = 'CAL'";
    }
    else if((ui->listView->currentIndex().row())== 2)
    {
        STRsend = "SELECT * FROM SummaryTable where CalOrVerify = 'VER'";
    }
    else if((ui->listView->currentIndex().row())== 4)
    {
        STRsend = "SELECT * FROM SummaryTable where CalOrVerify NOT IN('CAL','VER')";
    }
    else
    {
       STRsend = "select * from ConsumablesMaterial";
    }

    // Update Moreinfodlg UI
    QObject::connect(this,SIGNAL(Signal_UpdateViewMoreInforDlgUI(QString)),ViewMoreInforDlg,SLOT(updateUI_MoreInfrDlg(QString)));
    emit Signal_UpdateViewMoreInforDlgUI(STRsend);

    //back to mainwondow export to excel slot
    QObject::connect(ViewMoreInforDlg,SIGNAL(Signal_ExportfromMoreInfor(QAbstractItemModel*, QString)),this,SLOT(ExportToXlsx(QAbstractItemModel*, QString)));

    ViewMoreInforDlg->exec();


}


void MainWindow::SendEmail()
{
    // First we need to create an SmtpClient object
    // We will use the Gmail's smtp server (smtp.gmail.com, port 465, ssl)
    SmtpClient smtp("outlookanywhere.danahertm.com", 443, SmtpClient::SslConnection);

    // We need to set the username (your email address) and the password
    // for smtp authentification.
    smtp.setUser("Global\\ycao3");
    smtp.setPassword("Test004Bob");


    // Now we create a MimeMessage object. This will be the email.
    MimeMessage message;

    message.setSender(new EmailAddress("yongbo.cao@fluke.com", "Bob"));
    message.addRecipient(new EmailAddress("yongbo.cao@fluke.com", "ycao3"));
    message.setSubject("SmtpClient for Qt - Demo");

    // Now add some text to the email.    // First we create a MimeText object.
    MimeText text;
    text.setText("Hi,All\n\nThis is a Notification email message.\n\nThe IG Lab equipment calibration date will be expired, You may be arrage the calibration task in the following 30 days.\n\nThis is a auto send message, Please DO NOT Reply this email, \nNotification from IG Lab equipment management Tool. ");

    // Now add it to the mail
    message.addPart(&text);

    // Now we can send the mail
    if(smtp.connectToHost())
    {
        qDebug("Connect to Host successfully!");
    }
    else
    {
        qDebug("Connect to Host Failed!");
    }

    if(smtp.login())
    {
        qDebug("Login successfully!");
    }
    else
    {
        qDebug("Login Failed!");
    }

    if(smtp.sendMail(message))
    {
        qDebug("Send Email successfully!");
    }
    else
    {
        qDebug("Send Email Failed!");
    }

    smtp.quit();

}


/*
void MainWindow::communication(QTcpSocket & socket, const char *msg)
{
        char data[1024];

        if (socket.write(msg, qstrlen(msg)) == -1)
        {
                qDebug() << "@@@@@@@@@@@@@@ socket.write failed";
        }
        socket.flush();

        if (socket.waitForReadyRead(-1) == true)
        {
            memset(data, '\0', sizeof(data));
            socket.readLine(data, 1024);
            qDebug() << data;
        }
}


void MainWindow::smtpCommunication(QTcpSocket & socket)
{
        communication(socket, "hello outlookanywhere.danahertm.com\r\n");
        communication(socket, "auth login\r\n");
        communication(socket, QByteArray("yongbo.cao@fluke.com").toBase64()+"\r\n");
        communication(socket, QByteArray("Caoyongbo").toBase64()+"\r\n");
        communication(socket, "mail from: <yongbo.cao@fluke.com>\r\n");
        communication(socket, "rcpt to: <yongbo.cao@fluke.com>\r\n");
        communication(socket, "data\r\n");
        communication(socket, "From: yongbo.cao@fluke.com\r\nTo: yongbo.cao@fluke.com\r\n"
                              "Subject: QT EMAIL\r\n\r\n"
                              "QT EMail Test!"
                              "\r\n.\r\n");
        communication(socket, "quit\r\n");

        qDebug() << "send email ok." << endl;
}

void MainWindow::readWelcome(QTcpSocket & socket)
{
        char data[1024];
        int len;

        if (socket.waitForReadyRead(-1) == true)
        {
            memset(data, '\0', sizeof(data));
            len = socket.readLine(data, 1024);
            qDebug() << data << endl;
        }
}
*/
