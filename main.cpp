#include "mainwindow.h"
#include <QApplication>
#include "logindlg.h"
//#include "globalvarial.h"


//For log
#include <QtDebug>
#include <QFile>
#include <QTextStream>

#include "main.h"

extern bool log_on;

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);

    QFile file("log.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}
//For log


//回调函数实现debug信息到文件
/*
void customMessageHandler(QtMsgType type, const char *msg)
{
   QString txt;
   switch (type) {
   case QtDebugMsg:
       txt = QString("Debug: %1").arg(msg);
       break;

   case QtWarningMsg:
       txt = QString("Warning: %1").arg(msg);
   break;
   case QtCriticalMsg:
       txt = QString("Critical: %1").arg(msg);
   break;
   case QtFatalMsg:
       txt = QString("Fatal: %1").arg(msg);
       abort();
   }

   QFile outFile("debug.log");
   outFile.open(QIODevice::WriteOnly | QIODevice::Append);
   QTextStream ts(&outFile);
   ts << txt << endl;
}
*/



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    //如果需要静态直接启动就翻译，那么就直接添加下面命令在main函数里面
//    QTranslator translator;
//    translator.load(QString(":/translations/IGLabAM_cn.qm"));
//    app.installTranslator(&translator);

    if(log_on)
    {
        //注册MessageHandler
        qInstallMessageHandler(outputMessage); //For log
    }

    LoginDlg *Login;
    Login = new LoginDlg;
    if(Login->exec() == QDialog::Accepted)
    {
        MainWindow *w;
        w = new MainWindow(Login->Level,0);
/*
        QFile file(":/main.qss");
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());

        w->setStyleSheet(styleSheet);
        file.close();
*/
        w->show();

        return app.exec();
    }
    else
    {
        return 0;
    }

//    w.show();
//    return a.exec();
}
