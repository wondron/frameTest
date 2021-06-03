#include "cgetregions.h"
#include "../commonUsage/algorithm.h"
#include "../commonUsage/commonDef.h"
#include "XmlRead.h"
#include "QDebug"

namespace CSHDetect{

class CGetRegionsPrivate
{
public:
    CGetRegionsPrivate(CGetRegions *parent = nullptr)
    {
        p = parent;
    }
    ~CGetRegionsPrivate(){}

    int m_width;
    int m_height;
    HObject m_oriImg;

    //检测参数
    int blkBangMaxThre = 20;

    //黑色区域
    int darkMaxR = 30;
    int darkMaxG = 30;
    int darkMaxB = 30;

    //白色区域
    int whiteMinR = 200;
    int whiteMinG = 200;
    int whiteMinB = 200;

    //hsv区域
    int minhsvH = 200;
    int maxhsvH = 50;
    int minhsvS = 200;
    int maxhsvV = 60;

    //deepRed
    int deepRedThr = 9;

    //底面胶带区域
    int tapeholeSize = 10;
    double slctRange = 0.15;

    //焊印Roi区域
    int weldDilation = 3100;
    int weldMinArea = 100000;
    int weldSize = 2;

private:
    CGetRegions* p;
};

CGetRegions::CGetRegions():d(new CGetRegionsPrivate(this))
{

}

CError CGetRegions::detect(const HObject &obj, RegionS &res)
{
    CHECKEMPIMG(obj, "CGetRegions::detect obj is empty");

    CError err;
    d->m_oriImg = obj;
    res.m_oriImg = obj;
    err = transformImg(res.m_oriImg, res);
    CHECKERR(err);

    err = getBlackBangRoi(res.m_hsvVImg, res.BlackBangRegion, d->blkBangMaxThre);
    CHECKERR(err);

    err = getDarkRegion(res.m_RImg, res.m_GImg, res.m_BImg, res.DarkRegion, d->darkMaxR, d->darkMaxG, d->darkMaxB);
    CHECKERR(err);

    err = getWhiteRegion(res.m_RImg, res.m_GImg, res.m_BImg, res.WhiteRegion, d->whiteMinR, d->whiteMinG, d->whiteMinB);
    CHECKERR(err);

    err = getHsvRoi(res.m_hsvHImg, res.m_hsvSImg, res.m_hsvVImg, res.HSVRoi, d->minhsvH, d->maxhsvH, d->minhsvS, d->maxhsvH);
    CHECKERR(err);

    err = getDeepRedRegion(res.m_RImg, res.DeepRedRegion, d->deepRedThr);
    CHECKERR(err);

    err = getBlueRegion(res.HSVRoi, res.DarkRegion, res.DeepRedRegion, res.BlueRegion);
    CHECKERR(err);

    err = getTapeRegion(res.m_RImg, res.WhiteRegion, res.TapeRegion, d->tapeholeSize, d->slctRange);
    CHECKERR(err);

    int size;
    err = getWeldRoi(res.BlackBangRegion, res.BlueRegion, res.WeldRoi, size, d->weldDilation, d->weldMinArea);
    CHECKERR(err);

    if(size != d->weldSize)
        return CError(REGIONNUM, QString("size of weld is not %1").arg(d->weldSize));
    return 0;
}

CError CGetRegions::transformImg(CONSTIMG obj, RegionS& res)
{
    try {
        CHECKEMPIMG(obj, "CGetRegions::transformImg oriImg is empty!");
        res.m_oriImg = obj;

        HTuple Width, Height;
        Decompose3(obj, &res.m_RImg, &res.m_GImg, &res.m_BImg);

        TransFromRgb(res.m_RImg, res.m_GImg, res.m_BImg, &res.m_hsvHImg, &res.m_hsvSImg, &res.m_hsvVImg, "hsv");

        GetImageSize(res.m_oriImg, &Width, &Height);
        CHECKEMPTUPLE(Width, "get image size failed!");

        res.width = Width.I();
        res.height = Height.I();
        d->m_width = Width.I();
        d->m_height = Height.I();
        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "CGetRegions::transformImg unexception happen!");
    }
}

