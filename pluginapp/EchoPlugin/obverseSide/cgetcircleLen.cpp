#include "cgetcircleLen.h"
#include "../commonUsage/commonDef.h"
#include "qdebug.h"
#include "utils/XmlRead.h"

namespace CSHDetect {

class CGetCircleLenPrivate
{
public:
    CGetCircleLenPrivate(CGetCircleLen* parent): p(parent)
    {

    }

    int direction = LEFTRIGHT;

    int tapeDetectNum = 4; //蓝胶区域的检测数
    int ringRadius = 210;
    int circleCenterDilateLen = 4500;
    int psToLineDisCtrl = 300;

    double lRow = 0;
    double rRow = 0;

private:
    CGetCircleLen* p;
};

CGetCircleLen::CGetCircleLen(): d(new CGetCircleLenPrivate(this))
{

}

CGetCircleLen::CGetCircleLen(const QString name):
    d(new CGetCircleLenPrivate(this))
{
    taskName = name;
}

CError CGetCircleLen::detect(const RegionS& testInfo, QList<LineInfo>& lineList, QList<int>& errIndex)
{
    CError err;
    QList<LineInfo> lines;
    err = getLineInfo(testInfo, lines);
    CHECKERR(err);

    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].distance > d->psToLineDisCtrl)
            errIndex.push_back(i);
    }
    lineList.clear();
    lineList = lines;

    if (errIndex.size())
        return CError(LACKAREA, "distance out of control");

    return 0;
}

CError CGetCircleLen::getLineInfo(const RegionS& testInfo, QList<LineInfo>& lineList)
{
    CError res;
    CircleInfo smallPos, bigPos;
    HObject obj;
    LineInfo line;
    lineList.clear();

    QPointF center;
    res = getCircleCenter(testInfo, smallPos, bigPos, d->ringRadius, d->circleCenterDilateLen, d->direction);
    CHECKERR(res);

    int pointX, pointY;
    for (int i = 0 ; i < 4; i++) {
        res = getDetectLine(testInfo, obj, line, i, d->direction);
        CHECKERR(res);

        if (i == 1 || i == 2) {
            pointX = smallPos.X;
            pointY = smallPos.Y;
        } else {
            pointX = bigPos.X;
            pointY = bigPos.Y;
        }
        LineInfo inf = Algorithm::getCrossInfo(pointX, pointY, line);
        lineList.push_back(inf);
    }
    return 0;
}

