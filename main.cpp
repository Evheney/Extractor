#include <QCoreApplication>

#include <QList>
#include <QMap>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QTime>
#include <QDebug>
#include <cstdlib>
#include <QProcess>

#include <qtconcurrentmap.h>

using namespace QtConcurrent;

/*
    Utility function that recursivily searches for files.
*/
QStringList findFiles(const QString &startDir, const QStringList &filters)
{
    QStringList names;
    QDir dir(startDir);

    const auto files = dir.entryList(filters, QDir::Files);
    for (const QString &file : files)
        names += startDir + '/' + file;

    const auto subdirs =  dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (const QString &subdir : subdirs)
        names += findFiles(startDir + '/' + subdir, filters);
    return names;
}


int runBatchQtQProcessWaited(const std::string& BatchFile) {
    QProcess Process;
    auto Command = QString("cmd.exe");
    auto Arguments = QStringList{
            QString("/C"),
            QString::fromStdString(BatchFile)
    };

    //Process.start(Command, Arguments);
    //Process.waitForFinished(-1);
    int rc = Process.execute(Command, Arguments);

//    QByteArray out_data = Process.readAllStandardOutput();
//    QString out_string(out_data);
//    qDebug() << out_string.toStdString().c_str();

    return rc; //Process.exitCode();
}

void Extract(const QString &fileName)
{
    QFileInfo fi(fileName);
    QString line = fi.path()+"/"+fi.baseName();
    qDebug() << line;

    QString systemCommand("apktool d -s --force-all -o " + line + " " + fileName);
    qDebug() << systemCommand;

    runBatchQtQProcessWaited(systemCommand.toStdString());
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //QApplication app(argc, argv);
    qDebug() << "finding files...";
    const QStringList files = findFiles("D:/Eugene/Damkook/TestingFilesApk", QStringList() << "*.apk" );
    qDebug() << files.count() << "files";

    for(const QString &apkName : files) {
        QFileInfo fi(apkName);
        QString line = fi.path()+"/"+fi.baseName();
        qDebug() << line;
    }

/*
    QTime time;
    time.start();
    for(const QString &apkName : files){
        QFileInfo fi(apkName);
        QString line = fi.path()+"/"+fi.baseName();
        qDebug() << line;

        QString systemCommand("apktool d -s --force-all -o " + line + " " + apkName);
        qDebug() << systemCommand;

        runBatchQtQProcessWaited(systemCommand.toStdString());

    }
    int singleThreadTime = time.elapsed();
    qDebug() << "Working time" << singleThreadTime << "ms";
*/
    int mapTime = 0;
    {
        QTime time;
        time.start();

        map(files, Extract);

        mapTime = time.elapsed();
        qDebug() << "Map time" << mapTime << "ms";
    }

    return a.exec();
}
