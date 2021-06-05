#include "cgettapeanglewidget.h"
#include "ui_cgettapeanglewidget.h"
#include "halconGraphic/halconwidget.h"
#include "qmessagebox.h"
#include "commonUsage/algorithm.h"
#include "commonUsage/commonDef.h"
#include "qdebug.h"

#define SENDERR(a) \
    {QMessageBox::warning(this, "warnning", a);\
    return;}

namespace CSHDetect {

class CGetTapeAngleWidgetPrivate
{
public:
    CGetTapeAngleWidgetPrivate()
    {
        m_widget = new Graphics::halconWidget();
        m_pam = new CGetTapeAngle();
    }

    Graphics::halconWidget* m_widget;
    ReverRegionS m_region;
    CGetTapeAngle* m_pam;

    HObject slctReg;

    bool isSetRegion = false;

};

CGetTapeAngleWidget::CGetTapeAngleWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CGetTapeAngleWidget),
    d(new CGetTapeAngleWidgetPrivate())
{
    ui->setupUi(this);
    ui->vlay_widget->addWidget(d->m_widget);
}

CGetTapeAngleWidget::~CGetTapeAngleWidget()
{
    delete ui;
}

void CGetTapeAngleWidget::setRegions(const ReverRegionS& region)
{
    CError err = Algorithm::objIsEmpty(region.dblTapeReg);
    if (err.isWrong())
        SENDERR("input regiono is empty");

    d->m_region = region;
    d->isSetRegion = true;
    d->m_widget->setShowImage(region.m_oriImg);
}

void CGetTapeAngleWidget::setPam(CGetTapeAngle* pam)
{
    d->m_pam = pam;
}

HLineData CGetTapeAngleWidget::getUpPam()
{
    HLineData upPam;
    upPam.angleDown = ui->dspin_tAngleDown->value();
    upPam.angleUp = ui->dspin_tAngleUp->value();
    upPam.selectRatio = ui->dspin_tSelectRatio->value();
    upPam.xExpand = ui->spin_tXexpan->value();
    upPam.yExpand = ui->spin_tYexpan->value();
    upPam.subThreVal = ui->spin_tSubThreVal->value();
    return upPam;
}

HLineData CGetTapeAngleWidget::getDnPam()
{
    HLineData downPam;
    downPam.angleDown = ui->dspin_bAngleDown->value();
    downPam.angleUp = ui->dspin_bAngleUp->value();
    downPam.selectRatio = ui->dspin_bSelectRatio->value();
    downPam.xExpand = ui->spin_bXexpan->value();
    downPam.yExpand = ui->spin_bYexpan->value();
    downPam.subThreVal = ui->spin_bSubThreVal->value();
    return  downPam;
}

VLineData CGetTapeAngleWidget::getVPam()
{
    VLineData vPam;
    vPam.angleDown = ui->dspin_angDown->value();
    vPam.angleUp = ui->dspin_angleUp->value();
    vPam.dilaWid = ui->spin_dilaWid->value();
    vPam.eroHigh = ui->spin_eroHigh->value();
    vPam.minConLen = ui->spin_minConLen->value();
    vPam.threSubValue = ui->spin_threSubValue->value();
    return vPam;
}

}

void CSHDetect::CGetTapeAngleWidget::on_pushButton_clicked()
{
    try {
        CError err = Algorithm::objIsEmpty(d->m_region.blueTapesReg);
        if (err.isWrong())
            SENDERR("mregion.bluTapes is empty");

        HObject hConnect, res;
        Connection(d->m_region.blueTapesReg, &hConnect);
        d->m_pam->slctRegion(hConnect, res, ui->cmbx_quadrant->currentIndex());

        err = Algorithm::objIsEmpty(res);
        if (err.isWrong())
            return;

        d->m_widget->showObj(res);
        d->slctReg = res;
    }  catch (...) {
        SENDERR("select quadrant crashed!");
    }
}

void CSHDetect::CGetTapeAngleWidget::on_btn_singleDetct_clicked()
{
    try {
        CError err = Algorithm::objIsEmpty(d->m_region.blueTapesReg);
        if (err.isWrong())
            SENDERR("mregion.bluTapes is empty");

        HObject hConnect, res, hLines;
        Connection(d->m_region.blueTapesReg, &hConnect);
        d->m_pam->slctRegion(hConnect, res, ui->cmbx_quadrant->currentIndex());

        HTuple  Row1, Column1, Row2, Column2;
        SmallestRectangle1(res, &Row1, &Column1, &Row2, &Column2);

        HObject rect;
        GenRectangle1(&rect, Row1, Column1, Row2, Column2);
        d->m_widget->showObj(rect);

        GenEmptyObj(&hLines);
        HLineData upPam = getUpPam();
        HLineData downPam = getDnPam();
        VLineData vPam = getVPam();

        QList<QLine> lineList;
        d->m_pam->getSignalLines(d->m_region.m_oriImg, hConnect, lineList, ui->cmbx_quadrant->currentIndex(), upPam, downPam, vPam);

        HTuple showMsg;
        for (auto line : lineList) {
            GenRegionLine(&rect, line.y1(), line.x1(), line.y2(), line.x2());
            Union2(hLines, rect, &hLines);
        }

        QList<QString> stt;
        stt.push_back(QString("tangle:%1").arg(upPam.angle));
        stt.push_back(QString("bangle:%1").arg(downPam.angle));
        stt.push_back(QString("vangle:%1").arg(vPam.angle));

        d->m_widget->showObj(hLines);
        d->m_widget->setFont("Times New Roman", 25, false, false);
        d->m_widget->showMsg(stt, QPoint(50, 100));
    }  catch (...) {
        SENDERR("singleDetct crashed!");
    }
}

