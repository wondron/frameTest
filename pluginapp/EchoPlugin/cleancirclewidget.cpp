#include "cleancirclewidget.h"
#include "ui_cleancirclewidget.h"
#include "qfiledialog.h"
#include "qdebug.h"

using namespace HalconCpp;

class cleanCircleWidgetPrivate
{
  public:
    cleanCircleWidgetPrivate(cleanCircleWidget * parent): p(parent)
    {
        halDlg = new Graphics::ImageViewerHalcon();
    }

    ~cleanCircleWidgetPrivate(){
        delete halDlg;
    }

    cleanCircleWidget *p;
    Graphics::ImageViewerHalcon *halDlg;
    HObject img;
    HTuple rowCenter, colCenter, imgWid, imgHit;
};

cleanCircleWidget::cleanCircleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cleanCircleWidget),
    d(new cleanCircleWidgetPrivate(this))
{
    ui->setupUi(this);
    ui->vlay_Image->addWidget(d->halDlg);
    connect(d->halDlg, SIGNAL(pointClicked(int, int)), this, SLOT(recivePoint(int, int)));
}

cleanCircleWidget::~cleanCircleWidget()
{
    delete ui;
}

void cleanCircleWidget::on_btn_getImage_clicked()
{
    QString temPath = QFileDialog::getOpenFileName(this, "open file");
    if (temPath.isEmpty()) return;

    HTuple hv_Exception;
    try {
        ReadImage(&d->img, temPath.toStdString().c_str());
        GetImageSize(d->img, &d->imgWid, &d->imgHit);
    }  catch (HException &HDevExpDefaultException) {
        HDevExpDefaultException.ToHTuple(&hv_Exception);
    }

    if (d->halDlg->objIsEmpty(d->img)) {
        return;
    }
    d->halDlg->setShowImage(d->img);
}

void cleanCircleWidget::on_btn_detect_clicked()
{
    HObject  ho_Circle, ho_Circle1, ho_Rectangle;
    HObject  ho_RegionDifference, ho_RegionIntersection, ho_ImageReduced;
    HObject  ho_Border, ho_SelectedXLD, ho_UnionContours1, ho_UnionContours;
    HObject  ho_SelectedContours, ho_Circle2, ho_Circle3, ho_RegionDifference1, ho_showImage;

    HTuple  hv_Mean, hv_Deviation, hv_StartPhi, hv_EndPhi;
    HTuple  hv_PointOrder, hv_Width, hv_Height, hv_WindowHandle1;
    HTuple  hv_Exception, tempCol, tempRow, tempRadius;
    HTuple hv_CleanCenterRow2D, hv_CleanCenterCol2D, hv_CleanRadius2D;

    try {
        GenCircle(&ho_Circle, ui->spin_row->value(), ui->spin_col->value(), ui->spin_innerR->value());
        GenCircle(&ho_Circle1, ui->spin_row->value(), ui->spin_col->value(), ui->spin_outerR->value());
        GenRectangle2(&ho_Rectangle, ui->spin_row->value(), ui->spin_col->value(), 1.57, ui->spin_outerR->value() - 30,  1000);
        Difference(ho_Circle1, ho_Circle, &ho_RegionDifference);
        Intersection(ho_Rectangle, ho_RegionDifference, &ho_RegionIntersection);
        ReduceDomain(d->img, ho_RegionIntersection, &ho_ImageReduced);
        Intensity(ho_RegionIntersection, ho_ImageReduced, &hv_Mean, &hv_Deviation);

        ThresholdSubPix(ho_ImageReduced, &ho_Border, hv_Mean * 0.7);
        //200
        SelectShapeXld(ho_Border, &ho_SelectedXLD, "area", "and", 200, 9999999);
        //40
        UnionAdjacentContoursXld(ho_SelectedXLD, &ho_UnionContours1, 40, 1, "attr_keep");
        //300
        SelectShapeXld(ho_UnionContours1, &ho_SelectedXLD, "contlength", "and", 300, 9999999);
        UnionAdjacentContoursXld(ho_SelectedXLD, &ho_UnionContours, 10000, 10000, "attr_keep");
        //1500
        SelectContoursXld(ho_UnionContours, &ho_SelectedContours, "contour_length", 1500, 400000, -0.5, 0.5);
        FitCircleContourXld(ho_SelectedContours, "algebraic", -1, 0, 0, 3, 2, &tempRow,
                            &tempCol, &tempRadius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);

        //二次定位
        GenCircle(&ho_Circle, tempRow, tempCol, ui->spin_innerR->value());
        GenCircle(&ho_Circle1, tempRow, tempCol, ui->spin_outerR->value());
        GenRectangle2(&ho_Rectangle, tempRow, tempCol, 1.57, ui->spin_outerR->value() - 40, 1000);
        Difference(ho_Circle1, ho_Circle, &ho_RegionDifference);
        Intersection(ho_Rectangle, ho_RegionDifference, &ho_RegionIntersection);
        ReduceDomain(d->img, ho_RegionIntersection, &ho_ImageReduced);
        Intensity(ho_RegionIntersection, ho_ImageReduced, &hv_Mean, &hv_Deviation);
        ThresholdSubPix(ho_ImageReduced, &ho_Border, hv_Mean * 0.7);
        SelectShapeXld(ho_Border, &ho_SelectedXLD, "area", "and", 200, 9999999);
        UnionAdjacentContoursXld(ho_SelectedXLD, &ho_UnionContours1, 40, 1, "attr_keep");
        SelectShapeXld(ho_UnionContours1, &ho_SelectedXLD, "contlength", "and", 300, 9999999);
        UnionAdjacentContoursXld(ho_SelectedXLD, &ho_UnionContours, 10000, 10000, "attr_keep");
        SelectContoursXld(ho_UnionContours, &ho_SelectedContours, "contour_length", 1500, 200000, -0.5, 0.5);
        hv_CleanCenterRow2D = 0;

        FitCircleContourXld(ho_SelectedContours, "algebraic", -1, 0, 0, 3, 2, &hv_CleanCenterRow2D,
                            &hv_CleanCenterCol2D, &hv_CleanRadius2D, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);

        d->halDlg->setDrawMode(Graphics::ImageViewerHalcon::DrawMode::MARGIN);
        GenCircle(&ho_Circle1, hv_CleanCenterRow2D, hv_CleanCenterCol2D, hv_CleanRadius2D);
        d->halDlg->showObj(d->img, ho_Circle1);

        if (0 != (hv_CleanCenterRow2D > 0)) {
            ui->spin_centerX->setValue(hv_CleanCenterCol2D.D());
            ui->spin_centerY->setValue(hv_CleanCenterRow2D.D());
            ui->spin_radius->setValue(hv_CleanRadius2D.D());
        }
    }
    // catch (Exception)
    catch (HException &HDevExpDefaultException) {
        HDevExpDefaultException.ToHTuple(&hv_Exception);
    }
    return;
}

void cleanCircleWidget::recivePoint(int x, int y)
{
    ui->spin_col->setValue(x);
    ui->spin_row->setValue(y);
}
