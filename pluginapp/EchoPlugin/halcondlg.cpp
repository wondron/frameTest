#include "halcondlg.h"
#include "ui_halcondlg.h"
#include "QWheelEvent"
#include "qdebug.h"
#include "qfiledialog.h"
//#include "halcon/include/HalconCpp.h"

#define ZOOMRATIO 2.0

class halconDlgPrivate
{
public:
    halconDlgPrivate(halconDlg *parent): p(parent)
    {}

    halconDlg *p;
    HTuple windowHandle;
    HObject img;
    HObject obj;
    HObject roiImage;

    //中心点保存
    HTuple rowCenter, colCenter, area;
    HTuple imgWid, imgHeight;
    duanhanGroup *group;

    //鼠标事件的位置
    HTuple m_tMouseDownRow;
    HTuple m_tMouseDownCol;
    bool m_bIsMove;
};

halconDlg::halconDlg(QWidget *parent) :
    QDialog(parent),
    d(new halconDlgPrivate(this)),
    ui(new Ui::halconDlg)
{
    ui->setupUi(this);
    ui->label_Img->setObjectName("labelImage");
    ui->label_Img->installEventFilter(this);

    d->group = new duanhanGroup();
    Hlong windId = (Hlong)ui->label_Img->winId();
    OpenWindow(0, 0, ui->label_Img->geometry().width(), ui->label_Img->geometry().height(), windId, "visible", "", &d->windowHandle);
    SetColor(d->windowHandle, "red");
}

halconDlg::~halconDlg()
{
    delete ui;
}

void halconDlg::setShowImage(HObject obj)
{
    d->img = obj.Clone();
}

bool halconDlg::getRegion(HObject img, HObject &obj, HObject &roiImage)
{
    return false;
}

void halconDlg::showObj(HObject img, HObject obj)
{
    ClearWindow(d->windowHandle);
    DispObj(img, d->windowHandle);
    DispObj(obj, d->windowHandle);
}

bool halconDlg::genRingRoi(HObject &obj)
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
    showObj(d->img, obj);
    return true;
}

void halconDlg::wheelEvent(QWheelEvent *ev)
{
    double Zoom;
    HTuple mouseRow, mouseCol, Button;
    HTuple startRowBf, startColBf, endRowBf, endColBf, Ht, Wt, startRowAft, startColAft, endRowAft, endColAft;

    //if(d->img.)

    if (ev->delta() > 0) {
        Zoom = ZOOMRATIO;
    } else {
        Zoom = 1 / ZOOMRATIO;
    }

    bool in = ui->label_Img->rect().contains(ev->pos());
    if (!in) return;

    SetCheck("~give_error");
    GetMposition(d->windowHandle, &mouseRow, &mouseCol, &Button);

    GetPart(d->windowHandle, &startRowBf, &startColBf, &endRowBf, &endColBf);
    Ht = endRowBf - startRowBf;
    Wt = endColBf - startColBf;

    if ((Ht * Wt < 20000 * 20000 || Zoom == ZOOMRATIO)) {
        //计算缩放后的图像区域
        startRowAft = mouseRow - ((mouseRow - startRowBf) / Zoom);
        startColAft = mouseCol - ((mouseCol - startColBf) / Zoom);

        endRowAft = startRowAft + (Ht / Zoom);
        endColAft = startColAft + (Wt / Zoom);

        //如果放大过大，则返回
        if (endRowAft - startRowAft < 2) {
            return;
        }

        if (d->windowHandle != NULL) {
            //如果有图像，则先清空图像
            DetachBackgroundFromWindow(d->windowHandle);
        }
        SetPart(d->windowHandle, startRowAft, startColAft, endRowAft, endColAft);
        AttachBackgroundToWindow(d->img, d->windowHandle);
    }
    SetCheck("give_error");
}

void halconDlg::mousePressEvent(QMouseEvent *ev)
{
    bool in = ui->label_Img->rect().contains(ev->pos());
    if (!in) return;

    HTuple mouseRow, mouseCol, Button;
    SetCheck("~give_error");
    GetMposition(d->windowHandle, &mouseRow, &mouseCol, &Button);
    SetCheck("give_error");

    //鼠标按下时的行列坐标
    d->m_tMouseDownRow = mouseRow;
    d->m_tMouseDownCol = mouseCol;

    d->m_bIsMove = true;
}

void halconDlg::mouseReleaseEvent(QMouseEvent *ev)
{
    d->m_bIsMove = false;
}

