#include "imagedetectbase.h"
#include "ui_imagedetectbase.h"
#include "qfiledialog.h"
#include "qdebug.h"
#include "obverseSide/cgetregionswidget.h"
#include "obverseSide/cgetcustrapposwidget.h"
#include "obverseSide/cgetcirclelenwidget.h"
#include "commonUsage/commonDef.h"
#include "commonUsage/comStruct.h"
#include "commonUsage/cerror.h"
#include "qmessagebox.h"
#include "../FrameWork/commonWidget/citemlistwidget.h"

#define SENDERR(a) \
{QMessageBox::warning(this, "warnning", a);\
return 1;}

class ImageDetectBasePrivate
{
public:
    ImageDetectBasePrivate(){
        m_widget = new CItemListWidget();
        m_pam = new imageGroupBase();
        m_widgetHalcon = new Graphics::halconWidget();
    }

    CItemListWidget* m_widget;
    Graphics::halconWidget* m_widgetHalcon;
    imageGroupBase* m_pam;

    HObject m_img;
    HTuple m_width;
    HTuple m_height;
    RegionS regions;
};

ImageDetectBase::ImageDetectBase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageDetectBase),
    d(new ImageDetectBasePrivate())
{
    ui->setupUi(this);
    iniLayout();
    connect(d->m_widget, SIGNAL(currentFileName(QString)), this, SLOT(getFilename(QString)));
}

ImageDetectBase::~ImageDetectBase()
{
    delete ui;
}

void ImageDetectBase::setPam(CSHDetect::imageGroupBase *pam)
{
    d->m_pam = pam;
}

Graphics::halconWidget *ImageDetectBase::getHalconWgt()
{
    return d->m_widgetHalcon;
}

void ImageDetectBase::on_btn_getImage_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, "select the filepath");

    d->m_widget->setFilter("*.bmp *.jpg *.png");
    d->m_widget->setFilePath(directory);

}

void ImageDetectBase::getFilename(const QString &file)
{
    if (file.isEmpty()) return;

    int err = getImage(file);
    if(err) return;

    d->m_widgetHalcon->setShowImage(d->m_img);
}

int ImageDetectBase::getImage(const QString &file)
{
    try {
        ReadImage(&d->m_img, file.toLocal8Bit().data());

        CError err = Algorithm::objIsEmpty(d->m_img);
        if(err.isWrong())
            SENDERR(err.msg());

        return 0;
    }  catch (...) {
        SENDERR("getImage err");
    }
}

void ImageDetectBase::iniLayout()
{
    ui->lay_ImageList->addWidget(d->m_widget);
    ui->lay_widget->addWidget(d->m_widgetHalcon);
}

void ImageDetectBase::on_btn_readPam_clicked()
{
    d->m_pam->readPam(d->m_pam->getTaskName().toLocal8Bit().data());
}

void ImageDetectBase::on_btn_detect_clicked()
{
    qreal alpha = ui->spin_alpha->value();
    int low = ui->spin_low->value();
    int high = ui->spin_high->value();
    try {
        HObject edge;
        EdgesColorSubPix(d->m_img, &edge, "canny", alpha, low, high);
        d->m_widgetHalcon->showObj(d->m_img, edge);
    }  catch (...) {

    }
    emit detectClicked();
}