CError CGetRegions::pamRead(const char *xmlfilePath)
{
    XmlRead xmlRead;

    try {
        std::map<std::string, xmlInfo> res = xmlRead.parseXML(xmlfilePath, taskName.toLocal8Bit().data());

        auto iterMBB = res.find("blkBangMaxThre");
        if (iterMBB != res.end()) {
            d->blkBangMaxThre = iterMBB->second.value;
        }

        iterMBB = res.find("darkMaxR");
        if (iterMBB != res.end()) {
            d->darkMaxR = iterMBB->second.value;
        }

        iterMBB = res.find("darkMaxG");
        if (iterMBB != res.end()) {
            d->darkMaxG = iterMBB->second.value;
        }

        iterMBB = res.find("darkMaxB");
        if (iterMBB != res.end()) {
            d->darkMaxB = iterMBB->second.value;
        }

        iterMBB = res.find("whiteMinR");
        if (iterMBB != res.end()) {
            d->whiteMinR = iterMBB->second.value;
        }

        iterMBB = res.find("whiteMinG");
        if (iterMBB != res.end()) {
            d->whiteMinG = iterMBB->second.value;
        }

        iterMBB = res.find("whiteMinB");
        if (iterMBB != res.end()) {
            d->whiteMinG = iterMBB->second.value;
        }

        iterMBB = res.find("minhsvH");
        if (iterMBB != res.end()) {
            d->minhsvH = iterMBB->second.value;
        }

        iterMBB = res.find("maxhsvH");
        if (iterMBB != res.end()) {
            d->maxhsvH = iterMBB->second.value;
        }

        iterMBB = res.find("minhsvS");
        if (iterMBB != res.end()) {
            d->minhsvS = iterMBB->second.value;
        }

        iterMBB = res.find("maxhsvV");
        if (iterMBB != res.end()) {
            d->maxhsvV = iterMBB->second.value;
        }

        iterMBB = res.find("deepRedThr");
        if (iterMBB != res.end()) {
            d->deepRedThr = iterMBB->second.value;
        }

        iterMBB = res.find("tapeholeSize");
        if (iterMBB != res.end()) {
            d->tapeholeSize = iterMBB->second.value;
        }

        iterMBB = res.find("slctRange");
        if (iterMBB != res.end()) {
            d->slctRange = iterMBB->second.value;
        }

        iterMBB = res.find("weldDilation");
        if (iterMBB != res.end()) {
            d->weldDilation = iterMBB->second.value;
        }

        iterMBB = res.find("weldMinArea");
        if (iterMBB != res.end()) {
            d->weldMinArea = iterMBB->second.value;
        }

        iterMBB = res.find("weldSize");
        if (iterMBB != res.end()) {
            d->weldSize = iterMBB->second.value;
        }

        return 0;

    } catch (...) {
        return CError(PAMREAD, "task: CuPosDetecte, pamRead failed!");
    }
}

