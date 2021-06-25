#include "algorithm.h"
#include "QList"
#include "qline.h"
#include "qdebug.h"
#include "commonDef.h"

using namespace CSHDetect;
using namespace HalconCpp;

Algorithm::Algorithm()
{

}

CError Algorithm::objIsEmpty(const RHOBJ obj)
{
    try {
        bool ini = obj.IsInitialized();
        if (!ini) {
            return 1;
        }

        HObject null;
        HTuple number;
        GenEmptyObj(&null);
        TestEqualObj(obj, null, &number); //判定是否和空对象相等。

        ini = number == 1 ? 1 : 0;
        return ini;
    }  catch (...) {
        return CError(2, "Algorithm unexpected happened!");
    }
}

CError Algorithm::tupleisEmpty(const HTuple& tuple)
{
    try {
        HTuple Length;
        TupleLength(tuple, &Length);
        bool ini = (Length.I() == 0);
        return ini;
    }  catch (...) {
        return CError(2, "Algorithm unexpected happened!");
    }
}

CircleInfo Algorithm::getAverCircle(QList<CircleInfo> info)
{
    CircleInfo res;
    int num = 0;
    double totalX = 0;
    double totalY = 0;
    double totalR = 0;
    for (auto i : info) {
        totalX += i.X;
        totalY += i.Y;
        totalR += i.Radius;
        num ++;
    }

    res.X = totalX / num;
    res.Y = totalY / num;
    res.Radius = totalR / num;
    return res;
}

QPointF Algorithm::getCrossPoint(qreal pX, qreal pY, qreal lX1, qreal lY1, qreal lX2, qreal lY2)
{
    qreal lK = (lY2 - lY1) / (lX2 - lX1);
    qreal pK = -1 / (lK);
    qreal lB = lY1 - lK * lX1;
    qreal pB = pY - pK * pX;

    qreal X = (pB - lB) / (lK - pK);
    qreal Y = lK * X + lB;

    return QPointF(X, Y);
}

LineInfo Algorithm::getCrossInfo(qreal pX, qreal pY, const LineInfo& info)
{
    LineInfo data = info;
    qreal lX1 = info.startCol;
    qreal lY1 = info.startRow;
    qreal lX2 = info.endCol;
    qreal lY2 = info.endRow;

    qreal lK = (lY2 - lY1) / (lX2 - lX1);
    qreal pK = -1 / (lK);
    qreal lB = lY1 - lK * lX1;
    qreal pB = pY - pK * pX;

    qreal X = (pB - lB) / (lK - pK);
    qreal Y = lK * X + lB;

    data.pX = pX;
    data.pY = pY;
    data.crossX = X;
    data.crossY = Y;
    data.distance = QLineF(pX, pY, X, Y).length();

    return data;
}

const QString Algorithm::getErrDescri(const int errIndex)
{
    QString info;
    switch (errIndex) {
    case LACKAREA:
        info = "lackarea";
        break;
    case CUPOS:
        info = "cupos";
        break;
    case REGIONNUM:
        info = "regionnum";
        break;
    default:
        info = "Other";
    }
    return info;
}

HObject Algorithm::dynThre(const HObject& dst, const int threVal, const int meanSize)
{
    HObject ImageMean, dynImg;
    MeanImage(dst, &ImageMean, meanSize, meanSize);
    DynThreshold(dst, ImageMean, &dynImg, threVal, "light");
    return dynImg;
}