void halconDlg::mouseMoveEvent(QMouseEvent *ev)
{
    bool in = ui->label_Img->rect().contains(ev->pos());
    ui->label->setText(("position:( - , - )  gray： -  "));
    if (!in) return;

    HTuple startRowBf, startColBf, endRowBf, endColBf, mouseRow, mouseCol, Button;

    //获取当前鼠标在原图的位置
    SetCheck("~give_error");
    GetMposition(d->windowHandle, &mouseRow, &mouseCol, &Button);
    SetCheck("give_error");

    if (d->m_bIsMove) {
        //计算移动值
        double RowMove = mouseRow[0].D() - d->m_tMouseDownRow[0].D();
        double ColMove = mouseCol[0].D() - d->m_tMouseDownCol[0].D();

        //得到当前的窗口坐标
        GetPart(d->windowHandle, &startRowBf, &startColBf, &endRowBf, &endColBf);

        //移动图像
        if (d->windowHandle != NULL) {
            //如果有图像，则先清空图像
            DetachBackgroundFromWindow(d->windowHandle);
        }
        SetPart(d->windowHandle, startRowBf - RowMove, startColBf - ColMove, endRowBf - RowMove, endColBf - ColMove);
        AttachBackgroundToWindow(d->img, d->windowHandle);
        //showObj(d->img, d->obj);
    }

    HTuple pointGray;
    SetCheck("~give_error");
    GetGrayval(d->img, mouseRow, mouseCol, &pointGray);
    SetCheck("give_error");

    //当光标不在Halcon窗口内时返回，否则会报错
    ui->label->setText(QString("position( %1 , %2 )  gray: %3 ").arg(mouseCol[0].D()).arg(mouseRow[0].D()).arg(pointGray[0].D()));
    emit pointClicked(mouseCol[0].D(), mouseRow[0].D());
}

bool halconDlg::eventFilter(QObject *obj, QEvent *event)
{
    if (obj->objectName() != "labelImage")
        return false;

    if (event->type() == 14) {
        SetWindowExtents(d->windowHandle, 0, 0, ui->label_Img->geometry().width(), ui->label_Img->geometry().height());
        return false;
    }
}

void halconDlg::on_btn_getImg_clicked()
{
    QString temPath = QFileDialog::getOpenFileName(this, "open file");

    if (temPath.isEmpty()) return;

    qDebug() << "temPath" << temPath;
    ReadImage(&d->img, temPath.toStdString().c_str());
    GetImageSize(d->img, &d->imgHeight, &d->imgWid);
    SetPart(d->windowHandle, 0, 0, d->imgHeight, d->imgWid);
    DispObj(d->img, d->windowHandle);
}

void halconDlg::on_btn_detect_clicked()
{
    HObject roi;
    HObject threRegion, connectRegion, slctRegion, transRegion;
    Threshold(d->img, &threRegion, (HTuple)ui->spin_threMin->value(), (HTuple)ui->spin_threMax->value());
    DispObj(threRegion, d->windowHandle);
    Connection(threRegion, &connectRegion);
    SelectShape(connectRegion, &slctRegion, (HTuple("area").Append("row1")),
                "and", (HTuple(80076).Append(400)), (HTuple(500000).Append(874.38)));
    ShapeTrans(slctRegion, &transRegion, "outer_circle");
    HTuple num;
    CountObj(transRegion, &num);
    qDebug() << "num:" << num[0].I();
    showObj(d->img, transRegion);
    AreaCenter(slctRegion, &d->area, &d->rowCenter, &d->colCenter);
}

void halconDlg::on_btn_showRegion_clicked()
{
    showObj(d->img, d->obj);
}

void halconDlg::on_spin_threMin_valueChanged(int arg1)
{
    Threshold(d->img, &d->obj, (HTuple)arg1, (HTuple)ui->spin_threMax->value());
    showObj(d->img, d->obj);
}

void halconDlg::on_spin_smallR_valueChanged(int arg1)
{
    HObject obj;
    genRingRoi(obj);
}

void halconDlg::on_spin_bigR_valueChanged(int arg1)
{
    HObject obj;
    genRingRoi(obj);
}

void halconDlg::on_btn_lapuDetect_clicked()
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
    showObj(d->roiImage, d->obj);
}

void halconDlg::on_spin_laputhreMin_valueChanged(int arg1)
{
    HObject laplaceImage;
    HObject selectRoi;
    Laplace(d->roiImage, &laplaceImage, "absolute", 3, "n_8_isotropic");
    Threshold(laplaceImage, &selectRoi, arg1, 255);
    showObj(d->roiImage, selectRoi);
}

void halconDlg::on_btn_polarChange_clicked()
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
                        5.28, bigR, smallR, 1400, bigR - smallR, d->imgWid, d->imgHeight, "nearest_neighbor");

    showObj(d->roiImage, XYTransRegion);
}

void halconDlg::on_spin_minWid_valueChanged(int arg1)
{
    HTuple number;
    HObject polarTransRegion, connectRegion, transRegion;
    HObject selectRegion;

    int bigR = ui->spin_bigR->value();
    int smallR = ui->spin_smallR->value();

    CountObj(d->obj, &number);
    qDebug() << "num:" << number.I();
    if (number.I() < 1) return;

    PolarTransRegion(d->obj, &polarTransRegion, d->rowCenter, d->colCenter,
                     -1, 5.28, ui->spin_bigR->value(), ui->spin_smallR->value(), 1400, bigR - smallR, "nearest_neighbor");
    Connection(polarTransRegion, &connectRegion);
    ShapeTrans(connectRegion, &transRegion, "rectangle1");
    SelectShape(transRegion, &selectRegion, "width", "or", arg1, 50000);
    CountObj(selectRegion, &number);
    showObj(d->roiImage, selectRegion);
}
