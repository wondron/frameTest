#ifndef IMAGEGROUPBASE_H
#define IMAGEGROUPBASE_H
#include "commonUsage/cerror.h"

namespace CSHDetect {

class imageGroupBase
{
public:
    imageGroupBase();

    virtual CError detect();
    virtual CError readPam(const char* filepath);

    const QString getTaskName() const {return taskName;}
    void setTaskName(const QString& taskname);

private:
    QString taskName = "PamBase";
};

}
#endif // IMAGEGROUPBASE_H