CError CGetRegions::getTapeRegion(const HObject &Rimg, const HObject &whiteRegion, HObject &tapeRegion, int tapeHoleSize, double slctRange)
{
    try {
        CHECKEMPIMG(Rimg, "getTapeRegion::Rimg is empty");
        CHECKEMPIMG(whiteRegion, "getTapeRegion::whiteRegion is empty");

        HObject  ho_ImageMean, ho_RegionDynThresh, ho_Rectangle;
        HObject  ho_RegionDilation, ho_ConnectedRegions1, ho_SelectedRegions1;
        HObject  ho_RegionIntersection, ho_ConnectedRegions2, ho_SelectedRegions;
        HObject  ho_SelectedRegions3, ho_SelectedRegionsM, ho_SelectedRegionsM1;
        HObject  ho_SelectedRegionsM2, ho_SelectedRegionsM3, ho_RegionUnion1;
        HObject  ho_RegionDifference, ho_RegionDilation1, ho_RegionOpening;
        HObject  ho_RegionFillUp, ho_ConnectedRegions, ho_SelectedRegions2;

        HTuple  hv_Row, hv_Column, hv_Phi, hv_Length1;
        HTuple  hv_Length2, hv_Sorted, hv_Array, hv_Uniq, hv_ElementValue;
        HTuple  hv_Times, hv_Index, hv_Indices, hv_Length, hv_Sorted2;
        HTuple  hv_Max, hv_Max1, hv_Max2, hv_Max3, hv_Indices1;
        HTuple  hv_Indices2, hv_Indices3, hv_MaxElement, hv_MaxElement1;
        HTuple  hv_MaxElement2, hv_MaxElement3, hv_selectRange;
        HTuple  hv_Area, hv_Row1, hv_Column1, hv_Sorted1;

        //动态阈值求区域
        MeanImage(Rimg, &ho_ImageMean, tapeHoleSize*1.5, 1);
        DynThreshold(Rimg, ho_ImageMean, &ho_RegionDynThresh, 10, "light");
        GenRectangle1(&ho_Rectangle, 0, 0, tapeHoleSize, 1);
        Dilation1(ho_RegionDynThresh, ho_Rectangle, &ho_RegionDilation, 1);
        Connection(ho_RegionDilation, &ho_ConnectedRegions1);
        SelectShape(ho_ConnectedRegions1, &ho_SelectedRegions1, "height", "and", 13*tapeHoleSize, 5000000);
        CHECKEMPIMG(ho_SelectedRegions1, "getTapeRegion::ho_SelectedRegions1 is empty");

        Intersection(ho_RegionDynThresh, ho_SelectedRegions1, &ho_RegionIntersection);
        Connection(ho_RegionIntersection, &ho_ConnectedRegions2);
        SelectShape(ho_ConnectedRegions2, &ho_SelectedRegions, "area", "and", 80, 99999999);
        SelectShape(ho_SelectedRegions, &ho_SelectedRegions3, "rectangularity", "and", 0.1, 1);
        CHECKEMPIMG(ho_SelectedRegions3, "getTapeRegion::ho_SelectedRegions3 is empty");

        SmallestRectangle2(ho_SelectedRegions3, &hv_Row, &hv_Column, &hv_Phi, &hv_Length1, &hv_Length2);

        TupleSort(hv_Phi, &hv_Sorted);
        hv_Array = (hv_Sorted.TupleString(".1f")).TupleNumber();
        TupleUniq(hv_Array, &hv_Uniq);

        hv_ElementValue = HTuple();
        hv_Times = HTuple();

        HTuple end_val20 = (hv_Uniq.TupleLength())-1;
        HTuple step_val20 = 1;
        for (hv_Index=0; hv_Index.Continue(end_val20, step_val20); hv_Index += step_val20)
        {
          TupleFind(hv_Array, HTuple(hv_Uniq[hv_Index]), &hv_Indices);
          TupleLength(hv_Indices, &hv_Length);
          hv_ElementValue = hv_ElementValue.TupleConcat(HTuple(hv_Uniq[hv_Index]));
          hv_Times = hv_Times.TupleConcat(hv_Length);
        }

        TupleSort(hv_Times, &hv_Sorted2);
        hv_Max =  ((const HTuple&)hv_Sorted2)[(hv_Sorted2.TupleLength())-1];
        hv_Max1 = ((const HTuple&)hv_Sorted2)[(hv_Sorted2.TupleLength())-2];
        hv_Max2 = ((const HTuple&)hv_Sorted2)[(hv_Sorted2.TupleLength())-3];
        hv_Max3 = ((const HTuple&)hv_Sorted2)[(hv_Sorted2.TupleLength())-4];

        TupleFind(hv_Times, hv_Max, &hv_Indices);
        TupleFind(hv_Times, hv_Max1, &hv_Indices1);
        TupleFind(hv_Times, hv_Max2, &hv_Indices2);
        TupleFind(hv_Times, hv_Max3, &hv_Indices3);

        hv_MaxElement  = HTuple(hv_ElementValue[hv_Indices]);
        hv_MaxElement1 = HTuple(hv_ElementValue[hv_Indices1]);
        hv_MaxElement2 = HTuple(hv_ElementValue[hv_Indices2]);
        hv_MaxElement3 = HTuple(hv_ElementValue[hv_Indices3]);

        hv_selectRange = slctRange;
        SelectShape(ho_SelectedRegions3, &ho_SelectedRegionsM,  "phi", "and", hv_MaxElement-hv_selectRange,  hv_MaxElement+hv_selectRange);
        SelectShape(ho_SelectedRegions3, &ho_SelectedRegionsM1, "phi", "and", hv_MaxElement1-hv_selectRange, hv_MaxElement1+hv_selectRange);
        SelectShape(ho_SelectedRegions3, &ho_SelectedRegionsM2, "phi", "and", hv_MaxElement2-hv_selectRange, hv_MaxElement2+hv_selectRange);
        SelectShape(ho_SelectedRegions3, &ho_SelectedRegionsM3, "phi", "and", hv_MaxElement3-hv_selectRange, hv_MaxElement3+hv_selectRange);

        Union1(ho_SelectedRegionsM, &ho_RegionUnion1);
        Union2(ho_RegionUnion1, ho_SelectedRegionsM1, &ho_RegionUnion1);
        Union2(ho_RegionUnion1, ho_SelectedRegionsM2, &ho_RegionUnion1);
        Union2(ho_RegionUnion1, ho_SelectedRegionsM3, &ho_RegionUnion1);
        CHECKEMPIMG(ho_RegionUnion1, "getTapeRegion::ho_RegionUnion1 is empty");

        Difference(ho_RegionUnion1, whiteRegion, &ho_RegionDifference);
        DilationRectangle1(ho_RegionDifference, &ho_RegionDilation1, tapeHoleSize, tapeHoleSize);
        OpeningCircle(ho_RegionDilation1, &ho_RegionOpening, 10);
        FillUp(ho_RegionOpening, &ho_RegionFillUp);
        Connection(ho_RegionFillUp, &ho_ConnectedRegions);
        AreaCenter(ho_ConnectedRegions, &hv_Area, &hv_Row1, &hv_Column1);
        TupleSort(hv_Area, &hv_Sorted1);
        SelectShape(ho_ConnectedRegions, &ho_SelectedRegions2, "area", "and", HTuple(hv_Sorted1[(hv_Sorted1.TupleLength())-4]),
            HTuple(hv_Sorted1[(hv_Sorted1.TupleLength())-1]));
        ErosionCircle(ho_SelectedRegions2, &tapeRegion, 5);

        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "CGetRegions::getBlackBangRoi unexception happen!");
    }
}

