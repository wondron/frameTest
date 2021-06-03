#include "imagegroupbase.h"
#include "halconcpp/HalconCpp.h"

namespace CSHDetect {

imageGroupBase::imageGroupBase()
{

}

CSHDetect::CError imageGroupBase::detect()
{
    return 0;
}

CError imageGroupBase::readPam(const char *filepath)
{
    return 0;
}

void imageGroupBase::setTaskName(const QString &taskname)
{
    taskName = taskname;
}

}

