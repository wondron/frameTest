#pragma once
#include "../commonUsage/cerror.h"
#include "../commonUsage/comStruct.h"

namespace CSHDetect {

#ifndef CONSTIMG
    #define CONSTIMG const HalconCpp::HObject&
#endif

typedef const int cint ;

class CGetRegionsPrivate;
class CGetRegions
{
public:
    CGetRegions();

    /**
     * @brief detect: 程序外部使用接口，获取各部分的参数。
     */
    CError detect(CONSTIMG obj, RegionS& res);

    /**
     * @brief transform: 转换图像格式
     */
    CError transformImg(CONSTIMG obj, RegionS& res);

    /**
     * @brief pamRead: 读取检测参数
     * @return
     */
    CError pamRead(const char *xmlfilePath);

public:
    /**
     * @brief getBlackBangRoi：中间固定棒
     */
    CError getBlackBangRoi(CONSTIMG hsvVImg, HalconCpp::HObject& darkBangRoi, cint maxThre = 20);

    /**
     * @brief getDarkRegion: 获取图像的黑色区域
     * @return
     */
    CError getDarkRegion(CONSTIMG RImg, CONSTIMG GImg, CONSTIMG BImg, HalconCpp::HObject& resImg,
                         cint maxR = 30, cint maxG = 30, cint maxB = 30);
    /**
     * @brief getWhiteRegion：获取图像的白色区域
     */
    CError getWhiteRegion(CONSTIMG RImg, CONSTIMG GImg, CONSTIMG BImg,HalconCpp::HObject& resImg,
                          cint minR = 200, cint minG = 200, cint minB = 200);

    /**
     * @brief getWhiteRegion：获取图像的HsvRoi
     * 就是大致的蓝胶区域
     */
    CError getHsvRoi(CONSTIMG hsvH, CONSTIMG hsvS, CONSTIMG hsvV, HalconCpp::HObject &hsvRoi,
                     cint minH = 200, cint maxH = 50, cint minS = 200, cint maxV = 60);

    /**
     * @brief getDeepRedRegion: 也是求大致的蓝色区域
     */
    CError getDeepRedRegion(CONSTIMG RImg, HalconCpp::HObject &redRegion, cint maxRed = 9);

    /**
     * @brief getBlueRegion : 最终的胶带区域
     */
    CError getBlueRegion(CONSTIMG hsVRoi, CONSTIMG darkRegion, CONSTIMG deepRedRegion, HalconCpp::HObject& blueRegion);

    /**
     * @brief getTapeRegion: 底部的特氟龙胶带区域
     */
    CError getTapeRegion(RegionS& res, double guassSigma = 2.3, double guassFactor = 1.6, cint thresMax = -3, cint tapeHole = 10);

    /**
     * @brief getWeldRoi: 焊印Roi
     */
    CError getWeldRoi(CONSTIMG blackBangRegion, CONSTIMG blueRegion, HalconCpp::HObject &weldRoi, int& weldRegSize,
                      cint dilationSize = 3100, cint minArea = 100000);
private:
    CGetRegionsPrivate* d;
    QString taskName = "GetRegions";
};

}
