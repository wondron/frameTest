#include "halconwidget.h"
#include "ui_halconwidget.h"

#include "QWheelEvent"
#include "qdebug.h"

namespace Graphics {

using namespace HalconCpp;

#define ZOOMRATIO 2.0

class halconWidgetPrivate
{
public:
    halconWidgetPrivate(halconWidget *parent) : p(parent)
    {

    }

    halconWidget* p;
    HTuple windowHandle;
    HObject img;
    HObject obj;
    HObject roiImage;
    QString msg;

    int fontSize = 16;

    //图像长宽
    HTuple imgWid;
    HTuple imgHit;

    //图像视口位置
    HTuple row1, row2, col1, col2;

    //鼠标事件的位置
    HTuple m_tMouseDownRow;
    HTuple m_tMouseDownCol;
    bool m_bIsMove;
};



halconWidget::halconWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::halconWidget),
    d(new halconWidgetPrivate(this))
{
    ui->setupUi(this);

    ui->label_Img->setObjectName("labelImage");
    ui->label_Img->installEventFilter(this);

    Hlong windId = (Hlong)ui->label_Img->winId();

    OpenWindow(0, 0, ui->label_Img->geometry().width(),
               ui->label_Img->geometry().height(),
               windId, "visible", "", &d->windowHandle);

    SetColor(d->windowHandle, "red");
}

halconWidget::~halconWidget()
{
    delete ui;
    delete d;
}

bool halconWidget::setShowImage(HalconCpp::HObject obj)
{
    if (objIsEmpty(obj)) return false;

    d->img = obj.Clone();
    HTuple hv_Exception;
    try {

        if (d->windowHandle != NULL) {
            //如果有图像，则先清空图像
            qDebug() << "clear windows";
            DetachBackgroundFromWindow(d->windowHandle);
        }

        GetImageSize(d->img, &d->imgWid, &d->imgHit);
        d->col1 = 0;
        d->row1 = 0;
        d->col2 = d->imgWid;
        d->row2 = d->imgHit;
        qDebug() << "image size:" << d->imgWid[0].D() << d->imgHit[0].D();
        qDebug() << "image size:" << d->col2[0].D() << d->row2[0].D();
        SetPart(d->windowHandle, d->row1, d->col1, d->row2, d->col2);
        AttachBackgroundToWindow(d->img, d->windowHandle);
        DispObj(d->img, d->windowHandle);
        return true;
    }  catch (HException& HDevExpDefaultException) {
        HDevExpDefaultException.ToHTuple(&hv_Exception);
        return false;
    }
    return true;
}

void halconWidget::frashWindow()
{
    ClearWindow(d->windowHandle);

    if (objIsEmpty(d->img)) return;
    if (objIsEmpty(d->obj)) return;

    DispObj(d->img, d->windowHandle);
    DispObj(d->obj, d->windowHandle);
}

void halconWidget::showObj(HObject img, HObject obj)
{
    ClearWindow(d->windowHandle);

    if (objIsEmpty(img)) return;
    if (objIsEmpty(obj)) return;

    DispObj(img, d->windowHandle);
    DispObj(obj, d->windowHandle);
}

void halconWidget::showObj(HObject obj)
{
    ClearWindow(d->windowHandle);

    if (objIsEmpty(d->img)) return;
    if (objIsEmpty(obj)) return;

    d->obj = obj;
    DispObj(d->img, d->windowHandle);
    DispObj(obj, d->windowHandle);
}

bool halconWidget::objIsEmpty(const HalconCpp::HObject &obj)
{
    bool ini = obj.IsInitialized();
    if (!ini) {
        qDebug() << "the input obj is unuseless";
        return true;
    }

    HObject null;
    HTuple number;
    GenEmptyObj(&null);
    TestEqualObj(obj, null, &number); //判定是否和空对象相等。

    ini = number == 1 ? true : false;

    qDebug() << "the input obj is bad ?" << ini;
    return ini;
}

bool halconWidget::tupleisEmpty(const HTuple &tuple)
{
    HTuple hv_Length;
    TupleLength(tuple, &hv_Length);
    bool ini = (hv_Length.I() == 0);
    return ini;
}