CError CGetCircleLen::getCircleCenter(const RegionS& testInfo, CircleInfo& smallPos, CircleInfo& bigPos, int ringRadius, int dilaLength, int direction)
{
    try {
        HObject  blueRegion, blackBangRegion, RImage, tapeRegion, darkRegion;
        HObject  ho_RegionDilation;
        HObject  ho_Rectangle, ho_ImageReduced, ho_RegionDifference;
        HObject  ho_ImageReduced1, ho_Border, ho_SelectedXLD, ho_UnionContours;
        HObject  ho_ContCircle, ho_SelectedXLD1;

        HTuple  area, row, column, hv_Row1, hv_Column1;
        HTuple  hv_Row2, hv_Column2, hv_Mean, hv_Deviation, rows, cols, radius;
        HTuple  hv_StartPhi, hv_EndPhi, hv_PointOrder;
        HTuple  hv_rightRingRow, hv_rightRingCol, hv_rightRadius;
        HTuple  hv_leftRingRow, hv_leftRingCol, hv_leftRadius, hv_Index1;

        RImage = testInfo.m_RImg;
        CHECKEMPIMG(RImage, "CGetCircleLen::getCircleCenter, testInfo.m_RImg is empty");

        blueRegion = testInfo.BlueRegion;
        CHECKEMPIMG(blueRegion, "CGetCircleLen::getCircleCenter, testInfo.BlueRegion is empty");

        tapeRegion = testInfo.TapeRegion;
        CHECKEMPIMG(tapeRegion, "CGetCircleLen::getCircleCenter, testInfo.tapeRegion is empty");

        darkRegion = testInfo.DarkRegion;
        CHECKEMPIMG(darkRegion, "CGetCircleLen::getCircleCenter, testInfo.darkRegion is empty");

        blackBangRegion = testInfo.BlackBangRegion;
        CHECKEMPIMG(blackBangRegion, "CGetCircleLen::getCircleCenter, testInfo.BlackBangRegion is empty");
        AreaCenter(blackBangRegion, &area, &row, &column);

        DilationRectangle1(blackBangRegion, &ho_RegionDilation, dilaLength, 1);
        SmallestRectangle1(ho_RegionDilation, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
        GenRectangle1(&ho_Rectangle, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
        ReduceDomain(RImage, ho_Rectangle, &ho_ImageReduced);

        Difference(ho_RegionDilation, blackBangRegion, &ho_RegionDifference);
        Difference(ho_RegionDifference, tapeRegion, &ho_RegionDifference);
        Difference(ho_RegionDifference, blueRegion, &ho_RegionDifference);
        Difference(ho_RegionDifference, darkRegion, &ho_RegionDifference);

        ReduceDomain(ho_ImageReduced, ho_RegionDifference, &ho_ImageReduced1);
        Intensity(ho_RegionDifference, ho_ImageReduced1, &hv_Mean, &hv_Deviation);
        ThresholdSubPix(ho_ImageReduced1, &ho_Border, 128);
        SelectShapeXld(ho_Border, &ho_SelectedXLD, "width", "and", ringRadius * 0.8, ringRadius * 2.5);
        SelectShapeXld(ho_SelectedXLD, &ho_SelectedXLD, "height", "and", ringRadius * 0.8, ringRadius * 2.5);
        CHECKEMPIMG(ho_SelectedXLD, "CGetCircleLen::getCircleCenter, ho_SelectedXLD is empty")

        UnionAdjacentContoursXld(ho_SelectedXLD, &ho_UnionContours, 40, 1, "attr_forget");
        FitCircleContourXld(ho_UnionContours, "algebraic", -1, 0, 0, 3, 2, &rows, &cols,
                            &radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);

        QList<CircleInfo> smallInfo;
        QList<CircleInfo> bigInfo;
        CircleInfo cInfo;
        int val = (direction == LEFTRIGHT) ? testInfo.width / 2 : testInfo.height / 2;

        for (int i = 0; i < cols.Length(); i++) {
            qreal r = radius[i].D();
            if ((r < ringRadius * 1.1) && (r > ringRadius * 0.8)) {

                cInfo.X = cols[i].D();
                cInfo.Y = rows[i].D();
                cInfo.Radius = radius[i].D();

                qreal testval = (direction == LEFTRIGHT) ? cols[i].D() : rows[i].D();
                if (val < testval)
                    bigInfo.push_back(cInfo);
                else
                    smallInfo.push_back(cInfo);
            }
        }

        if (!bigInfo.size()) return CError(EMPTYOBJ, "not find the down/right circle");
        if (!smallInfo.size()) return CError(EMPTYOBJ, "not find the up/left circle");

        bigPos = Algorithm::getAverCircle(bigInfo);
        smallPos = Algorithm::getAverCircle(smallInfo);

        d->rRow = bigPos.Y;
        d->lRow = smallPos.Y;
        return 0;

    }  catch (...) {
        return CError(UNEXCEPTION, "CGetCircleLen::getCircleCenter unexception happen!");
    }
}

CError CGetCircleLen::getDetectLine(const RegionS& Info, HObject& showObj, LineInfo& line, int quadrantPos, int direction)
{
    Q_UNUSED(direction);
    try {
        HObject region, contours, contoursSplit;
        HObject selectedXLD, unionContours, selectedXLD1, selectedXLD2;

        HTuple hv_RowBegin, hv_ColBegin, hv_RowEnd, hv_ColEnd, hv_Nr, hv_Nc, hv_Dist;
        HTuple area, col, row, order, meanRow;
        double top, button;

        //选择对应象限的区域
        CError res = selectRegion(Info, region, quadrantPos);
        CHECKERR(res);
        showObj = region.Clone();
        CHECKEMPIMG(region, "CGetCircleLen::getDetectLine region is empty");
        AreaCenter(region, &area, &row, &col);

        GenContourRegionXld(region, &contours, "border");
        SegmentContoursXld(contours, &contoursSplit, "lines", 5, 4, 2);
        SelectShapeXld(contoursSplit, &selectedXLD, "ratio", "and", 0, 0.5);
        CHECKEMPIMG(selectedXLD, "CGetCircleLen::getDetectLine selectedXLD is empty");

        UnionAdjacentContoursXld(selectedXLD, &unionContours, 50, 50, "attr_keep");
        SelectShapeXld(unionContours, &selectedXLD1, "width", "and", 150, 9999999);
        CHECKEMPIMG(selectedXLD, "CGetCircleLen::getDetectLine selectedXLD1 is empty");

        CError err = slctRows(top, button, row, quadrantPos);

        if (err.code()) return err;
        SelectShapeXld(selectedXLD1, &selectedXLD2, "row", "and", top, button);

        CHECKEMPIMG(selectedXLD, "CGetCircleLen::getDetectLine selectedXLD2 is empty");
        UnionAdjacentContoursXld(selectedXLD2, &unionContours, 9999, 9999, "attr_keep");

        FitLineContourXld(unionContours, "tukey", -1, 0, 5, 2, &hv_RowBegin, &hv_ColBegin,
                          &hv_RowEnd, &hv_ColEnd, &hv_Nr, &hv_Nc, &hv_Dist);

        CHECKEMPTUPLE(hv_RowBegin, "CGetCircleLen::getDetectLine hv_RowBegin is empty");

        line.startRow = hv_RowBegin;
        line.startCol = hv_ColBegin;
        line.endRow = hv_RowEnd;
        line.endCol = hv_ColEnd;
        return 0;

    }  catch (...) {
        return CError(UNEXCEPTION, "CGetCircleLen::getDetectLine unexception happen!");
    }
}

CError CGetCircleLen::pamRead(const char* xmlfilePath)
{
    XmlRead xmlRead;

    try {
        std::map<std::string, xmlInfo> res = xmlRead.parseXML(xmlfilePath, taskName.toLocal8Bit().data());

        READPAM(d->direction, "direction", res);
        READPAM(d->tapeDetectNum, "tapeDetectNum", res);
        READPAM(d->ringRadius, "ringRadius", res);
        READPAM(d->circleCenterDilateLen, "circleCenterDilateLen", res);
        READPAM(d->psToLineDisCtrl, "psToLineDisCtrl", res);

        return 0;

    } catch (...) {
        return CError(PAMREAD, "task: GetCircleLen, pamRead failed!");
    }
}

CError CGetCircleLen::selectRegion(const RegionS& Info, HObject& region, int quadrantPos)
{
    try {
        HObject weldRoi = Info.WeldRoi;
        CHECKEMPIMG(weldRoi, "CGetCircleLen::selectRegion Info.WeldRoi is empty");

        HObject connectedRegions;
        HTuple area, col, row, rowMean, colMean;
        Connection(weldRoi, &connectedRegions);
        AreaCenter(connectedRegions, &area, &row, &col);

        if (!(area.Length() == 2 || area.Length() == 4))
            CError(UNEXCEPTION, "CGetCircleLen::getDetectLine region size is not 2 or 4!");

        TupleMean(col, &colMean);
        TupleMean(row, &rowMean);

        int row1 = 0;
        int row2 = Info.height;
        int col1 = 0;
        int col2 = Info.width;
        int midRow = rowMean.D();
        int midCol = colMean.D();

        switch (quadrantPos) {
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

        SelectShape(connectedRegions, &region, (HTuple("row").Append("column")),
                    "and", (HTuple(row1).Append(col1)), (HTuple(row2).Append(col2)));

        AreaCenter(region, &area, &row, &col);
        if (area.Length() != 1)
            return CError(REGIONNUM, QString("CGetCircleLen::selectRegion slected is not 1: %1").arg(area.Length()));

        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "CGetCircleLen::selectRegion crash!");
    }
}

CError CGetCircleLen::slctRows(double& top, double& button, const double meanRow, const int quadrantPos)
{
    if (d->rRow == 0)
        return CError(PAMVALUE, "d.rRow is 0");

    if (meanRow == 0)
        return CError(PAMVALUE, "CGetCircleLen::slctRows meanRow is 0");

    switch (quadrantPos) {
    case 0:
        top = meanRow;
        button = 5000;
        break;
    case 1:
        top = meanRow;
        button = 5000;
        break;
    case 2:
        top = 1;
        button = meanRow;
        break;
    case 3:
        top = 1;
        button = meanRow;
        break;
    }

    return 0;
}
}


