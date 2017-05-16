#include "addnewitem.h"
#include "mainwindow.h"
#include "ui_addnewitem.h"
#include <QStandardItem>
#include <QMessageBox>

addNewItem::addNewItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addNewItem)
{
    ui->setupUi(this);

    Items_ModifyorNew = New;
    connect(ui->pushButton_SaveAll,SIGNAL(clicked()),this,SLOT(SaveAll_ButtonClicked()));
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(Cancel_ButtonClicked()));

}

addNewItem::~addNewItem()
{
    delete ui;
}

void addNewItem::SaveAll_ButtonClicked()
{

    qDebug("SaveAll_ButtonClicked");

    QString item[6];
    QList <QString>ItemList;

    item[0] = ui->lineEdit_MaterialSN->text();
    item[1] = ui->lineEdit_AssetNum->text();
    item[2] = ui->lineEdit__MaterialName->text();
    item[3] = ui->lineEdit_MaterialBrand->text();
    item[4] = ui->lineEdit_Manufacture->text();
    item[5] = ui->textEdit_StandardUsed->toPlainText();
    for(int i = 0;i<6;i++)
    {
       ItemList << item[i];
       if(item[i].isEmpty())
       {
           QMessageBox Msg_warning;
           Msg_warning.setWindowTitle("Warning!");
           Msg_warning.setText("Item can not be empty");

           Msg_warning.setStandardButtons(QMessageBox::Ok);

           Msg_warning.setIcon(QMessageBox::Warning);
           Msg_warning.exec();
           return;
       }
    }

    if(Items_ModifyorNew == New)
    {
        emit Signal_addItem(1,ItemList);// 1: comsumable material
    }
    else if(Items_ModifyorNew == Modify)
    {
       emit Signal_ModifyItem(1,ItemList);// 1: comsumable material
    }

    this->close();
}

void addNewItem::Cancel_ButtonClicked()
{
    qDebug("Cancel_ButtonClicked");
    this->close();
}

void addNewItem::updateUI_items(QList<QString >ItemList,DB_Update_Catgroy ModifyorNew)
{
    Items_ModifyorNew = ModifyorNew;
    ui->lineEdit_MaterialSN->setText(ItemList.at(0));
    ui->lineEdit_AssetNum->setText(ItemList.at(1));
    ui->lineEdit__MaterialName->setText(ItemList.at(2));
    ui->lineEdit_MaterialBrand->setText(ItemList.at(3));
    ui->lineEdit_Manufacture->setText(ItemList.at(4));
    ui->textEdit_StandardUsed->setText(ItemList.at(5));
}