HObject halconWidget::getShowImage()
{
    return d->img;
}

void halconWidget::setDrawMode(const int Mode)
{
    if(Mode == FILL)
        SetDraw(d->windowHandle, "fill");
    else
        SetDraw(d->windowHandle, "margin");
}

void halconWidget::setFont(QString fontType, int size, bool bold, bool slant)
{
    HTuple  hv_OS, hv_Fonts, hv_Style, hv_Exception, hv_Size;
    HTuple  hv_AvailableFonts, hv_Fdx, hv_Indices, hv_Font;

    try {
        if (size <= 0) hv_Size = 16;
        else {
            d->fontSize = size;
            hv_Size = size;
        }

        if (fontType == "Courier") {
            hv_Fonts.Clear();
            hv_Fonts[0] = "Courier";
            hv_Fonts[1] = "Courier 10 Pitch";
            hv_Fonts[2] = "Courier New";
            hv_Fonts[3] = "CourierNew";
            hv_Fonts[4] = "Liberation Mono";
        } else if (fontType =="mono") {
            hv_Fonts.Clear();
            hv_Fonts[0] = "Consolas";
            hv_Fonts[1] = "Menlo";
            hv_Fonts[2] = "Courier";
            hv_Fonts[3] = "Courier 10 Pitch";
            hv_Fonts[4] = "FreeMono";
            hv_Fonts[5] = "Liberation Mono";
        } else if (fontType =="sans") {
            hv_Fonts.Clear();
            hv_Fonts[0] = "Luxi Sans";
            hv_Fonts[1] = "DejaVu Sans";
            hv_Fonts[2] = "FreeSans";
            hv_Fonts[3] = "Arial";
            hv_Fonts[4] = "Liberation Sans";
        } else if (fontType =="serif") {
            hv_Fonts.Clear();
            hv_Fonts[0] = "Times New Roman";
            hv_Fonts[1] = "Luxi Serif";
            hv_Fonts[2] = "DejaVu Serif";
            hv_Fonts[3] = "FreeSerif";
            hv_Fonts[4] = "Utopia";
            hv_Fonts[5] = "Liberation Serif";
        } else {
            hv_Fonts.Clear();
            hv_Fonts[0] = "Times New Roman";
            hv_Fonts[1] = "Luxi Serif";
            hv_Fonts[2] = "DejaVu Serif";
            hv_Fonts[3] = "FreeSerif";
            hv_Fonts[4] = "Utopia";
            hv_Fonts[5] = "Liberation Serif";
        }

        hv_Style = "";
        if (bold) hv_Style += HTuple("Bold");
        if (slant) hv_Style += HTuple("Italic");

        if (!(bold || slant)) hv_Style = "Normal";

        QueryFont(d->windowHandle, &hv_AvailableFonts);
        hv_Font = "";

        {
            HTuple end_val48 = (hv_Fonts.TupleLength()) - 1;
            HTuple step_val48 = 1;
            for (hv_Fdx = 0; hv_Fdx.Continue(end_val48, step_val48); hv_Fdx += step_val48) {
                hv_Indices = hv_AvailableFonts.TupleFind(HTuple(hv_Fonts[hv_Fdx]));
                if (0 != ((hv_Indices.TupleLength()) > 0)) {
                    if (0 != (HTuple(hv_Indices[0]) >= 0)) {
                        hv_Font = HTuple(hv_Fonts[hv_Fdx]);
                        break;
                    }
                }
            }
        }
        if (0 != (hv_Font == HTuple(""))) {
            throw HException("Wrong value of control parameter Font");
        }
        hv_Font = (((hv_Font + "-") + hv_Style) + "-") + hv_Size;
        SetFont(d->windowHandle, hv_Font);
        return;
    }  catch (...) {
        qDebug() << "halcon setfont crash!";
        return ;
    }
}

void halconWidget::showMsg(const QString str, const QPoint pts)
{
    int x = pts.x() > 0 ? pts.x() : 0;
    int y = pts.y() > 0 ? pts.y() : 0;

    SetTposition(d->windowHandle, y, x);
    WriteString(d->windowHandle, str.toLocal8Bit().data());
}