void CSHDetect::CGetTapeAngleWidget::on_btn_vDetect_clicked()
{
    CError err = Algorithm::objIsEmpty(d->m_region.blueTapesReg);
    if (err.isWrong())
        SENDERR("mregion.bluTapes is empty");

    HObject hConnect, res, hLine;
    Connection(d->m_region.blueTapesReg, &hConnect);
    d->m_pam->slctRegion(hConnect, res, ui->cmbx_quadrant->currentIndex());

    err = Algorithm::objIsEmpty(res);
    if (err.isWrong())
        SENDERR("slected region is empty");

    HTuple  Row1, Column1, Row2, Column2;
    SmallestRectangle1(res, &Row1, &Column1, &Row2, &Column2);

    QLine line;
    VLineData vPam = getVPam();
    err = d->m_pam->getVline(d->m_region.m_oriImg, res, line, vPam);
    if (err.isWrong())
        QMessageBox::warning(this, "warning", err.msg());

    if ((err.code() == NG) || (!err.isWrong())) {
        GenRegionLine(&hLine, line.y1(), line.x1(), line.y2(), line.x2());
        d->m_widget->showObj(hLine);
        d->m_widget->showMsg(QString("Angle:%1").arg(vPam.angle));
    }
}

void CSHDetect::CGetTapeAngleWidget::on_btn_upDetect_clicked()
{
    try {
        CError err = Algorithm::objIsEmpty(d->m_region.blueTapesReg);
        if (err.isWrong())
            SENDERR("mregion.bluTapes is empty");

        HObject hConnect, res, hLines;
        Connection(d->m_region.blueTapesReg, &hConnect);
        d->m_pam->slctRegion(hConnect, res, ui->cmbx_quadrant->currentIndex());

        err = Algorithm::objIsEmpty(res);
        if (err.isWrong())
            SENDERR("slected region is empty");

        HTuple  Row1, Column1, Row2, Column2;
        SmallestRectangle1(res, &Row1, &Column1, &Row2, &Column2);

        HObject rect, hLine;
        GenRectangle1(&rect, Row1, Column1, Row2, Column2);
        d->m_widget->showObj(rect);

        QLine upLine(Column1.D(), Row1.D(), Column2.D(), Row1.D());
        QLine line = QLine(0, 0, 0, 0);

        GenEmptyObj(&hLines);
        HLineData upPam = getUpPam();
        err = d->m_pam->getHline(d->m_region.m_oriImg, res, line, upLine, upPam);

        if (err.isWrong())
            QMessageBox::warning(this, "warning", err.msg());

        if ((err.code() == NG) || (!err.isWrong())) {
            GenRegionLine(&hLine, line.y1(), line.x1(), line.y2(), line.x2());
            d->m_widget->showObj(hLine);
            d->m_widget->showMsg(QString("Angle:%1").arg(upPam.angle));
        }

    }  catch (...) {
        SENDERR("singleDetct crashed!");
    }
}

void CSHDetect::CGetTapeAngleWidget::on_btn_doenDetect_clicked()
{
    try {
        CError err = Algorithm::objIsEmpty(d->m_region.blueTapesReg);
        if (err.isWrong())
            SENDERR("mregion.bluTapes is empty");

        HObject hConnect, res, hLines;
        Connection(d->m_region.blueTapesReg, &hConnect);
        d->m_pam->slctRegion(hConnect, res, ui->cmbx_quadrant->currentIndex());

        err = Algorithm::objIsEmpty(res);
        if (err.isWrong())
            SENDERR("slected region is empty");

        HTuple  Row1, Column1, Row2, Column2;
        SmallestRectangle1(res, &Row1, &Column1, &Row2, &Column2);

        HObject rect, hLine;
        GenRectangle1(&rect, Row1, Column1, Row2, Column2);
        d->m_widget->showObj(rect);

        QLine dnLine(Column1.D(), Row2.D(), Column2.D(), Row2.D());
        QLine line = QLine(0, 0, 0, 0);

        GenEmptyObj(&hLines);
        HLineData downPam = getDnPam();
        err = d->m_pam->getHline(d->m_region.m_oriImg, res, line, dnLine, downPam);

        if (err.isWrong())
            QMessageBox::warning(this, "warning", err.msg());

        if ((err.code() == NG) || (!err.isWrong())) {
            GenRegionLine(&hLine, line.y1(), line.x1(), line.y2(), line.x2());
            d->m_widget->showObj(hLine);
            d->m_widget->showMsg(QString("Angle:%1").arg(downPam.angle));
        }

    }  catch (...) {
        SENDERR("singleDetct crashed!");
    }
}

void CSHDetect::CGetTapeAngleWidget::on_btn_detect_clicked()
{
    QList<QLine> gdLine, ngLine;
    VLineData vPam = getVPam();
    HLineData upPam = getUpPam();
    HLineData dnPam = getDnPam();

    d->m_pam->setDownPam(dnPam);
    d->m_pam->setUpPam(upPam);
    d->m_pam->setVPam(vPam);
    CError err = d->m_pam->detect(d->m_region.m_oriImg, d->m_region, gdLine, ngLine);
    if (err.isWrong())
        SENDERR(err.msg());

    try {
        HObject hLines, hLine;
        GenEmptyObj(&hLines);
        for (QLine line : gdLine) {
            GenRegionLine(&hLine, line.y1(), line.x1(), line.y2(), line.x2());
            Union2(hLines, hLine, &hLines);
        }
        d->m_widget->showObj(hLines);

    }  catch (...) {
        SENDERR("detect crashed!");
    }
}
