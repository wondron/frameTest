#include "ccheckhanreg.h"
#include "qdebug.h"
#include "../commonUsage/commonDef.h"
#include "utils/XmlRead.h"

namespace CSHDetect {

class CCheckHanRegPrivate
{
public:
    CCheckHanRegPrivate() {}

    int controlNum = 4;

    int dynThreKernel = 15;
    int dynThreValu = 15;

    int hanHoleSize = 15;
    int maxHanArea = 180;
    int minHanRegionSize = 15;
};

CCheckHanReg::CCheckHanReg():
    d(new CCheckHanRegPrivate())
{

}

CError CCheckHanReg::detect(const HObject& obj, ReverRegionS& res, QList<QRect> &rects)
{
    HObject hanRegion;
    CError err = getHanRegion(obj, res.midRegion, res.hanregion,
                              d->hanHoleSize, d->dynThreValu, d->dynThreKernel,
                              d->maxHanArea, d->minHanRegionSize, rects);
    CHECKERR(err);

    if (rects.size() < d->controlNum)
        return CError(HANSIZE, QString("CCheckHanReg size is not %1").arg(d->controlNum));

    return 0;
}

CError CCheckHanReg::pamRead(const char* xmlfilePath)
{
    XmlRead xmlRead;

    try {
        std::map<std::string, xmlInfo> res = xmlRead.parseXML(xmlfilePath, taskName.toLocal8Bit().data());

        READPAM(d->controlNum, "controlNum", res);

        READPAM(d->dynThreValu, "dynThreValu", res);
        READPAM(d->dynThreKernel, "dynThreKernel", res);

        READPAM(d->hanHoleSize, "hanHoleSize", res);
        READPAM(d->maxHanArea, "maxHanArea", res);
        READPAM(d->minHanRegionSize, "minHanRegionSize", res);

        return 0;

    } catch (...) {
        return CError(PAMREAD, "task: CCheckHanReg, pamRead failed!");
    }
}

CError CCheckHanReg::getHanRegion(const HObject& dst, const HObject& midRegion, HObject& hanRegion,
                                  int hanHoleSize, int dynThreValu, int dynKernel, int maxHanArea, int minHanRegionSize, QList<QRect> &rects)
{
    try {
        CHECKEMPIMG(dst, "getHanRegion::Image is empty");
        CHECKEMPIMG(midRegion, "getHanRegion::midRegion is empty");
        CHECKTHREVALUE(dynThreValu, "getHanRegion::dynThreValu out of range");

        HObject  ImageReduced, ImageMean, RegionDynThresh;
        HObject  ConnectedRegions1, hanRegions, RegionUnion;
        HObject  ConnectedRegions2, SelectedRegions;

        HTuple tupTiao, tupMin, tupMax;
        HTuple holeSize;

        ReduceDomain(dst, midRegion, &ImageReduced);
        RegionDynThresh = Algorithm::dynThre(ImageReduced, dynThreValu, dynKernel);
        Connection(RegionDynThresh, &ConnectedRegions1);

        SelectShape(ConnectedRegions1, &hanRegions,
                    ((HTuple("area").Append("width")).Append("height")),
                    "and",
                    HTuple(hanHoleSize).TupleConcat((HTuple(0).Append(0))),
                    (HTuple(maxHanArea).TupleConcat(hanHoleSize)).TupleConcat(hanHoleSize));
        CHECKEMPIMG(hanRegions, "getHanRegion::hanRegions is empty");

        Union1(hanRegions, &RegionUnion);
        DilationCircle(RegionUnion, &hanRegions, hanHoleSize / 2);
        Connection(hanRegions, &ConnectedRegions2);
        SelectShape(ConnectedRegions2, &hanRegion, "area", "and", minHanRegionSize * maxHanArea, 99999);
        CountObj(hanRegion, &holeSize);

        HTuple row1, row2, col1, col2;
        SmallestRectangle1(hanRegion, &row1, &col1, &row2, &col2);

        rects.clear();
        for(int i = 0; i < row1.Length(); i++){
            QRect rect(QPoint(col1[i].D(), row1[i].D()), QPoint(col2[i].D(),row2[i].D()));
            rects.push_back(rect);
        }

        return 0;

    } catch (...) {
        return CError(PAMREAD, "CCheckHanReg::getHanRegion crashed!");
    }
}




}
