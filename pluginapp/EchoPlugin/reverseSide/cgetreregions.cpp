#include "cgetreregions.h"
#include "qdebug.h"
#include "../commonUsage/commonDef.h"
#include "utils/XmlRead.h"

namespace CSHDetect {

class CGetReRegionsPrivate
{
public:
    CGetReRegionsPrivate() {}

    ~CGetReRegionsPrivate() {}
    //总体
    int tapeNum = 4;

    //获取电池区域
    int minThre = 240;
    int eroValue = 120;

    //获取中间区域
    int maxThre = 240;

    //获取胶带的边缘区域
    int higDilation = 500;
    int widDilation = 150;

    //获取胶带连接处区域
    int tapeDire = UPDOWN;
    int erosionSize = 10;
};

CGetReRegions::CGetReRegions():
    d(new CGetReRegionsPrivate())
{

}

CError CGetReRegions::detect(const HObject& obj, ReverRegionS& res)
{
    try {
        CHECKEMPIMG(obj, "getBatteryRegion::img is empty");

        CError err;
        HTuple width, heigh;
        GetImageSize(obj, &width, &heigh);
        res.width = width.D();
        res.height = heigh.D();

        err = getBatteryRegion(obj, res.batteryRegion, d->minThre, d->eroValue);
        CHECKERR(err);

        err = getMidRegion(obj, res.batteryRegion, res.midRegion, d->maxThre);
        CHECKERR(err);

        int num = 0;
        err = getBlueTapeNum(res.midRegion, res.blueTapesReg, d->higDilation, d->widDilation, num);
        CHECKERR(err);
        if (num != d->tapeNum)
            return CError(NG, QString("tap num is not %1 : %2").arg(d->tapeNum).arg(num));

        err = getDoubleTapeRoi(res.blueTapesReg, res.dblTapeReg, d->tapeDire, d->erosionSize);
        CHECKERR(err);
        return 0;

    } catch (...) {
        return CError(PAMREAD, "CGetReRegions::detect crash!");
    }
}

CError CGetReRegions::pamRead(const char* xmlfilePath)
{
    XmlRead xmlRead;

    try {
        std::map<std::string, xmlInfo> res = xmlRead.parseXML(xmlfilePath, taskName.toLocal8Bit().data());

        READPAM(d->minThre, "minThre", res);
        READPAM(d->eroValue, "eroValue", res);
        READPAM(d->maxThre, "maxThre", res);
        READPAM(d->higDilation, "higDilation", res);
        READPAM(d->widDilation, "widDilation", res);
        READPAM(d->tapeDire, "tapeDire", res);
        READPAM(d->erosionSize, "erosionSize", res);

        return 0;

    } catch (...) {
        return CError(PAMREAD, "task: CGetReRegions, pamRead failed!");
    }
}

CSHDetect::CError CGetReRegions::getBatteryRegion(const HalconCpp::HObject& img, HalconCpp::HObject& batteryRegion, int minThre, int eroValue)
{
    try {
        HObject  Region, eroRegion, connectReg;
        HObject  selected, unionReg;

        HTuple  areaTup, rowTup, colTup, sortTuple;

        CHECKEMPIMG(img, "getBatteryRegion::img is empty");
        CHECKTHREVALUE(minThre, "getBlackBangRoi::maxThre out of range");

        Threshold(img, &Region, minThre, 255);
        ErosionCircle(Region, &eroRegion, eroValue);
        Connection(eroRegion, &connectReg);
        AreaCenter(connectReg, &areaTup, &rowTup, &colTup);

        CHECKEMPTUPLE(areaTup, "getBatteryRegion::areaTup is empty");
        TupleSort(areaTup, &sortTuple);
        int lenth = areaTup.TupleLength();
        SelectShape(connectReg, &selected, "area", "and", sortTuple[lenth - 2],  sortTuple[lenth - 1] + 10);
        Union1(selected, &unionReg);
        ShapeTrans(unionReg, &batteryRegion, "convex");

        return 0;
    }  catch (...) {
        qDebug() << "CGetReRegions::getBatteryRegion crashed!";
        return CError(UNEXCEPTION, "CGetReRegions::getBatteryRegion crashed");
    }
}

CError CGetReRegions::getMidRegion(const HObject& img, const HObject& batRegion, HObject& midRegion, int maxThre)
{
    try {
        HObject  imgReduce, Region, fillupReg;
        HObject  connecReg;

        CHECKEMPIMG(img, "getBatteryRegion::img is empty");
        CHECKEMPIMG(batRegion, "getMidRegion::batRegion is empty");
        CHECKTHREVALUE(maxThre, "getBlackBangRoi::maxThre out of range");

        ReduceDomain(img, batRegion, &imgReduce);
        Threshold(imgReduce, &Region, 0, maxThre);
        FillUp(Region, &fillupReg);
        Connection(fillupReg, &connecReg);
        CHECKEMPIMG(connecReg, "getBatteryRegion::connecReg is empty");

        SelectShapeStd(connecReg, &midRegion, "max_area", 70);
        return 0;

    }  catch (...) {
        qDebug() << "CGetReRegions::getBatteryRegion crashed!";
        return CError(UNEXCEPTION, "CGetReRegions::getMidRegion crashed");
    }
}

CError CGetReRegions::getBlueTapeNum(const HObject& midRegion, HObject& blueTapesReg, int higDilation, int widDilation, int& num)
{
    try {
        HObject  RegionTrans1, Rectangle, RegionDilation;
        HObject  RegionDifference;
        HTuple  area, col, row;

        CHECKEMPIMG(midRegion, "getBatteryRegion::midRegion is empty");

        ShapeTrans(midRegion, &RegionTrans1, "inner_rectangle1");
        GenRectangle1(&Rectangle, 0, 0, higDilation, widDilation);
        Dilation1(RegionTrans1, Rectangle, &RegionDilation, 1);
        Difference(midRegion, RegionDilation, &RegionDifference);
        CHECKEMPIMG(RegionDifference, "getBatteryRegion::RegionDifference is empty");

        Connection(RegionDifference, &blueTapesReg);
        AreaCenter(blueTapesReg, &area, &row, &col);
        num = area.Length();

        return 0;
    }  catch (...) {
        qDebug() << "CGetReRegions::getBlueNum crashed!";
        return CError(UNEXCEPTION, "getBlueNum::getBlueTapeNum crashed");
    }
}

CError CGetReRegions::getDoubleTapeRoi(const HObject& blueTapesReg, HObject& dblTapeReg, int direct, int eroSize)
{
    try {
        HObject  slctReg, slctReg1, RegionUnion, RegionUnion1, RegionTrans, RegionTrans1;

        HTuple  Area, Row, Col, rowMean, colMean;
        CHECKEMPIMG(blueTapesReg, "getDoubleTapeRoi::tapesReg is empty");

        AreaCenter(blueTapesReg, &Area, &Row, &Col);
        TupleMean(Row, &rowMean);
        TupleMean(Col, &colMean);

        if (direct == UPDOWN) {
            SelectShape(blueTapesReg, &slctReg, "row", "and", 0, rowMean);
            SelectShape(blueTapesReg, &slctReg1, "row", "and", rowMean, 99999);
        } else {
            SelectShape(blueTapesReg, &slctReg, "column", "and", 0, colMean);
            SelectShape(blueTapesReg, &slctReg1, "column", "and", colMean, 99999);
        }

        CHECKEMPIMG(slctReg, "getDoubleTapeRoi::slctReg is empty");
        CHECKEMPIMG(slctReg1, "getDoubleTapeRoi::slctReg is empty");

        Union1(slctReg, &RegionUnion);
        Union1(slctReg1, &RegionUnion1);
        ShapeTrans(RegionUnion, &RegionTrans, "convex");
        ShapeTrans(RegionUnion1, &RegionTrans1, "convex");

        Union2(RegionTrans, RegionTrans1, &RegionTrans);
        ErosionCircle(RegionTrans, &dblTapeReg, eroSize);
        return 0;

    }  catch (...) {
        qDebug() << "CGetReRegions::getDoubleTapeRoi crashed!";
        return CError(UNEXCEPTION, "CGetReRegions::getDoubleTapeRoi crashed");
    }
}

}


