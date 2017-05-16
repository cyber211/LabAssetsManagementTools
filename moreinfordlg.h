#ifndef MOREINFORDLG_H
#define MOREINFORDLG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QtSql>


namespace Ui {
class MoreInforDlg;
}

class MoreInforDlg : public QDialog
{
    Q_OBJECT

public:
    explicit MoreInforDlg(QWidget *parent = 0);
    ~MoreInforDlg();


    QSqlDatabase MoreInfoDB;
    QStandardItemModel *TableModelMoreInfo;

signals:
    void Signal_ExportfromMoreInfor(QAbstractItemModel*, QString);


public slots:
    void updateUI_MoreInfrDlg(QString);


private slots:
    void on_pushButton_Cancel_clicked();

    void on_pushButton_Export_clicked();

private:
    Ui::MoreInforDlg *ui;
};

#endif // MOREINFORDLG_H
