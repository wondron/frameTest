#include "aiplugin.h"
#include "aiwidget.h"

AiPlugin::AiPlugin(QObject *parent) :
    QObject(parent)
{

}

void AiPlugin::echo(QObject *parent)
{
    qDebug()<<"enter AiPlugin";

    AiWidget* widgt = new AiWidget();
    widgt->show();
}

#if(QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2(EchoPlugin, EchoPlugin);
#endif
