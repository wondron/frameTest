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
 * 私有类，用来保存检测参数。
 */
class CGetReRegionsPrivate;

/**
 * @brief CGetReRegions ： 对图像的各个部分进行分割，以便于检测算法的进行，起到节省算法损耗的作用。
 */
class CGetReRegions
{
public:
    CGetReRegions();

    ~CGetReRegions();

    /**
     * @brief detect: 程序外部使用接口，获取各部分的参数。
     */
    CError detect(CONSTIMG obj, ReverRegionS& res);

    /**
     * @brief pamRead: 读取检测参数
     * @return
     */
    CError pamRead(const char* xmlfilePath);

public:
    /**
     * @brief getBatteryRegion ：获取白色的电池区域部分，不用太准确，只要能涵盖中间极耳区域就行。
     * @param img：检测原图。
     * @param batteryRegion：求得的电池区域
     * @param minThre：二值分割的最小阈值
     * @param eroValue：腐蚀量
     */
    CError getBatteryRegion (CONSTIMG img, RHOBJ batteryRegion, int minThre, int eroValue);

    /**
     * @brief getMidRegion: 求中间的极耳区域，包括中间的蓝胶区域。
     * @param img： 检测原图
     * @param batRegion：电池区域
     * @param midRegion：求得的中间区域
     * @param maxThre：中间区域最大的阈值。
     */
    CError getMidRegion(CONSTIMG img, CONSTIMG batRegion, RHOBJ midRegion, int maxThre);

    /**
     * @brief getBlueTapeNum ：获取胶带贴的数量
     * @param midRegion：中间区域
     * @param blueTapesReg：去除中间极耳区域后的蓝胶区域，不是很全。
     * @param higDilation：中间变量经形状后生成矩形，对其高度的膨胀量。
     * @param widDilation：中间变量经形状后生成矩形，对其宽度的膨胀量。
     * @param num：求得的蓝胶数量
     */
    CError getBlueTapeNum (CONSTIMG midRegion, RHOBJ blueTapesReg, int higDilation, int widDilation, int& num);

    /**
     * @brief getDoubleTapeRoi : 求得蓝胶区域后，将左右对应的两个蓝胶区域融合，以便后续检测焊印有无
     * @param blueTapesReg：4个不完全的蓝胶区域
     * @param dblTapeReg：结果
     * @param direct：极耳方向： 0：UPDOWN， 1：LEFTRIGHT
     * @param eroSize：求得后对区域进行腐蚀的大小
     * @return
     */
    CError getDoubleTapeRoi(CONSTIMG blueTapesReg, RHOBJ dblTapeReg, int direct, int eroSize);
private:
    CGetReRegionsPrivate* d;
    QString taskName = "GetReRegions";
};

}




