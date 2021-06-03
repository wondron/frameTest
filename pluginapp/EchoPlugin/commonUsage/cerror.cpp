#include "cerror.h"

using namespace CSHDetect;

CError::CError(int i) : errIndex(i), errInfo("none")
{
}

CError::CError(int i, QString info):errIndex(i), errInfo(info)
{

}

void CError::setInfo(const QString &info)
{
    errInfo = info;
}

CError::operator int()
{
    return errIndex;
}



