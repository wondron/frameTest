#include "cgetcirclelenwidget.h"
#include "ui_cgetcirclelenwidget.h"
#include "halconGraphic/halconwidget.h"
#include "qmessagebox.h"
#include "commonUsage/algorithm.h"
#include "commonUsage/commonDef.h"
#include "qdebug.h"

#define SENDERR(a) \
    {QMessageBox::warning(this, "warnning", a);\
    return;}

namespace CSHDetect{

class CGetCircleLenWidgetPrivate
{
public:
    CGetCircleLenWidgetPrivate(){
        m_widget = new Graphics::halconWidget();
        m_pam = new CGetCircleLen();

        m_center = QPointF(0, 0);
    }
    Graphics::halconWidget *m_widget;
    RegionS m_region;
    CGetCircleLen* m_pam;
    QPointF m_center;

    CircleInfo m_smallInfo;
    CircleInfo m_bigInfo;

    bool isSetRegion = false;
};

CGetCircleLenWidget::CGetCircleLenWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CGetCircleLenWidget),
    d(new CGetCircleLenWidgetPrivate())
{
    ui->setupUi(this);
    ui->lay_widget->addWidget(d->m_widget);
}

CGetCircleLenWidget::~CGetCircleLenWidget()
{
    delete ui;
}

void CGetCircleLenWidget::setRegions(const RegionS &region)
{
    CError err = Algorithm::objIsEmpty(region.m_RImg);
    if(err.isWrong())
        SENDERR("input regions is empty");

    d->m_region = region;
    d->isSetRegion = true;

    d->m_widget->setShowImage(region.m_oriImg);
}

void CGetCircleLenWidget::setPam(CGetCircleLen *pam)
{
    d->m_pam = pam;
}

void CGetCircleLenWidget::on_btn_circleDetect_clicked()
{
    if(!d->isSetRegion)
        SENDERR("not set the regions");


    CError err = d->m_pam->getCircleCenter(d->m_region, d->m_smallInfo, d->m_bigInfo,
                              ui->spin_RingRadius->value(), ui->spin_dilation->value(),
                              ui->cmb_direction->currentIndex());
    if(err.isWrong())
        SENDERR(err.msg());

    if(d->m_smallInfo.X == 0)
        SENDERR("click circle test first");

    HObject smallC, bigC, totalC;
    try {
        GenCircle(&smallC, d->m_smallInfo.Y, d->m_smallInfo.X, d->m_smallInfo.Radius);
        GenCircle(&bigC, d->m_bigInfo.Y, d->m_bigInfo.X, d->m_bigInfo.Radius);
        Union2(smallC, bigC, &totalC);

        d->m_widget->showObj(d->m_region.m_oriImg, totalC);
    }  catch (...) {
        SENDERR("circleDetect err, happen unexcepted");
    }
}

void CGetCircleLenWidget::on_btn_lineDetect_clicked()
{
    if(!d->isSetRegion)
        SENDERR("not set the regions");

    try {
        LineInfo line;
        HObject showObj;
        CError err = d->m_pam->getDetectLine(d->m_region, showObj, line,
                                             ui->cmb_quadrantPos->currentIndex(),
                                             ui->cmb_direction->currentIndex());
        if(err.isWrong())
            SENDERR(err.msg());

        HObject lineHal, lineCross;
        GenRegionLine(&lineHal, line.startRow, line.startCol, line.endRow, line.endCol);
        d->m_widget->showObj(d->m_region.m_oriImg, lineHal);

        qreal X, Y;
        if(ui->cmb_quadrantPos->currentIndex() == 1 || ui->cmb_quadrantPos->currentIndex() == 2){
            X = d->m_smallInfo.X;
            Y = d->m_smallInfo.Y;
        } else {
            X = d->m_bigInfo.X;
            Y = d->m_bigInfo.Y;
        }

        QPointF pts = Algorithm::getCrossPoint(X, Y, line.startCol, line.startRow, line.endCol, line.endRow);
        GenRegionLine(&lineCross, Y, X, pts.y(), pts.x());
        Union2(lineHal, lineCross, &lineCross);
        Union2(lineCross, showObj, &lineCross);
        d->m_widget->showObj(d->m_region.m_oriImg, lineCross);
    }  catch (...) {
        SENDERR("lineDetect err, happen unexcepted");
    }
}

void CGetCircleLenWidget::on_btn_test_clicked()
{
    QList<LineInfo> info;
    if(!d->isSetRegion)
        SENDERR("not set the regions");

    CError err = d->m_pam->getLineInfo(d->m_region, info);
    if(err.isWrong()) SENDERR(err.msg());

    try {
        int index = 0;
        HObject line, lines;
        GenEmptyObj(&lines);
        for(auto i : info){
            qDebug()<< index++;
            qDebug()<<"i:"<<i.startRow.D()<< i.startCol.D()<< i.endRow.D()<< i.endCol.D();
            GenRegionLine(&line, i.startRow, i.startCol, i.endRow, i.endCol);
            Union2(lines, line, &lines);
            qDebug()<< index++;
            GenRegionLine(&line, i.pY, i.pX, i.crossY, i.crossX);
            Union2(lines, line, &lines);
        }

        d->m_widget->showObj(lines);
    }  catch (...) {
        qDebug()<<"test happen unexcepted";
    }
}
}
