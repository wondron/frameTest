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

    QList<PosBia> m_posPams;
    QList<MeasureposPam> m_measurePams;
};

CGetTapeAngle::CGetTapeAngle():
    d(new CGetTapeAnglePrivate())
{

}

CError CGetTapeAngle::detect(const HObject& dst, const ReverRegionS& res, QList<QLine>& gdLine, QList<QLine>& ngLine)
{
    CError err;
    CError errTotal(ANGLE, "tape angle failed");
    int errTime = 0;

    for(int i = 0; i < 4; i++){
        LineInfo lineInfo;
        err = getSignalLines(dst, res.hanregion, lineInfo, i, d->m_posPams[i], d->m_measurePams[i]);

        if(err.isWrong())
            errTime++;

        if(err.code() == ANGLE)
            ngLine.push_back(QLine(lineInfo.startCol.D(), lineInfo.startRow.D(), lineInfo.endCol.D(), lineInfo.endRow.D()));

        if(!err.isWrong())
            gdLine.push_back(QLine(lineInfo.startCol.D(), lineInfo.startRow.D(), lineInfo.endCol.D(), lineInfo.endRow.D()));
    }
    if(errTime == 0)
        errTotal = 0;

    return errTotal;
}

CError CGetTapeAngle::pamRead(const char* xmlfilePath)
{
    XmlRead xmlRead;

    d->m_posPams.clear();
    d->m_measurePams.clear();
    PosBia bia;
    MeasureposPam pam;

    try {
        std::map<std::string, xmlInfo> res = xmlRead.parseXML(xmlfilePath, taskName.toLocal8Bit().data());
        READPAM(bia.XBia,       "XBia1", res);
        READPAM(bia.YBia,       "YBia1", res);
        READPAM(bia.lineDirect, "lineDirect1", res);
        READPAM(bia.lineWidth,  "lineWidth1", res);
        READPAM(bia.detectTime, "detectTime1", res);
        READPAM(bia.angleDown,  "angleDown1", res);
        READPAM(bia.angleUp,    "angleUp1", res);
        d->m_posPams.push_back(bia);
        READPAM(pam.recLen1,    "recLen11", res);
        READPAM(pam.recLen2,    "recLen21", res);
        READPAM(pam.transition, "transition1", res);
        READPAM(pam.pntSelect,  "pntSelect1", res);
        READPAM(pam.sigma,      "sigma1", res);
        READPAM(pam.threshold,  "threshold1", res);
        d->m_measurePams.push_back(pam);

        READPAM(bia.XBia,       "XBia2", res);
        READPAM(bia.YBia,       "YBia2", res);
        READPAM(bia.lineDirect, "lineDirect2", res);
        READPAM(bia.lineWidth,  "lineWidth2", res);
        READPAM(bia.detectTime, "detectTime2", res);
        READPAM(bia.angleDown,  "angleDown2", res);
        READPAM(bia.angleUp,    "angleUp2", res);
        d->m_posPams.push_back(bia);
        READPAM(pam.recLen1,    "recLen12", res);
        READPAM(pam.recLen2,    "recLen22", res);
        READPAM(pam.transition, "transition2", res);
        READPAM(pam.pntSelect,  "pntSelect2", res);
        READPAM(pam.sigma,      "sigma2", res);
        READPAM(pam.threshold,  "threshold2", res);
        d->m_measurePams.push_back(pam);

        READPAM(bia.XBia,       "XBia3", res);
        READPAM(bia.YBia,       "YBia3", res);
        READPAM(bia.lineDirect, "lineDirect3", res);
        READPAM(bia.lineWidth,  "lineWidth3", res);
        READPAM(bia.detectTime, "detectTime3", res);
        READPAM(bia.angleDown,  "angleDown3", res);
        READPAM(bia.angleUp,    "angleUp3", res);
        d->m_posPams.push_back(bia);
        READPAM(pam.recLen1,    "recLen13", res);
        READPAM(pam.recLen2,    "recLen23", res);
        READPAM(pam.transition, "transition3", res);
        READPAM(pam.pntSelect,  "pntSelect3", res);
        READPAM(pam.sigma,      "sigma3", res);
        READPAM(pam.threshold,  "threshold3", res);
        d->m_measurePams.push_back(pam);

        READPAM(bia.XBia,       "XBia4", res);
        READPAM(bia.YBia,       "YBia4", res);
        READPAM(bia.lineDirect, "lineDirect4", res);
        READPAM(bia.lineWidth,  "lineWidth4", res);
        READPAM(bia.detectTime, "detectTime4", res);
        READPAM(bia.angleDown,  "angleDown4", res);
        READPAM(bia.angleUp,    "angleUp4", res);
        d->m_posPams.push_back(bia);
        READPAM(pam.recLen1,    "recLen14", res);
        READPAM(pam.recLen2,    "recLen24", res);
        READPAM(pam.transition, "transition4", res);
        READPAM(pam.pntSelect,  "pntSelect4", res);
        READPAM(pam.sigma,      "sigma4", res);
        READPAM(pam.threshold,  "threshold4", res);
        d->m_measurePams.push_back(pam);


//        for(int i = 0; i < 4; i++){

//            qDebug() << "biaPam:"
//                     << d->m_posPams[i].XBia
//                     << d->m_posPams[i].YBia
//                     << d->m_posPams[i].lineDirect
//                     << d->m_posPams[i].lineWidth
//                     << d->m_posPams[i].detectTime
//                     << d->m_posPams[i].angleDown
//                     << d->m_posPams[i].angleUp;

//            qDebug() << "measurePam:"
//                     << d->m_measurePams[i].recLen1
//                     << d->m_measurePams[i].recLen2
//                     << d->m_measurePams[i].transition
//                     << d->m_measurePams[i].pntSelect
//                     << d->m_measurePams[i].sigma
//                     << d->m_measurePams[i].threshold;
//        }

        return 0;

    } catch (...) {
        return CError(PAMREAD, "task: CGetTapeAngle, pamRead failed!");
    }
}

