#pragma once
#include "../commonUsage/cerror.h"
#include "../commonUsage/comStruct.h"
#include "QLine"

namespace CSHDetect {

#ifndef CONSTIMG
#define CONSTIMG const HalconCpp::HObject&
#endif

#ifndef RHOBJ
#define RHOBJ HalconCpp::HObject&
#endif

struct rectPos {
    int row1;
    int col1;
    int row2;
    int col2;
};

struct VLineData {
    int dilaWid = 30;
    int eroHigh = 40;
    int threSubValue = 245;
    int minConLen = 300;

    float angleDown = 1; //角度上下限
    float angleUp = 3;

    float angle = 0;
};

struct HLineData {
    float selectRatio = 0.5; //筛选的长度比例
    int xExpand = 30;
    int yExpand = 30;
    int subThreVal = 240;

    float angleDown = -0.2; //角度上下限
    float angleUp = 0.2;

    float angle = 0;
};

class CGetTapeAnglePrivate;
class CGetTapeAngle
{
public:
    CGetTapeAngle();

    /**
     * @brief detect: 程序外部使用接口，获取各部分的参数。
     */
    CError detect(CONSTIMG dst, const ReverRegionS& res, QList<QLine>& gdLine, QList<QLine>& ngLine);

    /**
     * @brief pamRead: 读取检测参数
     * @return
     */
    CError pamRead(const char* xmlfilePath);

    /**
     * @brief setUpPam: 设置上边缘参数
     */
    void setUpPam(const HLineData& pam);

    /**
     * @brief setDownPam： 设置下边缘参数
     */
    void setDownPam(const HLineData& pam);

    /**
     * @brief setVPam：设置长边参数
     */
    void setVPam(const VLineData& pam);

public:
    /**
     * @brief checkTapeNum: 对蓝胶区域的数量进行检测。
     * @param tapeReg：输入蓝胶区域
     * @param connectReg：connect以后的区域
     * @param num: 标准值
     */
    CError checkTapeNum(CONSTIMG tapeReg, RHOBJ connectReg, const int num);

    /**
     * @brief slctRegion :根据象限来选择对应的单个蓝胶区域
     * @param hConnect：connect以后的区域
     * @param region：结果
     * @param quadrant：0：第一象限，... 3：第四象限
     */
    CError slctRegion(CONSTIMG hConnect, RHOBJ region, int quadrant);

    /**
     * @brief getSignalLines；对选择的蓝胶区域求边界的直线角度
     * @param dst：检测原图
     * @param hConnect：connect以后的区域
     * @param lines：结果直线
     * @param quadrant：象限值
     * @param upPam：上边缘直线检测参数
     * @param dnPam：下边缘直线检测参数
     * @param vPam： 竖线的检测参数
     */
    CError getSignalLines(CONSTIMG dst, CONSTIMG hConnect, QList<QLine>& lines, int quadrant,
                          HLineData& upPam, HLineData& dnPam, VLineData& vPam);

    /**
     * @brief getHline：用于横线的倾斜角度检测
     * @param dst : 原图
     * @param sigTapeReg：选择后的单个蓝胶区域
     * @param line： 结果直线
     * @param pos：参考直线
     * @param pam：检测参数
     * @return
     */
    CError getHline(CONSTIMG dst, CONSTIMG sigTapeReg, QLine& line, const QLine pos, HLineData& pam);

    /**
     * @brief getVline 用于竖线的倾斜角度检测
     * @param dst 原图
     * @param sigTapeReg 选择后的单个蓝胶区域
     * @param line 结果直线
     * @param pam 检测参数
     * @return
     */
    CError getVline(CONSTIMG dst, CONSTIMG sigTapeReg, QLine& line, VLineData& pam);

private:
    CGetTapeAnglePrivate* d;
    QString taskName = "GetTapeAngle";

};


}

