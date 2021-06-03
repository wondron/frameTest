#include "totaltestwidget.h"
#include "ui_totaltestwidget.h"
#include "../FrameWork/commonWidget/citemlistwidget.h"
#include "qfiledialog.h"
#include "qdebug.h"
#include "obverseSide/cgetregionswidget.h"
#include "obverseSide/cgetcustrapposwidget.h"
#include "obverseSide/cgetcirclelenwidget.h"
#include "commonUsage/commonDef.h"
#include "commonUsage/comStruct.h"
#include "commonUsage/cerror.h"
#include "qmessagebox.h"
#include "imagedetectbase.h"

#define SENDERR(a) \
    {QMessageBox::warning(this, "warnning", a);\
    return 1;}

class totalTestWidgetPrivate
{
public:
    totalTestWidgetPrivate(){
        m_widget = new CItemListWidget();
        m_regionsWidget = new CGetRegionsWidget();
        m_cuWidget = new CGetCuStrapPosWidget();
        m_circleLenWidget = new CGetCircleLenWidget();
        m_pam = new CObverseSideDetect();
        m_testWidget = new ImageDetectBase();
    }

    CItemListWidget* m_widget;
    CGetCuStrapPosWidget* m_cuWidget;
    CGetRegionsWidget * m_regionsWidget;
    CGetCircleLenWidget* m_circleLenWidget;
    CObverseSideDetect* m_pam;
    ImageDetectBase* m_testWidget;

    HObject m_img;
    HTuple m_width;
    HTuple m_height;
    RegionS regions;
};

totalTestWidget::totalTestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::totalTestWidget),
    d(new totalTestWidgetPrivate())
{
    ui->setupUi(this);
    ui->lay_imgList->addWidget(d->m_widget);
    ui->layout_operator->addWidget(d->m_regionsWidget);
    ui->lay_CuPos->addWidget(d->m_cuWidget);
    ui->lay_circleLen->addWidget(d->m_circleLenWidget);
    connect(d->m_widget, SIGNAL(currentFileName(QString)), this, SLOT(getFilename(QString)));
    connect(d->m_regionsWidget, SIGNAL(detectDone()), this, SLOT(OnPamSetDetectDone()));
}

totalTestWidget::~totalTestWidget()
{
    delete ui;
}

void totalTestWidget::setPam(CObverseSideDetect *pam)
{
    pam = d->m_pam;

    d->m_cuWidget->setPam(d->m_pam->m_grpCuPos);
    d->m_circleLenWidget->setPam(d->m_pam->m_grpCircleLen);
    d->m_regionsWidget->setPam(d->m_pam->m_grpRegions);
}

void totalTestWidget::on_btn_getFilePath_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, "select the filepath");

    d->m_widget->setFilter("*.bmp *.jpg *.png");
    d->m_widget->setFilePath(directory);
}

void totalTestWidget::getFilename(const QString &file)
{
    if (file.isEmpty()) return;

    int err = getImage(file);
    if(err) return;

    d->m_regionsWidget->setImage(d->m_img, d->regions);
    d->m_cuWidget->setImage(d->regions.m_hsvHImg);
}

void totalTestWidget::OnPamSetDetectDone()
{
    d->regions = d->m_regionsWidget->getRegons();
    d->m_circleLenWidget->setRegions(d->regions);
}

int totalTestWidget::getImage(const QString &file)
{
    try {
        ui->tabWidget->setCurrentIndex(0);
        //ReadImage(&d->m_img, file.toStdString().c_str());
        ReadImage(&d->m_img, file.toLocal8Bit().data());
        GetImageSize(d->m_img, &d->m_width, &d->m_height);

        CError err = Algorithm::objIsEmpty(d->m_img);
        if(err.isWrong())
            SENDERR(err.msg());

        Decompose3(d->m_img, &d->regions.m_RImg,
                   &d->regions.m_GImg, &d->regions.m_BImg);

        TransFromRgb(d->regions.m_RImg, d->regions.m_GImg,
                     d->regions.m_BImg, &d->regions.m_hsvHImg,
                     &d->regions.m_hsvSImg, &d->regions.m_hsvVImg, "hsv");

        return 0;
    }  catch (...) {
        SENDERR("getImage err");
    }
}

void totalTestWidget::on_btn_loadPam_clicked()
{
    CError err;
    err = d->m_pam->PamRead("D:\\work\\detect\\detection\\bin\\conf\\algorithm\\cshdefault\\detection.xml");
    setPam(d->m_pam);
    qDebug()<<"err:"<<err.msg();
}

void totalTestWidget::on_btn_detect_clicked()
{
    d->m_pam->detect(d->m_img);
    //qDebug()<<"err:"<< err.msg();
}

void totalTestWidget::on_btn_algoTest_clicked()
{
    d->m_testWidget->show();
}
