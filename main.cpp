#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>
#include <QFontDatabase>
#include <QStyleFactory>

#include "globaleventfilter.h"

int main(int argc, char *argv[])
{
    qDebug() << "AppStartInit...";
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);


    QApplication a(argc, argv);

//    a.setStyle(QStyleFactory::create("motif"));
    //    a.setStyle(QStyleFactory::create("windows"));

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif
    //设置中文编码
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
#endif

#if 0
    QTranslator translator;
    if (translator.load(QLocale(), "myapp", "_", ":/translations")) {
        app.installTranslator(&translator);
    } else {
        qDebug() << "Translation file not found.";
    }
#endif

    MainWindow w;

//    QMetaObject::invokeMethod(&w,"slotShowSubWindow",Qt::QueuedConnection);
    w.show();

    w.slotShowSubWindow();

    GlobalEventFilter* globalFilter = new GlobalEventFilter();
    a.installEventFilter(globalFilter);

    return a.exec();
}