CError CGetRegions::getWeldRoi(const HObject &blackBangRegion, const HObject &blueRegion, HObject &weldRoi, int& weldRegSize, int dilationSize, int minArea)
{
    try {
        HObject  rectRoi, dilationReg, interRegion;
        HObject  ho_ConnectedRegions, slctReg;

        CHECKEMPIMG(blueRegion, "getWeldRoi::blueRegion is empty");
        CHECKEMPIMG(blackBangRegion, "getWeldRoi::blackBangRegion is empty");

        HTuple num;
        GenRectangle1(&rectRoi, 0, 0, 1, dilationSize);
        Dilation1(blackBangRegion, rectRoi, &dilationReg, 1);
        Intersection(dilationReg, blueRegion, &interRegion);
        Connection(interRegion, &ho_ConnectedRegions);
        SelectShape(ho_ConnectedRegions, &slctReg, "area", "and", minArea, 9999999999);
        CHECKEMPIMG(slctReg, "getWeldRoi::slctReg is empty");

        CountObj(slctReg, &num);
        weldRegSize = num.I();
        Union1(slctReg, &weldRoi);

        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "CGetRegions::getBlackBangRoi unexception happen!");
    }
}

CError CGetRegions::getBlackBangRoi(const HObject &hsvVImg, HObject &darkBangRoi, const int maxThre)
{
    try {
        HObject region, connectReg, selectReg;

        HObject maxSlctReg, openingReg, finalMaxReg;
        HTuple width, height;
        CError res;

        CHECKEMPIMG(hsvVImg, "getBlackBangRoi::hsvVImg is empty");
        CHECKTHREVALUE(maxThre, "getBlackBangRoi::maxThre out of range");

        GetImageSize(hsvVImg, &width, &height);

        d->m_width = width.I();
        d->m_height = height.I();

        Threshold(hsvVImg, &region, 0, maxThre);
        Connection(region, &connectReg);
        SelectShape(connectReg, &selectReg, (HTuple("column").Append("row")),
              "and", (width/3).TupleConcat(height/3), ((2*width)/3).TupleConcat((2*height)/3));

        CHECKEMPIMG(selectReg, "after select obj is empty");

        SelectShapeStd(selectReg, &maxSlctReg, "max_area", 70);
        FillUp(maxSlctReg, &maxSlctReg);

        OpeningCircle(maxSlctReg, &openingReg, 30);
        Connection(openingReg, &openingReg);

        SelectShapeStd(openingReg, &finalMaxReg, "max_area", 70);
        ShapeTrans(finalMaxReg, &darkBangRoi, "convex");

        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "CGetRegions::getBlackBangRoi unexception happen!");
    }
}

