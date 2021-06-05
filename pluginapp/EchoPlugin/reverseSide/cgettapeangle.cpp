#include "cgettapeangle.h"
#include "qdebug.h"
#include "../commonUsage/commonDef.h"
#include "utils/XmlRead.h"

namespace CSHDetect {

class CGetTapeAnglePrivate
{
public:
    CGetTapeAnglePrivate() {}
    ~CGetTapeAnglePrivate() {}

    //总体
    int tapeNum = 4;

    VLineData vLine;
    HLineData upLine;
    HLineData dnLine;
};

CGetTapeAngle::CGetTapeAngle():
    d(new CGetTapeAnglePrivate())
{

}

CError CGetTapeAngle::detect(const HObject& dst, const ReverRegionS& res, QList<QLine>& gdLine, QList<QLine>& ngLine)
{
    CError err;
    QList<QLine> lines;
    QString errInfo;
    HObject slctReg, tapeReg, hConnect;
    tapeReg = res.blueTapesReg;

    err = checkTapeNum(tapeReg, hConnect, d->tapeNum);
    CHECKERR(err);

    int allCrashtime = 0;
    for (int quadrant = 0; quadrant < 4; quadrant++) {
        err = getSignalLines(dst, hConnect, lines, quadrant, d->upLine, d->dnLine, d->vLine);
        if (err.code() == NG) {
            errInfo = QString("quadrant: %1, angle out of STD");
            ngLine.append(lines);
        } else if (!err.isWrong())
            gdLine.append(lines);
        else {
            errInfo = QString("quadrant: %1, all Detect crashed!");
            allCrashtime ++;
        }
        lines.clear();
    }

    if (allCrashtime || ngLine.size())
        return CError(NG, errInfo);

    return 0;
}

CError CGetTapeAngle::pamRead(const char* xmlfilePath)
{
    XmlRead xmlRead;

    try {
        std::map<std::string, xmlInfo> res = xmlRead.parseXML(xmlfilePath, taskName.toLocal8Bit().data());

        READPAM(d->tapeNum,            "tapeNum",           res);

        READPAM(d->vLine.angleDown,    "vLineangleDown",    res);
        READPAM(d->vLine.angleUp,      "vLineangleUp",      res);
        READPAM(d->vLine.dilaWid,      "vLinedilaWid",      res);
        READPAM(d->vLine.eroHigh,      "vLineeroHigh",      res);
        READPAM(d->vLine.minConLen,    "vLineminConLen",    res);
        READPAM(d->vLine.threSubValue, "vLinethreSubValue", res);

        READPAM(d->upLine.angleDown,   "upLineangleDown",   res);
        READPAM(d->upLine.angleUp,     "upLineangleUp",     res);
        READPAM(d->upLine.selectRatio, "upLineselectRatio", res);
        READPAM(d->upLine.subThreVal,  "upLinesubThreVal",  res);
        READPAM(d->upLine.xExpand,     "upLinexExpand",     res);
        READPAM(d->upLine.yExpand,     "upLineyExpand",     res);

        READPAM(d->dnLine.angleDown,   "dnLineangleDown",   res);
        READPAM(d->dnLine.angleUp,     "dnLineangleUp",     res);
        READPAM(d->dnLine.selectRatio, "dnLineselectRatio", res);
        READPAM(d->dnLine.subThreVal,  "dnLinesubThreVal",  res);
        READPAM(d->dnLine.xExpand,     "dnLinexExpand",     res);
        READPAM(d->dnLine.yExpand,     "dnLineyExpand",     res);

        return 0;

    } catch (...) {
        return CError(PAMREAD, "task: CGetTapeAngle, pamRead failed!");
    }
}

void CGetTapeAngle::setUpPam(const HLineData& pam)
{
    d->upLine = pam;
}

void CGetTapeAngle::setDownPam(const HLineData& pam)
{
    d->dnLine = pam;
}

void CGetTapeAngle::setVPam(const VLineData& pam)
{
    d->vLine = pam;
}

CError CGetTapeAngle::checkTapeNum(const HObject& tapeReg, HObject& connectReg, const int num)
{
    try {
        CHECKEMPIMG(tapeReg, "CGetTapeAngle::checkTapeNum tapeReg is empty");

        HTuple area, row, col;
        Connection(tapeReg, &connectReg);
        AreaCenter(connectReg, &area, &row, &col);

        if (row.Length() != num)
            CError(REGIONNUM, QString("CGetTapeAngle::slctRegion region size is not %1").arg(d->tapeNum));

        return 0;

    } catch (...) {
        return CError(PAMREAD, "CGetTapeAngle::checkTapeNum crash!");
    }
}

CError CGetTapeAngle::slctRegion(const HObject& hConnect, HObject& region, int quadrant)
{
    try {
        CHECKEMPIMG(hConnect, "CGetTapeAngle::slctRegion hConnect is empty");

        HTuple area, row, col, rowMean, colMean;
        AreaCenter(hConnect, &area, &row, &col);

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

        SelectShape(hConnect, &region, (HTuple("row").Append("column")),
                    "and", (HTuple(row1).Append(col1)), (HTuple(row2).Append(col2)));

        AreaCenter(region, &area, &row, &col);

        if (area.Length() != 1)
            return CError(REGIONNUM, QString("CGetTapeAngle::slctRegion slected is not 1: %1").arg(area.Length()));

        return 0;
    } catch (...) {
        return CError(UNEXCEPTION, "CGetTapeAngle::slctRegion crashed!");
    }
}

CError CGetTapeAngle::getSignalLines(const HObject& dst, const HObject& hConnect, QList<QLine>& lines, int quadrant,
                                     HLineData& upPam, HLineData& dnPam, VLineData& vPam)
{
    try {
        CHECKEMPIMG(dst, "getSignalLines::dst is empty");

        CError err;
        CError totalErr(NG, QString("quadrant: %1, all Line out of range").arg(quadrant));

        QLine line;
        HObject sigTapeReg;
        int crashTime = 0;
        HTuple  Row1, Column1, Row2, Column2;
        lines.clear();

        err = slctRegion(hConnect, sigTapeReg, quadrant);
        CHECKERR(err);

        SmallestRectangle1(sigTapeReg, &Row1, &Column1, &Row2, &Column2);
        QLine upLine(Column1.D(), Row1.D(), Column2.D(), Row1.D());
        QLine dnLine(Column1.D(), Row2.D(), Column2.D(), Row2.D());

        line = QLine(0, 0, 0, 0);
        err = getHline(dst, sigTapeReg, line, upLine, upPam);//求上边

        if (!err.isWrong())
            totalErr = 0;

        if (err.isWrong() && (err.code() != NG))
            crashTime ++;
        lines.push_back(line);

        line = QLine(0, 0, 0, 0);
        err = getHline(dst, sigTapeReg, line, dnLine, dnPam);//求下边

        if (!err.isWrong())
            totalErr = 0;

        if (err.isWrong() && (err.code() != NG))
            crashTime ++;
        lines.push_back(line);

        line = QLine(0, 0, 0, 0);
        err = getVline(dst, sigTapeReg, line, vPam); //求竖边

        if (!err.isWrong())
            totalErr = 0;

        if (err.isWrong() && (err.code() != NG))
            crashTime ++;
        lines.push_back(line);

        if (crashTime > 2)
            totalErr = CError(UNEXCEPTION, QString("quadrant: %1, all Line detect crashed").arg(quadrant));

        return totalErr;
    } catch (...) {
        return CError(UNEXCEPTION, "CGetTapeAngle::getSignalLines crashed!");
    }
}

CError CGetTapeAngle::getHline(const HObject& dst, const HObject& sigTapeReg, QLine& line,
                               const QLine pos, HLineData& pam)
{
    try {
        CHECKEMPIMG(dst, "getSignalLines::dst is empty");
        CHECKEMPIMG(sigTapeReg, "getSignalLines::doubleTapeRoi is empty");

        HObject  hRect, hReduce, hLine;
        HObject  Border, ContoursSplit, hSlctXld;
        HTuple  RowBegin, ColBegin, RowEnd, ColEnd, Nr, Nc, Dist;

        //求上边
        int y1 = pos.y1() - pam.yExpand;
        int x1 = pos.x1() - pam.xExpand;
        int y2 = pos.y2() + pam.yExpand;
        int x2 = pos.x2() + pam.xExpand;
        int width = std::abs(x1 - x2);

        GenRectangle1(&hRect, y1, x1, y2, x2);

        ReduceDomain(dst, hRect, &hReduce);
        ThresholdSubPix(hReduce, &Border, pam.subThreVal);
        SegmentContoursXld(Border, &ContoursSplit, "lines", 5, 4, 2);
        SelectContoursXld(ContoursSplit, &hSlctXld, "contour_length",
                          width * pam.selectRatio, 200, -0.5, 0.5);
        CHECKEMPIMG(hSlctXld, "getHline::hSlctXld is empty");

        FitLineContourXld(hSlctXld, "tukey", -1, 0, 5, 2,
                          &RowBegin, &ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);
        GenRegionLine(&hLine, RowBegin, ColBegin, RowEnd, ColEnd);
        line = QLine(ColBegin.D(), RowBegin.D(), ColEnd.D(), RowEnd.D());

        float angTan = (RowEnd - RowBegin) / (ColEnd - ColBegin);
        float angle = std::atan(angTan);
        pam.angle = angle;

        if (angle < pam.angleDown || angle > pam.angleUp)
            return CError(NG, "CGetTapeAngle::getHline get the angle out of STD");

        return 0;

    } catch (...) {
        return CError(UNEXCEPTION, "CGetTapeAngle::getSignalLines crashed!");
    }
}

CError CGetTapeAngle::getVline(const HObject& dst, const HObject& sigTapeReg, QLine& line, VLineData& pam)
{
    try {
        CHECKEMPIMG(dst, "CGetTapeAngle::getVline::dst is empty");
        CHECKEMPIMG(sigTapeReg, "CGetTapeAngle::getVline::sigTapeReg is empty");
        CHECKTHREVALUE(pam.threSubValue, "threSubvalue out of range");

        HObject hRect, hDilaReg, hEroReg, hReduce;
        HObject xldBorder, xldSlct, hLine;
        HTuple  RowBegin, ColBegin, RowEnd, ColEnd, Nr, Nc, Dist;

        GenRectangle1(&hRect, 0, 0, 0, pam.dilaWid);
        Dilation1(sigTapeReg, hRect, &hDilaReg, 1);
        GenRectangle1(&hRect, 0, 0, pam.eroHigh, 0);
        Erosion1(hDilaReg, hRect, &hEroReg, 1);
        ReduceDomain(dst, hEroReg, &hReduce);
        ThresholdSubPix(hReduce, &xldBorder, pam.threSubValue);
        SelectContoursXld(xldBorder, &xldSlct, "contour_length", pam.minConLen, 20000, -0.5, 0.5);
        CHECKEMPIMG(xldSlct, "CGetTapeAngle::getVline::xldSlct is empty");

        FitLineContourXld(xldSlct, "tukey", -1, 0, 5, 2, &RowBegin, &ColBegin,
                          &RowEnd, &ColEnd, &Nr, &Nc, &Dist);
        GenRegionLine(&hLine, RowBegin, ColBegin, RowEnd, ColEnd);
        line = QLine(ColBegin.D(), RowBegin.D(), ColEnd.D(), RowEnd.D());

        float angTan = (RowEnd.D() - RowBegin.D()) / (ColEnd.D() - ColBegin.D());
        float angle = abs(std::atan(angTan));
        pam.angle = angle;

        if (angle < pam.angleDown || angle > pam.angleUp)
            return CError(NG, "CGetTapeAngle::getVline get the angle out of STD");

        return 0;
    }  catch (...) {
        return CError(UNEXCEPTION, "CGetTapeAngle::getVline crashed!");
    }
}


}
