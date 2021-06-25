#include "cgetcustrappos.h"
#include "utils/XmlRead.h"
#include "QDebug"

namespace CSHDetect {

class CGetCuStrapPosPrivate
{
public:
    CGetCuStrapPosPrivate() {}

    int m_width;
    int m_height;

    int minThre = 20;
    int CuPos = 3;
    int strapDirection = 1;

    int numMinThre = 0;
    int numMaxThre = 150;
    double numAreaRatoi = 0.6;
    QSize regionSize = QSize(9999, 9999);
    QVector<RectInfo> rectList;
    RegionS m_region;
};

CGetCuStrapPos::CGetCuStrapPos(const int wid, const int height):
    d(new CGetCuStrapPosPrivate())
{
    d->m_width = wid;
    d->m_height = height;
}

CGetCuStrapPos::CGetCuStrapPos(const QString name):
    d(new CGetCuStrapPosPrivate())
{
    taskName = name;
}

CGetCuStrapPos::CGetCuStrapPos(const RegionS& region):
    d(new CGetCuStrapPosPrivate())
{
    d->m_region = region;
}

void CGetCuStrapPos::setImgSize(const int width, const int height)
{
    d->m_width = width;
    d->m_height = height;
}

void CGetCuStrapPos::setRegions(const RegionS& regions)
{
    d->m_region = regions;
}

CError CGetCuStrapPos::getMidRect(const RegionS& regions, HObject& midRegion, const int connectDirect)
{
    try {
        HObject white = regions.WhiteRegion;

        CHECKEMPIMG(white, "CGetCuStrapPos::getMidRect whiteRegion is empty");

        HObject hRect, hUpRect, hDnRect, hConnect, hUp, hDown, hResult;
        HTuple row, col, phi, len1, len2;
        HTuple areas, rows, cols, slctStd, slctStr, means;

        SmallestRectangle2(white, &row, &col, &phi, &len1, &len2);
        GenRectangle2(&hRect, row, col, phi, len1, len2);

        Connection(white, &hConnect);
        AreaCenter(hConnect, &areas, &rows, &cols);

        if (connectDirect == UPDOWN) {
            slctStr = "column";
            slctStd = cols;
        } else {
            slctStr = "row";
            slctStd = rows;
        }

        TupleMean(slctStd, &means);

        SelectShape(hConnect, &hUp, slctStr, "and", 0, means);
        Union1(hUp, &hUp);
        SmallestRectangle2(hUp, &row, &col, &phi, &len1, &len2);
        GenRectangle2(&hUpRect, row, col, phi, len1 + 4000, len2);

        SelectShape(hConnect, &hDown, slctStr, "and", means, 99999);
        Union1(hDown, &hDown);
        SmallestRectangle2(hDown, &row, &col, &phi, &len1, &len2);
        GenRectangle2(&hDnRect, row, col, phi, len1 + 4000, len2);

        Difference(hRect, hUpRect, &hResult);
        Difference(hResult, hDnRect, &midRegion);

        CHECKEMPIMG(white, "CGetCuStrapPos::getMidRect whiteRegion is empty");
        return 0;

    }  catch (...) {
        qDebug() << "CGetCuStrapPos::getMidRect crashed";
        return CError(UNEXCEPTION, "CGetCuStrapPos::getMidRect crashed");
    }
}

CError CGetCuStrapPos::getCuPos(const RegionS& regions, const HObject& midRegion, HObject& resCuRegion, int& CuPos, cint minThre, cint connectDirect)
{
    try {
        CHECKEMPIMG(midRegion, "CGetCuStrapPos::getMidRect midRegion is empty");
        CHECKEMPIMG(regions.TapeRegion, "CGetCuStrapPos::getMidRect TapeRegion is empty");
        CHECKEMPIMG(regions.DarkRegion, "CGetCuStrapPos::getMidRect DarkRegion is empty");
        CHECKEMPIMG(regions.HSVRoi, "CGetCuStrapPos::getMidRect HSVRoi is empty");
        CHECKEMPIMG(regions.m_RImg, "CGetCuStrapPos::getMidRect m_RImg is empty");
        CHECKEMPIMG(regions.m_GImg, "CGetCuStrapPos::getMidRect m_GImg is empty");

        CHECKTHREVALUE(minThre, "CGetCuStrapPos::getCuPos minthre is out of range");

        HObject hTestRegion, hSubImg, hReduce, hResRegion;
        HTuple area, col, row;

        Difference(midRegion, regions.HSVRoi, &hTestRegion);
        Difference(hTestRegion, regions.TapeRegion, &hTestRegion);
        Difference(hTestRegion, regions.DarkRegion, &hTestRegion);

        SubImage(regions.m_RImg, regions.m_GImg, &hSubImg, 1, 0);

        ReduceDomain(hSubImg, hTestRegion, &hReduce);
        Threshold(hReduce, &hResRegion, minThre, 255);
        CHECKEMPIMG(hResRegion, "hResRegion m_GImg is empty");

        Connection(hResRegion, &hResRegion);
        SelectShapeStd(hResRegion, &resCuRegion, "max_area", 70);
        AreaCenter(resCuRegion, &area, &row, &col);

        int STD, value;
        if (connectDirect == UPDOWN) {
            STD = regions.height / 2;
            value = row.D();
            CuPos = value > STD ? 1 : 0;
        } else {
            STD = regions.width / 2;
            value = col.D();
            CuPos = value > STD ? 3 : 2;
        }
        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "CGetCuStrapPos::getCuPos crashed");
    }
}

