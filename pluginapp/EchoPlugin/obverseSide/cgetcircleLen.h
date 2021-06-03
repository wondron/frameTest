#pragma once

#include "../commonUsage/cerror.h"
#include "../commonUsage/comStruct.h"
#include "qhash.h"
#include "qpoint.h"

namespace CSHDetect{

class CGetCircleLenPrivate;

class CGetCircleLen
{
public:

    CGetCircleLen();

    explicit CGetCircleLen(const QString name);

    CError detect(const RegionS& testInfo, QList<LineInfo> &lineList, QList<int> &errIndex);

    CError getLineInfo(const RegionS& testInfo, QList<LineInfo> &lineList);

    CError getCircleCenter(const RegionS& testInfo, CircleInfo &smallPos, CircleInfo &bigPos,
                           int ringRadius = 210, int dilaLength = 4500, int direction = 1);

    CError getDetectLine(const RegionS& Info, HalconCpp::HObject &showObj, LineInfo &line, int quadrantPos = 0, int direction = 1);

    CError pamRead(const char *xmlfilePath);

    void setTaskName(const char* name) {taskName = name;}

    const QString getTaskName() const {return taskName;}

private:
    CError selectRegion(const RegionS& Info, HalconCpp::HObject& region, int quadrantPos = 0);

    CError slctRows(double& top, double& button, const double meanRow, const int quadrantPos = 0);

    CGetCircleLenPrivate* d;

    QString taskName = "GetCircleLen";
};
}
