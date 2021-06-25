#include "ccheckhanregwidget.h"
#include "ui_ccheckhanregwidget.h"
#include "halconGraphic/halconwidget.h"
#include "qmessagebox.h"
#include "commonUsage/algorithm.h"
#include "commonUsage/commonDef.h"
#include "qdebug.h"

#define SENDERR(a) \
    {QMessageBox::warning(this, "warnning", a);\
    return 1;}

namespace CSHDetect {

class CCheckHanRegWidgetPrivate
{
public:
    CCheckHanRegWidgetPrivate()
    {
        m_widget = new Graphics::halconWidget();
        m_pam = new CCheckHanReg();
    }
    Graphics::halconWidget* m_widget;
    ReverRegionS m_region;
    CCheckHanReg* m_pam;

    HObject dynImg;

    bool isSetRegion = false;
};

CCheckHanRegWidget::CCheckHanRegWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CCheckHanRegWidget),
    d(new CCheckHanRegWidgetPrivate())
{
    ui->setupUi(this);
    ui->vlay_widget->addWidget(d->m_widget);

    connect(ui->spin_dynkernel, SIGNAL(valueChanged(int)), this, SLOT(checkDyn()));
    connect(ui->spin_dynThr, SIGNAL(valueChanged(int)), this, SLOT(checkDyn()));
}

CCheckHanRegWidget::~CCheckHanRegWidget()
{
    delete ui;
}

CError CCheckHanRegWidget::setRegions(const ReverRegionS& region)
{
    CError err = Algorithm::objIsEmpty(region.midRegion);

    if (err.isWrong())
        SENDERR("CCheckHanRegWidget::setRegions input region is empty");

    d->m_region = region;
    d->isSetRegion = true;
    d->m_widget->setShowImage(region.m_oriImg);

    return 0;
}

ReverRegionS &CCheckHanRegWidget::getRegions()
{
    return d->m_region;
}

void CCheckHanRegWidget::setPam(CCheckHanReg* pam)
{
    d->m_pam = pam;
}

CError CCheckHanRegWidget::detect()
{
    if (!d->isSetRegion)
        SENDERR("not set regions");

    HObject hanReg;
    QList<QRect> rects;
    int hanHolSize = ui->spin_hanHoleSize->value();
    int dynThr = ui->spin_dynThr->value();
    int dynKer = ui->spin_dynkernel->value();
    int hanSize = ui->spin_hanSize->value();
    int maxArea = ui->spin_maxArea->value();

    CError err = d->m_pam->getHanRegion(d->m_region.m_oriImg, d->m_region.midRegion, d->m_region.hanregion,
                                        hanHolSize, dynThr, dynKer, maxArea, hanSize, rects);
    if (err.isWrong())
        SENDERR(err.msg());

    emit detectDone();
    d->m_widget->showObj(d->m_region.hanregion);

    return 0;
}

void CCheckHanRegWidget::on_btn_detect_clicked()
{
    detect();
}

CError CCheckHanRegWidget::checkDyn()
{
    try {
        HObject dst, doubleTapeRoi, ImageReduced, RegionDynThresh;

        dst = d->m_region.m_oriImg;
        doubleTapeRoi = d->m_region.dblTapeReg;

        CError err = Algorithm::objIsEmpty(doubleTapeRoi);
        if (err.isWrong())
            SENDERR("dblTapeReg is empty");

        err = Algorithm::objIsEmpty(dst);
        if (err.isWrong())
            SENDERR("m_oriImg is empty");

        int hankernel = ui->spin_dynkernel->value();
        int dynVal = ui->spin_dynThr->value();

        ReduceDomain(dst, doubleTapeRoi, &ImageReduced);
        RegionDynThresh = Algorithm::dynThre(ImageReduced, dynVal, hankernel);

        d->m_widget->showObj(RegionDynThresh);
        d->dynImg = RegionDynThresh;

        return 0;

    }  catch (...) {
        SENDERR(" application crashed! ");
    }
}

}

