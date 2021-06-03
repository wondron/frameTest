#pragma once
#include "cgetregions.h"
#include "cgetcircleLen.h"
#include "cgetcustrappos.h"
#include "../commonUsage/commonDef.h"

namespace CSHDetect{

class CObverseSideDetect
{
public:
    CObverseSideDetect();

    CError PamRead(const char* filePath);
    DetectResult_t detect(HalconCpp::HObject img);

private:
    QVector<DetectLineInfo_t> transtoLineInfo(const QList<LineInfo>& lineInfo);
    void trans2LineInfo(const QList<LineInfo>& lineInfo, const QList<int> errList, QVector<DetectLineInfo_t>& gdLine, QVector<DetectLineInfo_t>& ngLine);

public:
    CGetRegions* m_grpRegions;
    CGetCuStrapPos* m_grpCuPos;
    CGetCircleLen* m_grpCircleLen;
};
}

