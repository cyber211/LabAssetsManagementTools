#ifndef ADDNEWEQUIPMENT_H
#define ADDNEWEQUIPMENT_H

#include <QDialog>
#include <QStandardItem>
#include "typedef.h"



namespace Ui {
class addNewEquipment;
}

class addNewEquipment : public QDialog
{
    Q_OBJECT

public:
    explicit addNewEquipment(QWidget *parent = 0);
    ~addNewEquipment();

public:
    DB_Update_Catgroy Items_ModifyorNew;

signals:
    void Signal_addEquipment(int,QList <QStandardItem*>);//信号，在点击ok按钮式发出
    void Signal_ModifyEquipment(int,QList <QStandardItem*>);


public slots:

    void updateUI_Equipment(QList<QString>,DB_Update_Catgroy);

public slots:
    void SaveAll_ButtonClicked();
    void Cancel_ButtonClicked();



private:
    Ui::addNewEquipment *ui;
};

#endif // ADDNEWEQUIPMENT_H
