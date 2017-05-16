#ifndef UPDATECALVERDATEDLG_H
#define UPDATECALVERDATEDLG_H

#include <QDialog>
#include <QStandardItem>
#include <QtSql>
#include "typedef.h"


namespace Ui {
class UpdateCalVerDateDlg;
}

class UpdateCalVerDateDlg : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateCalVerDateDlg(QWidget *parent = 0);
    ~UpdateCalVerDateDlg();


public:
    QSqlDatabase dbCalVer;
    CalOrVer CalVerCatHistory;
    QStandardItemModel *TableModelCalver;


signals:
    //void Signal_UpdateDate(QList <QStandardItem*>);//信号，在点击ok按钮式发�?
    void Signal_UpdateTableViewfromCalVerDateDlg();


public slots:
    void Update_ButtonClicked();
    void Cancel_ButtonClicked();
    void TableViewHistory_Clicked();
    void update_UpdateDateUI(CalOrVer, QString, int);

private:
    Ui::UpdateCalVerDateDlg *ui;
};

#endif // UPDATECALVERDATEDLG_H
