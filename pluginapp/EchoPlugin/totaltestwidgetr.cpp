#include "totaltestwidgetr.h"
#include "ui_totaltestwidgetr.h"
#include "../FrameWork/commonWidget/citemlistwidget.h"
#include "qfiledialog.h"
#include "qdebug.h"

#include "reverseSide/ccheckhanregwidget.h"
#include "reverseSide/cgetreregionswidget.h"
#include "reverseSide/cgettapeanglewidget.h"

#include "commonUsage/commonDef.h"
#include "commonUsage/comStruct.h"
#include "commonUsage/cerror.h"
#include "qmessagebox.h"
#include "imagedetectbase.h"

#define SENDERR(a) \
    {QMessageBox::warning(this, "warnning", a);\
    return 1;}

class totalTestWidgetRPrivate
{
public:
    totalTestWidgetRPrivate(){
        m_widget = new CItemListWidget();
        m_pam = new CReverseSideDetect();
        m_testWidget = new ImageDetectBase();

        m_hanWidget = new CCheckHanRegWidget();
        m_regionWidget = new CGetReRegionsWidget();
        m_angleWidget = new CGetTapeAngleWidget();
    }

    CItemListWidget* m_widget;
    CReverseSideDetect* m_pam;
    ImageDetectBase* m_testWidget;

    CCheckHanRegWidget *m_hanWidget;
    CGetReRegionsWidget* m_regionWidget;
    CGetTapeAngleWidget* m_angleWidget;

    HObject m_img;
    HTuple m_width;
    HTuple m_height;
    ReverRegionS regions;
};


totalTestWidgetR::totalTestWidgetR(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::totalTestWidgetR),
    d(new totalTestWidgetRPrivate())
{
    ui->setupUi(this);

    ui->lay_imgList->addWidget(d->m_widget);
    ui->layout_region->addWidget(d->m_regionWidget);
    ui->lay_han->addWidget(d->m_hanWidget);
    ui->lay_angle->addWidget(d->m_angleWidget);

    connect(d->m_widget, SIGNAL(currentFileName(QString)), this, SLOT(getFilename(QString)));
    connect(d->m_regionWidget, SIGNAL(detectDone()), this, SLOT(OnPamSetDetectDone()));
    connect(d->m_hanWidget, SIGNAL(detectDone()), this, SLOT(OnHanDetectDone()));
}

totalTestWidgetR::~totalTestWidgetR()
{
    delete ui;
}

void totalTestWidgetR::getFilename(const QString &file)
{
    if (file.isEmpty()) return;

    int err = getImage(file);
    if(err) return;

    d->m_regionWidget->setImage(d->m_img, d->regions);
}

void totalTestWidgetR::on_btn_getFilePath_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, "select the filepath");

    d->m_widget->setFilter("*.bmp *.jpg *.png");
    d->m_widget->setFilePath(directory);
}

int totalTestWidgetR::getImage(const QString &file)
{
    try {
        ui->tabWidget->setCurrentIndex(0);
        ReadImage(&d->m_img, file.toLocal8Bit().data());
        GetImageSize(d->m_img, &d->m_width, &d->m_height);

        CError err = Algorithm::objIsEmpty(d->m_img);
        if(err.isWrong())
            SENDERR(err.msg());

        return 0;
    }  catch (...) {
        SENDERR("getImage err");
    }
}

void totalTestWidgetR::OnPamSetDetectDone()
{
    d->regions = d->m_regionWidget->getRegons();
    d->m_hanWidget->setRegions(d->regions);
}

void totalTestWidgetR::OnHanDetectDone()
{
    d->regions = d->m_hanWidget->getRegions();
    d->m_angleWidget->setRegions(d->regions);
}

void totalTestWidgetR::on_btn_before_clicked()
{
    int idx = ui->tabWidget->currentIndex();

    ui->tabWidget->setCurrentIndex(--idx);
}

void totalTestWidgetR::on_btn_next_clicked()
{

    int idx = ui->tabWidget->currentIndex();

    ui->tabWidget->setCurrentIndex(++idx);
}

void totalTestWidgetR::on_btn_detect_clicked()
{
    CError err;
    err = d->m_regionWidget->detect();
    if(err.isWrong()) return;

    err = d->m_hanWidget->detect();
    if(err.isWrong()) return;

    d->m_angleWidget->detect();
}
