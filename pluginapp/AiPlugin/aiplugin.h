#pragma once

#include <QObject>
#include "QtPlugin"
#include "Echonterface.h"

class AiPlugin : public QObject, public EchoInterface
{
    Q_OBJECT

#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID EchoInterface_iid)
#endif

Q_INTERFACES(EchoInterface)

public:
    explicit AiPlugin(QObject *parent = 0);
    void echo(QObject *parent);
};

