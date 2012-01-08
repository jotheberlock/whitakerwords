#include <QtCore/QProcess>
#include <QtCore/QFile>
#include <QtCore/QCoreApplication>
#include <QtCore/QTextStream>
#include <QtCore/QHash>
#include <QtCore/QDataStream>

class WhitLocation
{
  public:

    qint64 pos;
    qint64 len;
    
};

QDataStream & operator<< ( QDataStream & out, const WhitLocation val )
{
    out << val.pos;
    out << val.len;
    return out;
}

int main(int argc, char ** argv)
{
    QHash<QString,WhitLocation> hash;
    
    QCoreApplication app(argc,argv);
    QFile input("listall");
    QFile index("whitindex.cal");
    QFile output("whitdefinitions.cal");

    input.open(QFile::ReadOnly);
    index.open(QFile::WriteOnly);
    output.open(QFile::WriteOnly);

    QTextStream is(&input);
    QTextStream ins(&index);

    QString line;

    while(!is.atEnd())
    {
        line = is.readLine();
        
        QProcess process;
        QStringList args;
        args.push_back(line);
        process.start("./words", args);
        process.waitForStarted();
        process.write("\n\n\n\n\n");
        process.waitForFinished();
        QByteArray ret = process.readAllStandardOutput();
        QString retstring(ret);

        app.processEvents();
        
        ins << line << endl;
        ins << QString::number(output.pos()) << endl;
        QByteArray outd = retstring.toUtf8();
        ins << outd.length() << endl;
        WhitLocation wl;
        wl.pos = output.pos();
        wl.len = outd.length();
        line=line.remove("\n");
        hash[line]=wl;
        output.write(outd);
        output.flush();
    }

    QFile output2("whitindexhash.cal");
    output2.open(QFile::WriteOnly);
    QDataStream qds(&output2);
    qds << hash;
}
