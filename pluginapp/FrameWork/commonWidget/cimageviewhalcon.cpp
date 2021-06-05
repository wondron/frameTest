#include "cimageviewhalcon.h"
#include "ui_cimageviewhalcon.h"
#include "citemlistwidget.h"

namespace Graphics {

class CImageViewHalconPrivate
{
public:
    CImageViewHalconPrivate(){}

    halconWidget* widget = new halconWidget();
    CItemListWidget* listWidget = new CItemListWidget();
};

CImageViewHalcon::CImageViewHalcon(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CImageViewHalcon),
    d(new CImageViewHalconPrivate())
{
    ui->setupUi(this);

    ui->lay_imgList->addWidget(d->listWidget);
    ui->lay_widget->addWidget(d->widget);
    connect(d->listWidget, &CItemListWidget::currentFileName, d->widget, &halconWidget::setImageFile);
}

CImageViewHalcon::~CImageViewHalcon()
{
    delete ui;
}

void CImageViewHalcon::setFilePath(const QString &filePath)
{
    d->listWidget->setFilePath(filePath);
}

QWidget *CImageViewHalcon::imageWindow()
{
    return d->widget;
}

}

