#ifndef ECHOPLUGIN_H
#define ECHOPLUGIN_H

#include <QObject>
#include "QtPlugin"
#include "Echonterface.h"

class EchoPlugin : public QObject, public EchoInterface
{
    Q_OBJECT

    #if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID EchoInterface_iid)
    #endif

    Q_INTERFACES(EchoInterface)
public:
    explicit EchoPlugin(QObject *parent = 0);
    void echo(QObject *parent);
};

#endif // ECHOPLUGIN_H
