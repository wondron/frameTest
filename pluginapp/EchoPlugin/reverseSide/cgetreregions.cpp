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
    int batteryDire = 0;

    //获取电池区域
    int batGridW = 50;
    int batGridH = 50;
    int batSlctNum = 2;
    int batMinThre = 240;
    int batEroValue = 0;

    //获取中间区域
    int midMaxThre = 240;
    int midDilaWid = 1400;
    int midEroValue= 10;

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
        res.batDire = d->batteryDire;

        err = getBatteryRegion(obj, res.batteryRegion, d->batGridW, d->batGridH, d->batMinThre, d->batEroValue, d->batSlctNum);
        CHECKERR(err);

        err = getMidRegion(obj, res.batteryRegion, res.midRegion, d->batteryDire, d->midMaxThre, d->midDilaWid, d->midEroValue);
        CHECKERR(err);

//        int num = 0;
//        err = getBlueTapeNum(res.midRegion, res.blueTapesReg, d->higDilation, d->widDilation, num);
//        CHECKERR(err);
//        if (num != d->tapeNum)
//            return CError(NG, QString("tap num is not %1 : %2").arg(d->tapeNum).arg(num));

//        err = getDoubleTapeRoi(res.blueTapesReg, res.dblTapeReg, d->tapeDire, d->erosionSize);
//        CHECKERR(err);
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

        READPAM(d->batMinThre, "batMinThre", res);
        READPAM(d->batEroValue, "batEroValue", res);
        READPAM(d->midMaxThre, "midMaxThre", res);
        READPAM(d->higDilation, "higDilation", res);
        READPAM(d->widDilation, "widDilation", res);
        READPAM(d->tapeDire, "tapeDire", res);
        READPAM(d->erosionSize, "erosionSize", res);

        return 0;

    } catch (...) {
        return CError(PAMREAD, "task: CGetReRegions, pamRead failed!");
    }
}

CSHDetect::CError CGetReRegions::getBatteryRegion(const HalconCpp::HObject& img, HalconCpp::HObject& batteryRegion, cint gridW, cint gridH, cint batMinThre, cint midEroValue, cint batSlctNum)
{
    try {
        CError err = Algorithm::useGridGetRegion(img, batteryRegion, gridW, gridH, batMinThre, midEroValue, batSlctNum);
        if(err.isWrong()) return err;
        return 0;
    }  catch (...) {
        qDebug() << "CGetReRegions::getBatteryRegion crashed!";
        return CError(UNEXCEPTION, "CGetReRegions::getBatteryRegion crashed");
    }
}

CError CGetReRegions::getMidRegion(const HObject& img, const HObject& batRegion, HObject& midRegion, cint batteryDire, cint midMaxThre, cint midDilaWid, cint eroVal)
{
    try {

        CHECKEMPIMG(img, "getMidRegion input is empty");
        CHECKEMPIMG(batRegion, "batRegion input is empty");
        CHECKTHREVALUE(midMaxThre, "getMidRegion midMaxThre is out of range");
        CHECKTHREVALUE(eroVal, "getMidRegion eroVal is out of range");

        HObject  ConnectedRegions, RegionLines;
        HObject  RegionDilation, ImageReduced, Region, ConnectedRegions1;

        HTuple  Area, Row, Column;
        Connection(batRegion, &ConnectedRegions);
        AreaCenter(ConnectedRegions, &Area, &Row, &Column);
        GenRegionLine(&RegionLines, HTuple(Row[0]), HTuple(Column[0]), HTuple(Row[1]), HTuple(Column[1]));
        int dilaW, dilaH;
        if(batteryDire == 0){
            dilaW = midDilaWid;
            dilaH = 1;
        }else{
            dilaW = 1;
            dilaH = midDilaWid;
        }

        DilationRectangle1(RegionLines, &RegionDilation, dilaW, dilaH);
        ReduceDomain(img, RegionDilation, &ImageReduced);
        Threshold(ImageReduced, &Region, 0, midMaxThre);
        FillUp(Region, &midRegion);

        ErosionCircle(midRegion, &midRegion, eroVal);
        Connection(midRegion, &ConnectedRegions1);
        SelectShapeStd(ConnectedRegions1, &midRegion, "max_area", 70);
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


