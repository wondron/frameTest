#include "aiplugin.h"
#include "aiwidget.h"
#include "graphics/wgraphicswidget.h"
#include "cgetimagewidget.h"
#include "commonWidget/cimageviewhalcon.h"

AiPlugin::AiPlugin(QObject *parent) :
    QObject(parent)
{

}

void AiPlugin::echo(QObject *parent)
{
    Q_UNUSED(parent);
    qDebug()<<"enter AiPlugin";

    CGetImageWidget* widgt = new CGetImageWidget();
    widgt->show();
}

#if(QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2(EchoPlugin, EchoPlugin);
#endif
