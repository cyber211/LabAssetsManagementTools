#include "addnewequipment.h"
#include "ui_addnewequipment.h"
#include "mainwindow.h"

addNewEquipment::addNewEquipment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addNewEquipment)
{
    ui->setupUi(this);

    Items_ModifyorNew = New;
    connect(ui->pB_SaveAll_E,SIGNAL(clicked()),this,SLOT(SaveAll_ButtonClicked()));
    connect(ui->pB_Cancel_E,SIGNAL(clicked()),this,SLOT(Cancel_ButtonClicked()));
 }

addNewEquipment::~addNewEquipment()
{
    delete ui;
}

void addNewEquipment::SaveAll_ButtonClicked()
{
    qDebug("SaveAll_ButtonClicked");

    QStandardItem *item[16];
    QList <QStandardItem*> ItemList;

    //QStandardItem.data()

    item[0] = new QStandardItem(ui->lineEdit_EquipmentName->text());
    item[1] = new QStandardItem(ui->lineEdit_ModelName->text());
    item[2] = new QStandardItem(ui->lineEdit_MfrName->text());
    item[3] = new QStandardItem(ui->lineEdit_IGSN->text());
    item[4] = new QStandardItem(ui->lineEdit_MFRSN->text());

    item[5] = new QStandardItem();
    item[5]->setData(ui->dateEdit_Cal->date());
    //item[5] = new QStandardItem(ui->dateEdit_Cal->date());


    item[6] = new QStandardItem;
    item[6]->setData(ui->dateEdit_Due->date());

    item[7] = new QStandardItem(ui->comboBox_Status->currentText());

    item[8] = new QStandardItem(ui->plainTextEdit_Spec->toPlainText());

    item[9] = new QStandardItem;
    item[9]->setData(ui->dateEdit_Due->date());
 //   item[9] = new QStandardItem(ui->dateEdit_PurchaseDate->date());

    item[10] = new QStandardItem(ui->lineEdit_PurchasePrice->text());
    item[11] = new QStandardItem(ui->lineEdit_Location->text());

    item[12] = new QStandardItem(ui->lineEdit_InternalCatergory->text());

    if(ui->comboBox_CalorVer->currentText() == "Calibration by CNAS Lab")
    {
        item[13] = new QStandardItem("CAL");
    }
    else if(ui->comboBox_CalorVer->currentText() == "Verification by Internal Lab")
    {
        item[13] = new QStandardItem("VER");
    }
    else if(ui->comboBox_CalorVer->currentText() == "Other")
    {
       item[13] = new QStandardItem("Other");
    }

    item[14] = new QStandardItem(ui->lineEdit_CalLocation->text());
    item[15] = new QStandardItem(ui->lineEdit_CalPeriod->text());


    for(int i = 0;i< 16;i++)
    {
       ItemList << item[i];
   /*
       if(item[i])
       {
           QMessageBox Msg_warning;
           Msg_warning.setWindowTitle("Warning!");
           Msg_warning.setText("Item can not be empty");

           Msg_warning.setStandardButtons(QMessageBox::Ok);

           Msg_warning.setIcon(QMessageBox::Warning);
           Msg_warning.exec();
           return;
       }
       */
    }

    if(Items_ModifyorNew == New)
    {
        qDebug("add new equipment signal emit.");
        emit Signal_addEquipment(2,ItemList);
    }
    else if(Items_ModifyorNew == Modify) //emits Signal_Modify Equipment
    {
        qDebug("Modify equipment signal emit.");
        emit Signal_ModifyEquipment(2,ItemList);
    }

    this->close();
}

void addNewEquipment::Cancel_ButtonClicked()
{
    qDebug("Cancel_ButtonClicked");
    this->close();
}

void addNewEquipment::updateUI_Equipment(QList<QString>ItemList,DB_Update_Catgroy ModifyorNew)
{
    Items_ModifyorNew = ModifyorNew;
    ui->lineEdit_EquipmentName->setText(ItemList.at(0));
    ui->lineEdit_ModelName->setText(ItemList.at(1));
    ui->lineEdit_MfrName->setText(ItemList.at(2));
    ui->lineEdit_IGSN->setText(ItemList.at(3));
    ui->lineEdit_MFRSN->setText(ItemList.at(4));

    QString DateString = ItemList.at(5);
    QString DateFormat = "yyyy-MM-dd";
    QDate Date = QDate::fromString(DateString,DateFormat);
    ui->dateEdit_Cal->setDate(Date);

    DateString = ItemList.at(6);
    Date = QDate::fromString(DateString,DateFormat);
    ui->dateEdit_Due->setDate(Date);

    if(ItemList.at(7) == "Using")  //0
    {
       ui->comboBox_Status->setCurrentIndex(0);
       ui->comboBox_Status->setStyleSheet("background-color: green");
    }
    else if(ItemList.at(7) == tr("Í£ÓÃ"))//1
    {
      ui->comboBox_Status->setCurrentIndex(1);
      ui->comboBox_Status->setStyleSheet("background-color: yellow");
    }
    else if(ItemList.at(7) == "±¨·Ï")//2
    {
      ui->comboBox_Status->setCurrentIndex(2);
      ui->comboBox_Status->setStyleSheet("background-color: red");
    }

    ui->plainTextEdit_Spec->setPlainText(ItemList.at(8));

    DateString = ItemList.at(9);
    Date = QDate::fromString(DateString,DateFormat);
    ui->dateEdit_PurchaseDate->setDate(Date);

    ui->lineEdit_PurchasePrice->setText(ItemList.at(10));
    ui->lineEdit_Location->setText(ItemList.at(11));
    ui->lineEdit_InternalCatergory->setText(ItemList.at(12));
    if(ItemList.at(13) == "CAL")  //0
    {
       ui->comboBox_Status->setCurrentIndex(0);
    }
    else if(ItemList.at(13) == "VER")//1
    {
      ui->comboBox_Status->setCurrentIndex(1);
    }
    else if(ItemList.at(13) == "Other")
    {
        ui->comboBox_Status->setCurrentIndex(2);
    }

    ui->lineEdit_CalLocation->setText(ItemList.at(14));

    if(ModifyorNew == New)
    {
        ui->lineEdit_CalPeriod->setVisible(true);
        ui->label_CalPeriod->setVisible(true);

    }
    else if(ModifyorNew == Modify)
    {
      ui->lineEdit_CalPeriod->setVisible(false);
      ui->label_CalPeriod->setVisible(false);
    }
}