CError CGetRegions::getDarkRegion(const HObject &RImg, const HObject &GImg, const HObject &BImg, HObject &resImg, const int maxR, const int maxG, const int maxB)
{
    try {
        HObject  ho_Region, ho_Region1, ho_Region2, ho_RegionIntersection;

        CError res;

        CHECKEMPIMG(RImg, "getDarkRegion func: Rimg is empty");
        CHECKEMPIMG(GImg, "getDarkRegion func: Gimg is empty");
        CHECKEMPIMG(BImg, "getDarkRegion func: Bimg is empty");

        CHECKTHREVALUE(maxR, "getDarkRegion func: maxR out of range");
        CHECKTHREVALUE(maxG, "getDarkRegion func: maxG out of range");
        CHECKTHREVALUE(maxB, "getDarkRegion func: maxB out of range");

        Threshold(RImg, &ho_Region, 0, maxR);

        Threshold(GImg, &ho_Region1, 0, maxG);

        Threshold(BImg, &ho_Region2, 0, maxB);

        Intersection(ho_Region2, ho_Region1, &ho_RegionIntersection);
        Intersection(ho_RegionIntersection, ho_Region, &resImg);

        CHECKEMPIMG(resImg, "getDarkRegion func: final result is empty");
        return 0;

    }  catch (...) {
        return CError(UNEXCEPTION, "CGetRegions::getDarkRegion unexception happen!");
    }
}

CError CGetRegions::getWhiteRegion(const HObject &RImg, const HObject &GImg, const HObject &BImg, HObject &resImg, const int minR, const int minG, const int minB)
{
    try {
        HObject  ho_Region, ho_Region1, ho_Region2, ho_RegionIntersection;
        HObject  ho_RegionOpening, ho_ConnectedRegions, ho_SelectedRegions;
        HObject  ho_SelectedRegions2, ho_Rectangle;

        CError res;

        CHECKEMPIMG(RImg, "getDarkRegion func: Rimg is empty");
        CHECKEMPIMG(GImg, "getDarkRegion func: Gimg is empty");
        CHECKEMPIMG(BImg, "getDarkRegion func: Bimg is empty");

        CHECKTHREVALUE(minR, "getDarkRegion func: maxR out of range");
        CHECKTHREVALUE(minG, "getDarkRegion func: maxG out of range");
        CHECKTHREVALUE(minB, "getDarkRegion func: maxB out of range");

        Threshold(RImg, &ho_Region, minR, 255);

        Threshold(GImg, &ho_Region1, minG, 255);

        Threshold(BImg, &ho_Region2, minB, 255);

        Intersection(ho_Region2, ho_Region1, &ho_RegionIntersection);
        Intersection(ho_RegionIntersection, ho_Region, &ho_Region);
        OpeningCircle(ho_Region, &ho_RegionOpening, 20);
        CHECKEMPIMG(ho_RegionOpening, "getWhiteRrgion func: ho_RegionOpening is empty");

        Connection(ho_RegionOpening, &ho_ConnectedRegions);
        SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", 150000, 99999999);
        CHECKEMPIMG(ho_SelectedRegions, "getWhiteRrgion func: ho_SelectedRegions is empty");

        SelectShape(ho_SelectedRegions, &ho_SelectedRegions2, (HTuple("area").Append("rectangularity")),
            "and", (HTuple(80000).Append(0.8)), (HTuple(1e+016).Append(1.11009)));
        CHECKEMPIMG(ho_SelectedRegions2, "getWhiteRrgion func: ho_SelectedRegions2 is empty");

        Union2(ho_SelectedRegions, ho_SelectedRegions2, &resImg);
        Union1(resImg, &resImg);
        GenRectangle1(&ho_Rectangle, 0, 0, 20, 20);
        Dilation1(resImg, ho_Rectangle, &resImg, 1);
        return 0;

    }  catch (...) {
        return CError(UNEXCEPTION, "CGetRegions::getWhiteRrgion unexception happen!");
    }
}