CError CGetCuStrapPos::checkTapeNum(const RegionS& region, const HObject& cuRegion, QVector<RectInfo> testInfo, QVector<QRect>& gdInfo, QVector<QRect>& ngInfo, cint minThre, cint maxThre, const qreal areaRatoi,  const QSize size)
{
    try {
        HObject Rimg = region.m_RImg;
        HObject hShow;

        CHECKEMPIMG(Rimg, "CGetCuStrapPos::checkTapeNum rimg empty");
        CHECKEMPIMG(cuRegion, "CGetCuStrapPos::checkTapeNum cuRegion empty");

        HTuple area, row, col;
        AreaCenter(cuRegion, &area, &row, &col);

        QPoint basePt(col.D(), row.D());
        QVector<RectInfo> gInfo, nInfo;
        CError err = Algorithm::detectRegionExit(Rimg, hShow, testInfo, gInfo, nInfo, basePt, minThre, maxThre, areaRatoi, size);
        CHECKERR(err);

        for (auto rect : gInfo) {
            QRect rectres = QRect(QPoint(rect.col - rect.len1, rect.row - rect.len2),
                                 QPoint(rect.col + rect.len1, rect.row + rect.len2));
            gdInfo.push_back(rectres);
        }

        for (auto rect : nInfo) {
            QRect rectres = QRect(QPoint(rect.col - rect.len1, rect.row - rect.len2),
                                 QPoint(rect.col + rect.len1, rect.row + rect.len2));
            ngInfo.push_back(rectres);
        }

        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "CGetCuStrapPos::checkTapeNum crashed");
    }
}

CError CGetCuStrapPos::detect(const RegionS& regions, QVector<QRect> &gdRect, QVector<QRect> &ngRect)
{
    try {
        CError err;
        HObject midRegion, cuRegion;
        int CuPos;
        err = getMidRect(regions, midRegion, d->strapDirection);
        CHECKERR(err);

        //检测铜片位置
        err = getCuPos(regions, midRegion, cuRegion, CuPos, d->minThre, d->strapDirection);
        CHECKERR(err);

        HTuple row1, col1, row2, col2;
        SmallestRectangle1(cuRegion, &row1, &col1, &row2, &col2);
        QRect resRect(QPoint(col1, row1), QPoint(col2, row2));

        if (d->CuPos != CuPos) {
            ngRect.push_back(resRect);
            return CError(CUPOS, "Cu Position err");
        } else {
            gdRect.push_back(resRect);
        }

        err = checkTapeNum(regions, cuRegion, d->rectList, gdRect, ngRect, d->numMinThre, d->numMaxThre, d->numAreaRatoi, d->regionSize);
        CHECKERR(err);

        if (ngRect.size())
            return CError(REGIONNUM, "tapeNum err");
        qDebug() << 9;
        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "CGetCuStrapPos::detect crashed!");
    }
}

CError CGetCuStrapPos::pamRead(const char* xmlfilePath)
{
    XmlRead xmlRead;

    try {
        std::map<std::string, xmlInfo> res = xmlRead.parseXML(xmlfilePath, taskName.toLocal8Bit().data());

        READPAM(d->CuPos, "CuPos", res);
        READPAM(d->minThre, "minThre", res);
        READPAM(d->strapDirection, "strapDirection", res);

        d->rectList.clear();
        READPAM(d->numMinThre, "numMinThre", res);
        READPAM(d->numMaxThre, "numMaxThre", res);
        READPAM(d->numAreaRatoi, "numAreaRatoi", res);

        qDebug() << d->numMinThre << d->numMaxThre << d->numAreaRatoi;

        RectInfo info;
        READPAM(info.XBia, "XBia1", res);
        READPAM(info.YBia, "YBia1", res);
        READPAM(info.len1, "len11", res);
        READPAM(info.len2, "len21", res);
        d->rectList.push_back(info);

        READPAM(info.XBia, "XBia2", res);
        READPAM(info.YBia, "YBia2", res);
        READPAM(info.len1, "len12", res);
        READPAM(info.len2, "len22", res);
        d->rectList.push_back(info);

        READPAM(info.XBia, "XBia3", res);
        READPAM(info.YBia, "YBia3", res);
        READPAM(info.len1, "len13", res);
        READPAM(info.len2, "len23", res);
        d->rectList.push_back(info);

        READPAM(info.XBia, "XBia4", res);
        READPAM(info.YBia, "YBia4", res);
        READPAM(info.len1, "len14", res);
        READPAM(info.len2, "len24", res);
        d->rectList.push_back(info);

        for (int i = 0; i < d->rectList.size(); i++) {

            qDebug() << "biaPam:"
                     << d->rectList[i].XBia
                     << d->rectList[i].YBia
                     << d->rectList[i].len1
                     << d->rectList[i].len2;
        }

        return 0;
    } catch (...) {
        return CError(PAMREAD, "task: CGetCuStrapPos, pamRead failed!");
    }
}

}
