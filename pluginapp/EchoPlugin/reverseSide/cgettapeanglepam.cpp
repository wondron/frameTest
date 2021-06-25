#include "cgettapeanglepam.h"
#include "ui_cgettapeanglepam.h"
#include "qdebug.h"
namespace CSHDetect {

CGetTapeAnglePam::CGetTapeAnglePam(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CGetTapeAnglePam)
{
    ui->setupUi(this);
}

CGetTapeAnglePam::~CGetTapeAnglePam()
{
    delete ui;
}

void CGetTapeAnglePam::on_btn_getPam_clicked()
{
    if(sendSignal)
        emit pamChanged();
}


void CSHDetect::CGetTapeAnglePam::getPamValue(MeasureposPam& pam, PosBia& posBia)
{
    pam.pntSelect = ui->cmbx_slctPnt->currentIndex();
    pam.recLen1 = ui->spin_len1->value();
    pam.recLen2 = ui->spin_len2->value();
    pam.sigma = ui->spin_sigma->value();
    pam.threshold = ui->spin_threshold->value();
    pam.transition = ui->cmbx_transition->currentIndex();

    posBia.XBia = ui->spin_XBia->value();
    posBia.YBia = ui->spin_YBia->value();
    posBia.lineWidth = ui->spin_lineWid->value();
    posBia.lineDirect = ui->cmbx_direct->currentIndex();
    posBia.detectTime = ui->spin_dtctTime->value();
    posBia.angleDown = ui->spin_angleDn->value();
    posBia.angleUp = ui->spin_angleUp->value();

    //qDebug() << posBia.XBia << posBia.YBia  << posBia.angleDown  << posBia.angleUp  << posBia.detectTime  << posBia.lineDirect  << posBia.lineWidth;
    //qDebug() << pam.pntSelect << pam.recLen1 << pam.recLen2 << pam.sigma << pam.threshold << pam.transition;
}

void CGetTapeAnglePam::setPamValue(const MeasureposPam& pam, const PosBia& posBia)
{
    sendSignal = false;
    ui->cmbx_slctPnt->setCurrentIndex(pam.pntSelect);
    ui->spin_len1->setValue(pam.recLen1);
    ui->spin_len2->setValue(pam.recLen2);
    ui->spin_sigma->setValue(pam.sigma);
    ui->spin_threshold->setValue(pam.threshold );
    ui->cmbx_transition->setCurrentIndex(pam.transition);

    ui->spin_XBia->setValue(posBia.XBia);
    ui->spin_YBia->setValue(posBia.YBia );
    ui->spin_lineWid->setValue(posBia.lineWidth);
    ui->cmbx_direct->setCurrentIndex(posBia.lineDirect);
    ui->spin_dtctTime->setValue(posBia.detectTime);
    ui->spin_angleDn->setValue(posBia.angleDown);
    ui->spin_angleUp->setValue(posBia.angleUp);
    sendSignal = true;
}

}

