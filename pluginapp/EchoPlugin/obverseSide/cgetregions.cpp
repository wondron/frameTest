#include "cgetregions.h"
#include "../commonUsage/algorithm.h"
#include "../commonUsage/commonDef.h"
#include "utils/XmlRead.h"
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
    double guassSigma = 2.3;
    double guassFactor = 1.6;
    int thresMax = -3;
    double tapeHole = 10;

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

    err = getTapeRegion(res, d->guassSigma, d->guassFactor, d->thresMax, d->tapeHole);
    CHECKERR(err);

    int size;
    err = getWeldRoi(res.BlackBangRegion, res.BlueRegion, res.WeldRoi, size, d->weldDilation, d->weldMinArea);
    CHECKERR(err);

    res.TapeNum = size;
    if(size != d->weldSize)
        return CError(REGIONNUM, QString("size of weld is not %1,result is %2").arg(d->weldSize).arg(size));
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

        READPAM(d->blkBangMaxThre, "blkBangMaxThre", res);

        READPAM(d->darkMaxR, "darkMaxR", res);
        READPAM(d->darkMaxG, "darkMaxG", res);
        READPAM(d->darkMaxB, "darkMaxB", res);

        READPAM(d->whiteMinR, "whiteMinR", res);
        READPAM(d->whiteMinG, "whiteMinG", res);
        READPAM(d->whiteMinB, "whiteMinB", res);

        READPAM(d->minhsvH, "minhsvH", res);
        READPAM(d->maxhsvH, "maxhsvH", res);
        READPAM(d->minhsvS, "minhsvS", res);
        READPAM(d->maxhsvV, "maxhsvV", res);

        READPAM(d->deepRedThr, "deepRedThr", res);

        READPAM(d->guassSigma, "guassSigma", res);
        READPAM(d->guassFactor, "guassFactor", res);
        READPAM(d->thresMax, "thresMax", res);
        READPAM(d->tapeHole, "tapeHole", res);

        READPAM(d->weldDilation, "weldDilation", res);
        READPAM(d->weldMinArea,  "weldMinArea", res);
        READPAM(d->weldSize,     "weldSize", res);

        return 0;

    } catch (...) {
        return CError(PAMREAD, "task: CuPosDetecte, pamRead failed!");
    }
}

CError CGetRegions::getTapeRegion(RegionS& res, double guassSigma, double guassFactor, cint thresMax, cint tapesize)
{
    try {
        HObject BImage = res.m_BImg;
        CHECKEMPIMG(BImage, "getTapeRegion::Bimg is empty");

        HObject  diffOfGauss, region, connectedRegions;
        HObject  slctRegion, slctRegion1, hUnion;
        HObject  RegionDilation, RegionFillUp, RegionErosion;
        HObject  hConnect, RegionClosing;

        HTuple  Area, Row, Column, Sorted;
        HTuple tapeHole = tapesize;

        DiffOfGauss(BImage, &diffOfGauss, guassSigma, guassFactor);
        Threshold(diffOfGauss, &region, -67, thresMax);
        Connection(region, &connectedRegions);

        SelectShape(connectedRegions, &slctRegion, ((HTuple("width").Append("height")).Append("holes_num")),
            "and", ((HTuple(0).Append(0)).Append(0)), ((3*tapeHole).TupleConcat(3*tapeHole)).TupleConcat(0));

        SelectShape(slctRegion, &slctRegion1, "area", "and", 3*tapeHole, 40*tapeHole);
        SelectShape(slctRegion1, &slctRegion1, "compactness", "and", 0, 2);

        Union1(slctRegion1, &hUnion);
        DilationRectangle1(hUnion, &RegionDilation, tapeHole, tapeHole);
        FillUp(RegionDilation, &RegionFillUp);
        ErosionRectangle1(RegionFillUp, &RegionErosion, tapeHole, tapeHole);
        Connection(RegionErosion, &hConnect);
        AreaCenter(hConnect, &Area, &Row, &Column);
        TupleSort(Area, &Sorted);

        SelectShape(hConnect, &slctRegion1, "area", "and",
                    HTuple(Sorted[(Sorted.TupleLength())-4]),
                    HTuple(Sorted[(Sorted.TupleLength())-1]));

        ClosingCircle(slctRegion1, &RegionClosing, 5*tapeHole);
        ErosionCircle(RegionClosing, &res.TapeRegion, 3.5);

        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "CGetRegions::getTapeRegion unexception happen!");
    }
}

