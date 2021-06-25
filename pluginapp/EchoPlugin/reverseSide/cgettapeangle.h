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

//调整焊印中心转换到直线检测的区域
struct PosBia
{
    int XBia;         //区域中心的X偏移
    int YBia;         //区域中心的Y偏移
    int lineDirect;   //直线是垂直：0， 还是水平：1
    int lineWidth;    //检测长度
    int detectTime    = 15;   //检测次数
    double angleDown  = 10; //角度下限
    double angleUp    = -10;//角度上限
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

public:

    /**
     * @brief getPamValue:获取检测参数
     */
    void getPamValue(QList<MeasureposPam>& pam, QList<PosBia> &bia);

    /**
     * @brief getSignalLines: 对选择的蓝胶区域求边界的直线角度
     * @param dst：检测原图
     * @param hConnect：connect以后的区域
     * @param lines：结果直线
     * @param quadrant：象限值
     * @param biaInfo: 用于获取检测拟合直线的区域
     * @param pam：检测参数
     */
    CError getSignalLines(CONSTIMG dst, CONSTIMG hConnect, LineInfo& lines, const int quadrant, const PosBia biaInfo, const MeasureposPam pam);

private:
    CGetTapeAnglePrivate* d;
    QString taskName = "GetTapeAngle";
};


}

