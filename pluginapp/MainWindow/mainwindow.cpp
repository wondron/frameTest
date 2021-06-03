#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtGui"
#include "qdebug.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createGUI();
    setWindowTitle("echopluginExample");
    loadPlugin();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendEcho()
{
    echoInterface->echo(this);
}

void MainWindow::createGUI()
{
}

bool MainWindow::loadPlugin()
{
    bool ret = true;
    //获取当前应用程序所在路径
    QDir pluginsDirs(qApp->applicationDirPath());
    QStringList data = qApp->applicationDirPath().split('/');
    QString filepath;
    for(auto i = data.begin(); i != data.end()-2; i++){
        filepath += *i + QString("\\");
    }
    filepath += QString("plugins");
    qDebug()<<"filepath:"<<filepath;
    QDir pluginsDir(filepath);

    //切换到插件目录
    pluginsDir.cd("plugins");
    //遍历plugins目录下所有文件
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            //插件名称
            QString pluginName = plugin->metaObject()->className();
            qDebug() << "get the plugin Name:" << pluginName;
            //对插件初始化
            if (pluginName == "EchoPlugin") {
                echoInterface = qobject_cast<EchoInterface *>(plugin);
                echoInterface->echo(this);
            } else if(pluginName == "AiPlugin"){
                echoInterface = qobject_cast<EchoInterface *>(plugin);
                echoInterface->echo(this);
            }

            else {
                ret = false;
            }
        }
    }
    return ret;
}

