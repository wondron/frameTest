#include "cimageviewcv.h"
#include "ui_cimageviewcv.h"
#include "citemlistwidget.h"
#include "graphics/wgraphicswidget.h"

namespace Graphics {

class CImageViewCvPrivate
{
public:
    CImageViewCvPrivate(){};

    WGraphicsWidget* widget = new WGraphicsWidget();
    CItemListWidget* listWidget = new CItemListWidget();

};

CImageViewCv::CImageViewCv(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CImageViewCv),
    d(new CImageViewCvPrivate())
{
    ui->setupUi(this);
    iniLayout();

    connect(d->listWidget, &CItemListWidget::currentFileName, d->widget, &WGraphicsWidget::setImagePath);
}

CImageViewCv::~CImageViewCv()
{
    delete ui;
}

void CImageViewCv::setFilePath(const QString &filePath)
{
    d->listWidget->setFilePath(filePath);
}

void CImageViewCv::setImage(const QImage &img)
{
    if(img.isNull()) return;

    d->widget->setImage(img);
}

QWidget *CImageViewCv::imageWindow()
{
    return d->widget;
}

void CImageViewCv::iniLayout()
{
    ui->lay_imgList->addWidget(d->listWidget);
    ui->lay_widget->addWidget(d->widget);
}

}