CError Algorithm::edgeLineDtct(const HObject& dst, RHOBJ resObj, LineInfo& resLine, const int direct, const int dtctTime,
                               const LineInfo dtctRegion, const MeasureposPam measurePam)
{
    try {
        CHECKEMPIMG(dst, "Algorithm::edgeLineDtct input image is empty!");

        HObject  Contour;
        int interval;
        HTuple  phi, colRatio, rowRatio;
        HTuple  Width, Height, edgeCol, edgeRow, indx;
        HTuple  row, col, MeasureHandle, RowEdge, ColumnEdge;
        HTuple  Amplitude, Distance, Nr, Nc, Dist;
        HTuple  RowBegin, ColBegin, RowEnd, ColEnd;

        HTuple transition, pntSlct;

        int endCol = dtctRegion.endCol.D();
        int endRow = dtctRegion.endRow.D();
        int startCol = dtctRegion.startCol.D();
        int startRow = dtctRegion.startRow.D();

        if (direct == 1) {
            phi = 3.1415926 / 2;
            interval = (endCol - startCol) / (dtctTime - 1);
            colRatio = 1;
            rowRatio = 0;
        } else {
            phi = 0;
            interval = (endRow - startRow) / (dtctTime - 1);
            colRatio = 0;
            rowRatio = 1;
        }
        GetImageSize(dst, &Width, &Height);

        switch (measurePam.transition) {
        case ALLTRANS:
            transition = "all";
            break;
        case POSITIVE:
            transition = "positive";
            break;
        case NAGETIVE:
            transition = "negative";
            break;
        default:
            transition = "all";
        }

        switch (measurePam.pntSelect) {
        case ALL:
            pntSlct = "all";
            break;
        case FIRSTPNT:
            pntSlct = "first";
            break;
        case LASTPNT:
            pntSlct = "last";
            break;
        default:
            pntSlct = "first";
            break;
        }

        edgeCol = HTuple();
        edgeRow = HTuple();
        {
            HTuple end_val16 = dtctTime - 1;
            HTuple step_val16 = 1;
            for (indx = 0; indx.Continue(end_val16, step_val16); indx += step_val16) {
                row = startRow + ((interval * rowRatio) * indx);
                col = startCol + ((interval * colRatio) * indx);
                GenMeasureRectangle2(row, col, phi, measurePam.recLen1, measurePam.recLen2, Width, Height, "nearest_neighbor", &MeasureHandle);
                MeasurePos(dst, MeasureHandle, measurePam.sigma, measurePam.threshold, transition,
                           pntSlct, &RowEdge, &ColumnEdge, &Amplitude, &Distance);

                edgeRow = edgeRow.TupleConcat(RowEdge);
                edgeCol = edgeCol.TupleConcat(ColumnEdge);
            }
        }

        GenContourPolygonXld(&Contour, edgeRow, edgeCol);
        FitLineContourXld(Contour, "tukey", -1, 0, 5, 2, &RowBegin, &ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);
        CHECKEMPTUPLE(RowBegin, "Algorithm::edgeLineDtct not get the line");
        resObj = Contour;

        resLine.startRow = RowBegin[0];
        resLine.startCol = ColBegin[0];
        resLine.endRow   = RowEnd[0];
        resLine.endCol   = ColEnd[0];
        resLine.angle = (resLine.endRow.D() - resLine.startRow.D()) / (resLine.endCol.D() - resLine.startCol.D()) * 180.0 / 3.1415926;

        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "edgeLineDtct algorithm crashed!");
    }
}

CError Algorithm::useGridGetRegion(const HObject& dst, HObject& resRegion, cint gridWid, cint gridHigt, cint minThre, cint eroValue, cint slctNum)
{
    try {
        if (slctNum <= 0)
            return CError(PAMVALUE, "Algorithm useGridGetRegion:: select num shold large than 0");

        CHECKEMPIMG(dst, "Algorithm::edgeLineDtct input image is empty!");

        HObject  RegionGrid, ImageReduced, Region;
        HObject  RegionFillUp, RegionErosion, ConnectedRegions1;
        HObject  ConnectedRegions, SelectedRegions;

        HTuple  Width, Height, Area, Row;
        HTuple  Column, Sorted;

        CHECKEMPIMG(dst, "Algorithm::useGridGetRegion input image is empty");
        GetImageSize(dst, &Width, &Height);

        GenGridRegion(&RegionGrid, gridWid, gridHigt, "lines", Width, Height);
        ReduceDomain(dst, RegionGrid, &ImageReduced);
        Threshold(ImageReduced, &Region, minThre, 255);

        FillUp(Region, &RegionFillUp);
        ErosionCircle(RegionFillUp, &RegionErosion, 2);
        Connection(RegionErosion, &ConnectedRegions);
        AreaCenter(ConnectedRegions, &Area, &Row, &Column);
        CHECKEMPTUPLE(Column, "Algorithm::useGridGetRegion dnot get the region after threshold");

        TupleSort(Area, &Sorted);
        SelectShape(ConnectedRegions, &SelectedRegions, "area", "and", HTuple(Sorted[(Area.TupleLength()) - slctNum]),
                    HTuple(Sorted[(Area.TupleLength()) - 1]) + 10);
        Union1(SelectedRegions, &resRegion);

        if (eroValue > 0)
            ErosionCircle(resRegion, &resRegion, eroValue);
        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "Algorithm::useGridGetRegion crashed!");
    }
}

