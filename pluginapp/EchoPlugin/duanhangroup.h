#pragma once
#include "halconcpp/HalconCpp.h"
#include "halconcpp/HDeformableModel.h"

using namespace HalconCpp;

class duanhanGroupPrivate;
class duanhanGroup
{
public:
    duanhanGroup();

    /**
     * @brief detect:总检测流程
     * @return false:Ng
     */
    bool detect();

    /**
     * @brief getRegion: 获取粗糙面区域(可能为焊条的区域)。
     * @param img： 检测图片
     * @param obj： 筛选得到的区域
     * @return true：不存在区域， false：存在区域
     */
    bool getRegion(HObject img, HObject &obj, HObject &roiImage);

    /**
     * @brief getPoints :使用measurepos获取焊条的边缘。
     * @param roiImg
     * @param slctRegion
     * @return
     */
    bool getPoints(HObject roiImg, HObject slctRegion);


public:
     HObject  ho_Image;
     HObject  ho_slctRegion;
     HObject  ho_ImageReduced;

     HTuple  hv_Width, hv_Height;
     HTuple  hv_Area, hv_Row, hv_Column; //区域中心及面积
     HTuple  hv_MeasureHandle;

public:
     HTuple smallR;
     HTuple bigR;

private:
    duanhanGroupPrivate *d;
};