CError CGetRegions::getHsvRoi(const HObject &hsvH, const HObject &hsvS, const HObject &hsvV, HObject &hsvRoi,
                              const int minH, const int maxH, const int minS, const int maxV)
{
    try {

        CHECKEMPIMG(hsvH, "getHsvRoi func: hsvH is empty");
        CHECKEMPIMG(hsvS, "getHsvRoi func: hsvS is empty");
        CHECKEMPIMG(hsvV, "getHsvRoi func: hsvV is empty");

        CHECKTHREVALUE(maxV, "getHsvRoi func: maxV out of range");
        CHECKTHREVALUE(minS, "getHsvRoi func: minS out of range");
        CHECKTHREVALUE(minH, "getHsvRoi func: minH out of range");
        CHECKTHREVALUE(maxH, "getHsvRoi func: maxH out of range");

        HObject  ho_VDarKRegion, ho_SLightRegion, ho_HLightRegion;
        HObject  ho_HDarkRegion, ho_RegionUnion, ho_RegionUnion1;

        Threshold(hsvV, &ho_VDarKRegion, 0, maxV);

        Threshold(hsvS, &ho_SLightRegion, minS, 255);

        Threshold(hsvH, &ho_HLightRegion, minH, 255);

        Threshold(hsvH, &ho_HDarkRegion, 0, maxH);

        Union2(ho_HLightRegion, ho_HDarkRegion, &ho_RegionUnion);
        Union2(ho_RegionUnion, ho_VDarKRegion, &ho_RegionUnion1);
        Difference(ho_SLightRegion, ho_RegionUnion1, &ho_RegionUnion1);
        FillUp(ho_RegionUnion1, &hsvRoi);
        return 0;

    }  catch (...) {
        return CError(UNEXCEPTION, "CGetRegions::getHsvRoi unexception happen!");
    }
}

CError CGetRegions::getDeepRedRegion(const HObject &RImg, HObject &redRegion, const int maxRed)
{
    try {
        CHECKEMPIMG(RImg, "getDeepRedRegion func: RImg is empty");
        CHECKTHREVALUE(maxRed, "getDeepRedRegion func: maxRed out of range");

        Threshold(RImg, &redRegion, 0, maxRed);
        CHECKEMPIMG(redRegion, "getDeepRedRegion func: redRegion is empty");
        return 0;

    }  catch (...) {
        return CError(UNEXCEPTION, "CGetRegions::getDeepRedRegion unexception happen!");
    }
}

CError CGetRegions::getBlueRegion(const HObject &hsVRoi, const HObject &darkRegion, const HObject &deepRedRegion, HObject &blueRegion)
{
    try {
        CHECKEMPIMG(hsVRoi, "getBlueRegion func: hsVRoi is empty");
        CHECKEMPIMG(darkRegion, "getBlueRegion func: darkRegion is empty");
        CHECKEMPIMG(deepRedRegion, "getBlueRegion func: deepRedRegion is empty");

        HObject  ho_RegionDifference, ho_RegionIntersection2;
        HObject  ho_ConnectedRegions, ho_SelectedRegions;
        HTuple  hv_Area, hv_Row, hv_Column, hv_Sorted;

        Difference(hsVRoi, darkRegion, &ho_RegionDifference);
        Intersection(ho_RegionDifference, deepRedRegion, &ho_RegionIntersection2);
        Connection(ho_RegionIntersection2, &ho_ConnectedRegions);
        AreaCenter(ho_ConnectedRegions, &hv_Area, &hv_Row, &hv_Column);
        TupleSort(hv_Area, &hv_Sorted);
        SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", HTuple(hv_Sorted[(hv_Sorted.TupleLength())-2]), HTuple(hv_Sorted[(hv_Sorted.TupleLength())-1])+1);
        FillUp(ho_SelectedRegions, &blueRegion);

        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "CGetRegions::getBlueRegion unexception happen!");
    }
}
}
