#ifndef ADDNEWITEM_H
#define ADDNEWITEM_H

#include <QDialog>
#include <QStandardItem>
#include "typedef.h"




namespace Ui {
class addNewItem;
}

class addNewItem : public QDialog
{
    Q_OBJECT

public:
    explicit addNewItem(QWidget *parent = 0);
    ~addNewItem();

public:
    DB_Update_Catgroy Items_ModifyorNew;

signals:
    void Signal_addItem(int database,QList <QString>);//信号，在点击ok按钮式发出
    void Signal_ModifyItem(int ,QList <QString>);

public slots:
   void updateUI_items(QList<QString>,DB_Update_Catgroy);




public slots:
    void SaveAll_ButtonClicked();
    void Cancel_ButtonClicked();

public:
    Ui::addNewItem *ui;
};

#endif // ADDNEWITEM_H