void halconWidget::showMsg(QList<QString> strList, const QPoint pts)
{
    int x = pts.x() > 0 ? pts.x() : 0;
    int y = pts.y() > 0 ? pts.y() : 0;

    qDebug() << "font size:"<< d->fontSize;

    for(auto str : strList){
        SetTposition(d->windowHandle, y, x);
        WriteString(d->windowHandle, str.toLocal8Bit().data());

        y += d->fontSize * 5;
    }
}

void halconWidget::setImageFile(const QString &filepath)
{
    if(filepath.isEmpty()) return;

    try {
        HObject image;
        ReadImage(&image, filepath.toLocal8Bit().data());
        setShowImage(image);
    }  catch (...) {
        qDebug() << "halconWidget::setImageFile crashed";
    }

}

void halconWidget::wheelEvent(QWheelEvent *ev)
{
    try {

        if(objIsEmpty(d->img))
            return;

        double Zoom;
        HTuple mouseRow, mouseCol, Button;
        HTuple startRowBf, startColBf, endRowBf, endColBf, Ht, Wt, startRowAft, startColAft, endRowAft, endColAft;

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
            d->row1 = startRowAft;
            d->col1 = startColAft;
            d->row2 = endRowAft;
            d->col2 = endColAft;
            SetPart(d->windowHandle, d->row1, d->col1, d->row2, d->col2);
            AttachBackgroundToWindow(d->img, d->windowHandle);
        }
        SetCheck("give_error");
    }  catch (...) {
        qDebug() << "halconWidget::wheelEvent";
    }
}

void halconWidget::mousePressEvent(QMouseEvent *ev)
{
    bool in = ui->label_Img->rect().contains(ev->pos());
    if (!in) return;

    try {
        if(objIsEmpty(d->img))
            return;

        HTuple mouseRow, mouseCol, Button;
        SetCheck("~give_error");
        GetMposition(d->windowHandle, &mouseRow, &mouseCol, &Button);
        SetCheck("give_error");

        //鼠标按下时的行列坐标
        d->m_tMouseDownRow = mouseRow;
        d->m_tMouseDownCol = mouseCol;

        d->m_bIsMove = true;
    }  catch (...) {
        qDebug() << "halconWidget::mousePressEvent crash";
    }
}

void halconWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    d->m_bIsMove = false;
}

void halconWidget::mouseMoveEvent(QMouseEvent *ev)
{
    try {
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
            d->row1 = startRowBf - RowMove;
            d->row2 = endRowBf - RowMove;
            d->col1 = startColBf - ColMove;
            d->col2 = endColBf - ColMove;
            SetPart(d->windowHandle, d->row1, d->col1, d->row2, d->col2);
            AttachBackgroundToWindow(d->img, d->windowHandle);
        }

        HTuple pointGray;
        SetCheck("~give_error");
        GetGrayval(d->img, mouseRow, mouseCol, &pointGray);
        SetCheck("give_error");

        //当光标不在Halcon窗口内时返回，否则会报错
        ui->label->setText(QString("position( %1 , %2 )  gray: %3 ").arg(mouseCol[0].D()).arg(mouseRow[0].D()).arg(pointGray[0].D()));
        emit pointClicked(mouseCol[0].D(), mouseRow[0].D());

    }  catch (...) {
        qDebug() << "halconWidget::mouseMoveEvent crashed";
    }
}

bool halconWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj->objectName() != "labelImage")
        return false;

    if (event->type() == 14) {
        SetWindowExtents(d->windowHandle, 0, 0, ui->label_Img->geometry().width(), ui->label_Img->geometry().height());
        if (!tupleisEmpty(d->row1)) {
            SetPart(d->windowHandle, d->row1, d->col1, d->row2, d->col2);
        }
        return false;
    }
    return false;
}

}

void Graphics::halconWidget::on_radio_margin_clicked()
{
    setDrawMode(MARGIN);
    showObj(d->img, d->obj);
}

void Graphics::halconWidget::on_radio_fill_clicked()
{
    setDrawMode(FILL);
    showObj(d->img, d->obj);
}
