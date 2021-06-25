#include "cgetcustrapposwidget.h"
#include "ui_cgetcustrapposwidget.h"
#include "halconGraphic/halconwidget.h"
#include "commonUsage/algorithm.h"
#include "commonUsage/comStruct.h"
#include "qmessagebox.h"
#include "qdebug.h"

using namespace HalconCpp;
using namespace CSHDetect;

#define SENDERR(a) \
    {QMessageBox::warning(this, "warnning", a);\
    return;}

namespace CSHDetect
{

class CGetCuStrapPosWidgetPrivate
{
public:
    CGetCuStrapPosWidgetPrivate(){
        m_widget = new Graphics::halconWidget();
        m_pam = new CGetCuStrapPos();
    }

    int m_width;
    int m_height;

    RegionS m_region;
    Graphics::halconWidget* m_widget;
    HObject m_img;
    CGetCuStrapPos* m_pam;
};

CGetCuStrapPosWidget::CGetCuStrapPosWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CGetCuStrapPosWidget),
    d(new CGetCuStrapPosWidgetPrivate())
{
    ui->setupUi(this);

    ui->lay_widget->addWidget(d->m_widget);
}

CGetCuStrapPosWidget::~CGetCuStrapPosWidget()
{
    delete ui;
}

void CGetCuStrapPosWidget::setRegions(const RegionS &region)
{
    d->m_region = region;

    d->m_img = region.m_oriImg;
    d->m_width = region.width;
    d->m_height = region.height;

    d->m_widget->setShowImage(d->m_region.m_oriImg);
}

void CGetCuStrapPosWidget::setImage(const HObject &obj)
{
    if(Algorithm::objIsEmpty(obj)) return;

    d->m_img = obj.Clone();
    d->m_widget->setShowImage(obj);

    try {
        HTuple width, height;
        GetImageSize(d->m_img, &width, &height);
        d->m_width = width.I();
        d->m_height = height.I();
    }  catch (...) {
        QMessageBox::warning(this, "warnning", "get image size error");
    }
}

void CGetCuStrapPosWidget::setPam(CGetCuStrapPos *pam)
{
    d->m_pam = pam;
}

void CGetCuStrapPosWidget::on_btn_detect_clicked()
{
    QVector<QRect> ngInfo, gdInfo;

    d->m_pam->pamRead("D:\\work\\detect\\detection\\bin\\conf\\algorithm\\default\\detection.xml");
    CError err = d->m_pam->detect(d->m_region, gdInfo, ngInfo);

    qDebug() << gdInfo.size() << ngInfo.size();
    try {
        HObject hRect, hUnion;
        GenEmptyObj(&hUnion);
        for(auto resRec : gdInfo){
            GenRectangle1(&hRect, resRec.top(), resRec.left(), resRec.bottom(), resRec.right());
            Union2(hUnion, hRect, &hUnion);
        }
        for(auto resRec : ngInfo){
            GenRectangle1(&hRect, resRec.top(), resRec.left(), resRec.bottom(), resRec.right());
            Union2(hUnion, hRect, &hUnion);
        }

        d->m_widget->showObj(hUnion);

    }  catch (...) {
        SENDERR("detect crashed!");
    }
}


void CGetCuStrapPosWidget::on_btn_getMidRegion_clicked()
{
    HObject midRegion;
    CError err = d->m_pam->getMidRect(d->m_region, midRegion, ui->cmb_direction->currentIndex());
    if(err.isWrong())
        SENDERR(err.msg());

    d->m_widget->showObj(midRegion);
}


void CGetCuStrapPosWidget::on_btn_getCuPos_clicked()
{
    HObject midRegion, cuRegion;
    CError err = d->m_pam->getMidRect(d->m_region, midRegion, ui->cmb_direction->currentIndex());
    if(err.isWrong())
        SENDERR(err.msg());

    int cuPos;
    err = d->m_pam->getCuPos(d->m_region, midRegion, cuRegion, cuPos, ui->spin_minthre->value(),ui->cmb_direction->currentIndex());
    if(err.isWrong())
        SENDERR(err.msg());

    d->m_widget->showObj(cuRegion);
    ui->cmb_cuPos->setCurrentIndex(cuPos);
}

}


void CSHDetect::CGetCuStrapPosWidget::on_btn_pamRead_clicked()
{
    d->m_pam->pamRead("D:\\work\\detect\\detection\\bin\\conf\\algorithm\\cshdefault\\detection.xml");
}