void CGetTapeAngle::getPamValue(QList<MeasureposPam> &pam, QList<PosBia> &bia)
{
    pam = d->m_measurePams;
    bia = d->m_posPams;
}

CError CGetTapeAngle::getSignalLines(const HObject& dst, const HObject& hConnect, LineInfo& lines, const int quadrant, const PosBia biaInfo, const MeasureposPam pam)
{
    try {
        CError err;

        HObject sigHanReg, showLine;
        HTuple  Row, Column, Area;

        if(biaInfo.detectTime < 3)
            return CError(PAMVALUE, QString("CGetTapeAngle::getSignalLines biaInfo.detectTime should > 3, value:%1").arg(biaInfo.detectTime));

        err = Algorithm::getRegionByQuadrant(hConnect, sigHanReg, quadrant);
        CHECKERR(err);
        AreaCenter(sigHanReg, &Area, &Row, &Column);

        LineInfo dtctLine;

        if (biaInfo.lineDirect == 1) {
            dtctLine.startRow = biaInfo.YBia + Row.D();
            dtctLine.startCol = biaInfo.XBia + Column.D() - biaInfo.lineWidth / 2;
            dtctLine.endRow   = biaInfo.YBia + Row.D();
            dtctLine.endCol   = biaInfo.XBia + Column.D() + biaInfo.lineWidth / 2;
        } else {
            dtctLine.startRow = biaInfo.YBia + Row.D() - biaInfo.lineWidth / 2;
            dtctLine.startCol = biaInfo.XBia + Column.D();
            dtctLine.endRow   = biaInfo.YBia + Row.D() + biaInfo.lineWidth / 2;
            dtctLine.endCol   = biaInfo.XBia + Column.D();
        }

        err = Algorithm::edgeLineDtct(dst, showLine, lines, biaInfo.lineDirect, biaInfo.detectTime, dtctLine, pam);
        CHECKERR(err);

        if (lines.angle < biaInfo.angleDown || lines.angle > biaInfo.angleUp)
            err = CError(ANGLE, QString("angle is out of range ,value:%1").arg(lines.angle));

        return err;
    } catch (...) {
        return CError(UNEXCEPTION, "CGetTapeAngle::getSignalLines crashed!");
    }
}

}
