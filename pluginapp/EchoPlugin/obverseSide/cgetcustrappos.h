#pragma once
#include "../commonUsage/cerror.h"
#include "halconcpp/HalconCpp.h"

namespace CSHDetect{

class CGetCuStrapPos
{
public:
    explicit CGetCuStrapPos(const int wid = 5472, const int height = 3648);

    explicit CGetCuStrapPos(const QString taskName);

    void setImgSize(const int width, const int height);

    CError testPos(int& position, const HalconCpp::HObject& desImg, HalconCpp::HObject& resObj,
                  const int widHeight, const int maxThre = 220, const int direction = 1);

    CError testPos(int& position, const HalconCpp::HObject& desImg, HalconCpp::HObject& resObj,
                  const int maxThre = 220, const int direction = 1);

    CError detect(const HalconCpp::HObject& desImg);

    CError pamRead(const char *xmlfilePath);

    void setTaskName(const char* name) {taskName = name;}

    const QString getTaskName() const {return taskName;}

public:
    int m_width;
    int m_height;

    int maxThre = 220;
    int CuPos = 3;
    int strapDirection = 1;
    QString taskName = "GetCuStrapPos";
};
}
