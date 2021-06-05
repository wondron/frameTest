#include "cgetcustrapposwidget.h"
#include "ui_cgetcustrapposwidget.h"
#include "halconGraphic/halconwidget.h"
#include "commonUsage/algorithm.h"
#include "qmessagebox.h"

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
    ui->cmb_direction->setCurrentIndex(pam->strapDirection);
    ui->spin_thre->setValue(pam->maxThre);
}

void CGetCuStrapPosWidget::on_btn_detect_clicked()
{
    if(Algorithm::objIsEmpty(d->m_img))
        SENDERR("image is empty, cannt detect!");

    if(d->m_pam == nullptr)
        SENDERR("pam is emppty");

    int position;
    HObject resObj;

    int index = ui->cmb_direction->currentIndex() == 0 ? d->m_height : d->m_width;

    CError err = d->m_pam->testPos(position, d->m_img, resObj, index, ui->spin_thre->value(),
                                ui->cmb_direction->currentIndex());

    if(err.isWrong())
        SENDERR(err.msg());

    d->m_widget->showObj(resObj);
}

}

