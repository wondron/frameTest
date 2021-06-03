#ifndef ECHONTERFACE_H
#define ECHONTERFACE_H

#include <QString>
#include "QtCore"

//定义接口
class EchoInterface
{
public:
    virtual ~EchoInterface() {}
    virtual void echo(QObject * parent) = 0;
};

#define EchoInterface_iid "Examples.Plugin.EchoInterface"

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(EchoInterface, EchoInterface_iid)
QT_END_NAMESPACE

#endif // ECHONTERFACE_H
