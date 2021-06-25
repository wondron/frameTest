#pragma once
#include "halconcpp/HalconCpp.h"
#include "comStruct.h"
#include "cerror.h"
#include "qpoint.h"
#include "comStruct.h"

namespace CSHDetect {

#ifndef CONSTIMG
#define CONSTIMG const HalconCpp::HObject&
#endif

#ifndef RHOBJ
#define RHOBJ HalconCpp::HObject&
#endif

typedef const int cint;

class Algorithm
{
public:
    Algorithm();

    static CError objIsEmpty(CONSTIMG obj);
    static CError tupleisEmpty(const HalconCpp::HTuple& tuple);
    static CircleInfo getAverCircle(QList<CircleInfo> info);

    static QPointF getCrossPoint(qreal pX, qreal pY, qreal lX1, qreal lY1, qreal lX2, qreal lY2);
    static LineInfo getCrossInfo(qreal pX, qreal pY, const LineInfo& info);

    static const QString getErrDescri(cint errIndex);

    static HalconCpp::HObject dynThre(CONSTIMG dst, cint threVal, cint meanSize);

    /**
     * @brief edgeLineDtct: 检测边缘参数
     * @param dst：检测图像
     * @param direct：直线检测的方向， 0：上下，1：左右
     * @param directTime：检测的次数
     * @param dtctRegion：直线检测区域确定，起始点，终点
     * @param measurePam：measurepos使用的参数，
     *                      len1/len2:确定单点检测区域
     *                      transition：0：all, 1:positive, 2:nagetive
     *                      pntSelect : 0:all, 1: first, 2: second
     *                      sigma,
     *                      threshold
     * @param resLine：检测结果直线参数
     * @return
     */
    static CError edgeLineDtct(CONSTIMG dst, RHOBJ resObj, LineInfo &resLine, cint direct, cint dtctTime,
                               const LineInfo dtctRegion, const MeasureposPam measurePam);

    /**
     * @brief useGridGetRegion: 快速分割区域
     * @param dst： 检测原图
     * @param gridWid：网格宽度
     * @param gridHigt：网格高度
     * @param minThre：目标区域最小的灰度值
     * @param eroValue：腐蚀量
     * @param slctNum：连通域个数（从最大的开始计算）
     * @param resRegion：结果obj
     */
    static CError useGridGetRegion(CONSTIMG dst, RHOBJ resRegion, cint gridWid, cint gridHigt, cint minThre, cint eroValue, cint slctNum);


    /**
     * @brief getRegionByQuadrant: 根据象限来选择对应的区域
     * @param dstObj：输入的区域
     * @param region：结果值
     * @param quadrant：需要获得的象限序号
     * @return
     */
    static CError getRegionByQuadrant(CONSTIMG dstObj, RHOBJ region, cint quadrant);

    /**
     * @brief detectRegionExit: 检测某个区域的是否存在灰度范围内的蓝胶
     * @param dstObj：输入图像
     * @param inRect：输入的检测区域
     * @param gdRect：符合条件的区域
     * @param ngRect：不符合条件的区域
     * @param basePt: 基准点，用于定位矩形的位置
     * @param minthre: 阈值下限
     * @param maxThre: 阈值上限
     * @param percent: 所筛选region占矩形的最小比例
     * @return
     */
    static CError detectRegionExit(CONSTIMG dstObj, RHOBJ showObj, const QVector<RectInfo> &inRect, QVector<RectInfo> &gdRect, QVector<RectInfo> &ngRect,
                                   const QPoint basePt, cint minThre, cint maxThre, const qreal percent, const QSize minSize);
};

}
