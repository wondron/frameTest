#include "echoplugin.h"
#include "halcondlg.h"
#include "commonUsage/imageviewerhalcon.h"
#include "qdebug.h"
#include "totaltestwidget.h"
#include "totaltestwidgetr.h"
#include "obverseSide/cobversesidedetect.h"
#include "reverseSide/creversesidedetect.h"
#include "totalalg.h"
#include "test.h"
#include "graphics/wgraphicswidget.h"

EchoPlugin::EchoPlugin(QObject *parent) :
    QObject(parent)
{
}

void EchoPlugin::echo(QObject *parent)
{

    qDebug()<<"enter echo plugin";
    Graphics::WGraphicsWidget *widget = new Graphics::WGraphicsWidget();
    widget->setMinimumSize(600,600);
    widget->show();
}

#if(QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2(EchoPlugin, EchoPlugin);
#endif
