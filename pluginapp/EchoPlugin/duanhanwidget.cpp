#include "duanhanwidget.h"
#include "ui_duanhanwidget.h"
#include "imageviewerhalcon.h"
#include "halconcpp/HalconCpp.h"
#include "qdebug.h"
#include "qfiledialog.h"

using namespace HalconCpp;
class duanhanWidgetPrivate
{
public:
    duanhanWidgetPrivate(duanhanWidget *p)
    {
        parent = p;
        halDlg = new Graphics::ImageViewerHalcon();
    }

public:
    Graphics::ImageViewerHalcon *halDlg;
    duanhanWidget *parent;
    HTuple rowCenter, colCenter, imgWid, imgHit;
    HObject img, roiImage;
    HObject obj;
};

duanhanWidget::duanhanWidget(QWidget *parent) :
    QWidget(parent),
    d(new duanhanWidgetPrivate(this)),
    ui(new Ui::duanhanWidget)
{
    ui->setupUi(this);
    initialWindow();
}

duanhanWidget::~duanhanWidget()
{
    delete ui;
}

void duanhanWidget::initialWindow()
{
    ui->hlay_src->addWidget(d->halDlg);
}

bool duanhanWidget::genRingRoi(HalconCpp::HObject &obj)
{
    qDebug() << "enter Roi";
    int BR = ui->spin_bigR->value();
    int SR = ui->spin_smallR->value();

    HObject circleSmall, circleBig;

    if (SR >= BR) return false;
    GenCircle(&circleSmall, d->rowCenter, d->colCenter, SR);
    GenCircle(&circleBig, d->rowCenter, d->colCenter, BR);
    Difference(circleBig, circleSmall, &obj);

    ReduceDomain(d->img, obj, &d->roiImage);
    d->halDlg->showObj(d->img, obj);
    return true;
}

void duanhanWidget::on_btn_getImg_clicked()
{
    QString temPath = QFileDialog::getOpenFileName(this, "open file");
    if (temPath.isEmpty()) return;

    HTuple hv_Exception;
    try {
        ReadImage(&d->img, temPath.toStdString().c_str());
        GetImageSize(d->img, &d->imgWid, &d->imgHit);
    }  catch (HException &HDevExpDefaultException) {
        HDevExpDefaultException.ToHTuple(&hv_Exception);
        qDebug() << "read image error";
    }

    if (d->halDlg->objIsEmpty(d->img)) {
        qDebug() << "open a empty obj";
        return;
    }
    d->halDlg->setShowImage(d->img);
}

void duanhanWidget::on_btn_detect_clicked()
{
    HObject roi;
    HTuple area, hv_Exception;
    HObject threRegion, connectRegion, slctRegion, transRegion;
    HTuple num;

    try {
        Threshold(d->img, &threRegion, (HTuple)ui->spin_threMin->value(), (HTuple)ui->spin_threMax->value());
        d->halDlg->showObj(d->img, threRegion);

        Connection(threRegion, &connectRegion);

        SelectShape(connectRegion, &slctRegion, (HTuple("area").Append("row1")),
                    "and", (HTuple(80076).Append(400)), (HTuple(500000).Append(874.38)));

        ShapeTrans(slctRegion, &transRegion, "outer_circle");

        CountObj(transRegion, &num);
        d->halDlg->showObj(d->img, transRegion);
        AreaCenter(slctRegion, &area, &d->rowCenter, &d->colCenter);
    }  catch (HException &HDevExpDefaultException) {
        HDevExpDefaultException.ToHTuple(&hv_Exception);
    }
}

void duanhanWidget::on_btn_lapuDetect_clicked()
{
    HObject RoiRegion, laplaceImage, threRegion;
    HObject closeRegion, openRegion, connectRegion;

    bool ret = genRingRoi(RoiRegion);
    if (!ret) return;

    Laplace(d->roiImage, &laplaceImage, "absolute", 3, "n_8_isotropic");
    Threshold(laplaceImage, &threRegion, ui->spin_laputhreMin->value(), ui->spin_laputhreMax->value());
    ClosingCircle(threRegion, &closeRegion, 15);
    OpeningCircle(closeRegion, &openRegion, 4);
    Connection(openRegion, &connectRegion);
    SelectShape(connectRegion, &d->obj, "area", "or", 10558.44, 1000000);
    d->halDlg->showObj(d->roiImage, d->obj);
}

void duanhanWidget::on_btn_polarChange_clicked()
{
    HObject XYTransRegion, polarTransRegion, connectRegion, transRegion;

    int bigR = ui->spin_bigR->value();
    int smallR = ui->spin_smallR->value();

    PolarTransRegion(d->obj, &polarTransRegion, d->rowCenter, d->colCenter,
                     -1, 5.28, ui->spin_bigR->value(), ui->spin_smallR->value(), 1400, bigR - smallR, "nearest_neighbor");
    Connection(polarTransRegion, &connectRegion);
    ShapeTrans(connectRegion, &transRegion, "rectangle1");
    SelectShape(transRegion, &d->obj, "width", "or", ui->spin_minWid->value(), 50000);
    PolarTransRegionInv(d->obj, &XYTransRegion, d->rowCenter, d->colCenter, -1,
                        5.28, bigR, smallR, 1400, bigR - smallR, d->imgWid, d->imgHit, "nearest_neighbor");


    d->halDlg->showObj(d->roiImage, XYTransRegion);
}
