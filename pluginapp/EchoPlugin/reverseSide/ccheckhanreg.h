#pragma once
#include "../commonUsage/cerror.h"
#include "../commonUsage/comStruct.h"

namespace CSHDetect {

#ifndef CONSTIMG
#define CONSTIMG const HalconCpp::HObject&
#endif

#ifndef RHOBJ
#define RHOBJ HalconCpp::HObject&
#endif

/**
 *  CCheckHanReg : 检测反面状态下，极耳上面是由存在焊印。
 */
class CCheckHanRegPrivate;

class CCheckHanReg
{
public:
    CCheckHanReg();

    ~CCheckHanReg() {};

    /**
     * @brief detect: 程序外部使用接口，获取各部分的参数。
     */
    CError detect(CONSTIMG obj, const ReverRegionS& res, QList<QRect>& rects);

    /**
     * @brief pamRead: 读取检测参数
     * @return
     */
    CError pamRead(const char* xmlfilePath);

public:
    /**
     * @brief getHanRegion : 检测是否存在焊印区域，对四个部分一起做检测。
     * @param dst :检测原图
     * @param doubleTapeRoi：获取到的蓝胶区域（左右两个蓝胶叠加后的区域，正常状态下应该是2个）
     * @param hanRegion：处理后所得结果，用于在示教界面上显示用的。
     * @param hanHoleSize：定义焊印颗粒的大小，用直径表示。
     * @param dynThreValu：动态阈值大小
     * @param maxHanArea：最大的焊印颗粒面积
     * @param minHanRegionSize：认定为焊印区域的最小焊印颗数
     * @param rects: 最小外接矩形
     * @return
     */
    CError getHanRegion (CONSTIMG dst, CONSTIMG doubleTapeRoi, RHOBJ hanRegion,
                         int hanHoleSize, int dynThreValu, int dynKernel, int maxHanArea, int minHanRegionSize, QList<QRect>& rects);

private:
    CCheckHanRegPrivate* d;
    QString taskName = "CheckHanReg";
};

}