CError CGetRegions::getWeldRoi(const HObject &blackBangRegion, const HObject &blueRegion, HObject &weldRoi, int& weldRegSize, cint dilationSize, cint minArea)
{
    try {
        HObject  rectRoi, dilationReg, interRegion;
        HObject  ho_connectedRegions, slctReg;

        CHECKEMPIMG(blueRegion, "getWeldRoi::blueRegion is empty");
        CHECKEMPIMG(blackBangRegion, "getWeldRoi::blackBangRegion is empty");

        HTuple num;
        GenRectangle1(&rectRoi, 0, 0, 1, dilationSize);
        Dilation1(blackBangRegion, rectRoi, &dilationReg, 1);
        Intersection(dilationReg, blueRegion, &interRegion);
        DilationRectangle1(interRegion, &interRegion, 30, 10);
        Connection(interRegion, &ho_connectedRegions);
        SelectShape(ho_connectedRegions, &slctReg, "area", "and", minArea, 9999999999);
        CHECKEMPIMG(slctReg, "getWeldRoi::slctReg is empty");

        HTuple nums, rows, cols;
        AreaCenter(slctReg, &nums, &rows, &cols);
        qDebug() << "cols num:"<< nums.Length();

        for(int i = 0; i < nums.Length(); i++)
            qDebug()<<"cols:" << nums[i].D();

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
        HObject  ho_RegionOpening, ho_connectedRegions, ho_slctRegion;
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

        Connection(ho_RegionOpening, &ho_connectedRegions);
        SelectShape(ho_connectedRegions, &ho_slctRegion, "area", "and", 150000, 9999999999);
        SelectShape(ho_connectedRegions, &ho_SelectedRegions2, (HTuple("area").Append("rectangularity")),
            "and", (HTuple(80000).Append(0.8)), (HTuple(1e+016).Append(1.11009)));

        Union2(ho_slctRegion, ho_SelectedRegions2, &resImg);
        Union1(resImg, &resImg);
        CHECKEMPIMG(resImg, "getWhiteRrgion func: resImg is empty");

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
        HObject  ho_HDarkRegion, ho_hUnion, ho_RegionUnion1;

        Threshold(hsvV, &ho_VDarKRegion, 0, maxV);

        Threshold(hsvS, &ho_SLightRegion, minS, 255);

        Threshold(hsvH, &ho_HLightRegion, minH, 255);

        Threshold(hsvH, &ho_HDarkRegion, 0, maxH);

        Union2(ho_HLightRegion, ho_HDarkRegion, &ho_hUnion);
        Union2(ho_hUnion, ho_VDarKRegion, &ho_RegionUnion1);
        Difference(ho_SLightRegion, ho_RegionUnion1, &ho_RegionUnion1);

        FillUpShape(ho_RegionUnion1, &ho_RegionUnion1, "area", 1, 99999);
        Connection(ho_RegionUnion1, &ho_RegionUnion1);

        SelectShape(ho_RegionUnion1, &ho_RegionUnion1, "area", "and", 10000, 99999999999);
        Union1(ho_RegionUnion1, &hsvRoi);
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
        HObject  ho_connectedRegions, ho_slctRegion;
        HTuple  hv_Area, hv_Row, hv_Column, hv_Sorted;

        Difference(hsVRoi, darkRegion, &ho_RegionDifference);
        Intersection(ho_RegionDifference, deepRedRegion, &ho_RegionIntersection2);
        Connection(ho_RegionIntersection2, &ho_connectedRegions);
        AreaCenter(ho_connectedRegions, &hv_Area, &hv_Row, &hv_Column);
        TupleSort(hv_Area, &hv_Sorted);
        SelectShape(ho_connectedRegions, &ho_slctRegion, "area", "and", HTuple(hv_Sorted[(hv_Sorted.TupleLength())-3]), HTuple(hv_Sorted[(hv_Sorted.TupleLength())-1])+1);
        //FillUp(ho_slctRegion, &blueRegion);
        SelectShape(ho_slctRegion, &ho_slctRegion, "area", "and", 200000, 9999999999);
        FillUpShape(ho_slctRegion, &blueRegion, "area", 1, 999999);

        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "CGetRegions::getBlueRegion unexception happen!");
    }
}
}