CError Algorithm::getRegionByQuadrant(const RHOBJ dstObj, RHOBJ region, cint quadrant)
{
    try {
        CHECKEMPIMG(dstObj, "Algorithm::getRegionByQuadrant dstObj is empty");

        HObject detect, finalRegion;
        Connection(dstObj, &detect);

        HTuple area, row, col, rowMean, colMean;

        AreaCenter(detect, &area, &row, &col);
        TupleMean(col, &colMean);
        TupleMean(row, &rowMean);

        int row1 = 0;
        int row2 = 99999;
        int col1 = 0;
        int col2 = 99999;
        int midRow = rowMean.D();
        int midCol = colMean.D();

        switch (quadrant) {
        case 0:
            row2 = midRow;
            col1 = midCol;
            break;
        case 1:
            row2 = midRow;
            col2 = midCol;
            break;
        case 2:
            row1 = midRow;
            col2 = midCol;
            break;
        case 3:
            row1 = midRow;
            col1 = midCol;
            break;
        default:
            break;
        }

        GenEmptyRegion(&finalRegion);
        SelectShape(detect, &finalRegion, (HTuple("row").Append("column")),
                    "and", (HTuple(row1).Append(col1)), (HTuple(row2).Append(col2)));

        CHECKEMPIMG(finalRegion, "Algorithm::getRegionByQuadrant slected region is empty");

        AreaCenter(finalRegion, &area, &row, &col);
        if (area.Length() != 1)
            return CError(REGIONNUM, QString("Algorithm::getRegionByQuadrant slected is not 1: %1").arg(area.Length()));

        region = finalRegion;
        return 0;
    } catch (...) {
        return CError(UNEXCEPTION, "Algorithm::getRegionByQuadrant crashed!");
    }
}

CError Algorithm::detectRegionExit(CONSTIMG dstObj, RHOBJ showObj, const QVector<RectInfo> &inRect, QVector<RectInfo> &gdRect, QVector<RectInfo> &ngRect,
                                   const QPoint basePt, cint minThre, cint maxThre, const qreal percent, const QSize minSize)
{
    try {
        CHECKEMPIMG(dstObj, "Algorithm::detectRegionExit dstObj empty");

        int rectSize = inRect.size();
        if(rectSize == 0)
            return CError(PAMVALUE, "Algorithm::detectRegionExit inRect is empty");

        gdRect.clear();
        ngRect.clear();

        HTuple row, col, phi, len1, len2;
        HTuple row1, row2, col1, col2;
        HTuple area, cols, rows;
        HObject hRect, hReduceImg, hThreRg;

        GenEmptyObj(&showObj);
        double resRatio = 0;
        RectInfo info;
        for(auto i : inRect){
            row = basePt.y() + i.YBia;
            col = basePt.x() + i.XBia;
            phi = i.phi;
            len1= i.len1;
            len2= i.len2;

            info = i;
            info.row = row.D();
            info.col = col.D();
            GenRectangle2(&hRect, row, col, phi, len1, len2);
            ReduceDomain(dstObj, hRect, &hReduceImg);

            Threshold(hReduceImg, &hThreRg, minThre, maxThre);
            Union2(showObj, hThreRg, &showObj);
            AreaCenter(hThreRg, &area, &rows, &cols);
            SmallestRectangle1(hThreRg, &row1, &col1, &row2, &col2);

            bool isOK = (minSize.width() > (col2.D() - col1.D())) &&
                        (minSize.height()> (row2.D() - row1.D()));

            if(objIsEmpty(hThreRg))
                resRatio = 0;
            else
                resRatio = area.D()/(len1.D() * len2.D()*4);

            if((resRatio >= percent)&& isOK)
                gdRect.push_back(info);
            else
                ngRect.push_back(info);
        }
        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "Algorithm::detectRegionExit crashed");
    }
}
