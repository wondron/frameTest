#include "cgetreregionswidget.h"
#include "ui_cgetreregionswidget.h"
#include "halconGraphic/halconwidget.h"
#include "qfiledialog.h"
#include "obverseSide/cgetregions.h"
#include "qdebug.h"
#include "commonUsage/algorithm.h"
#include "qmessagebox.h"
#include "commonUsage/commonDef.h"

#define SENDERR(a) \
    {QMessageBox::warning(this, "warnning", a);\
    return;}

namespace CSHDetect {

using namespace Graphics;

class CGetReRegionsWidgetPrivate
{
public:
    enum {DETECTSIZE = 4};

    CGetReRegionsWidgetPrivate()
    {
        pam = new CGetReRegions();

        for (int i = 0; i < DETECTSIZE; i++) {
            halconWidget* widget = new halconWidget();
            widgetList.push_back(widget);
        }
    }

    ~CGetReRegionsWidgetPrivate()
    {

    }

public:
    HObject img;
    HTuple imgWid;
    HTuple imgHit;
    CGetReRegions* pam;
    ReverRegionS regions;

    QList<halconWidget*> widgetList;
    QList<QGroupBox*> pamGroupList;
};

CGetReRegionsWidget::CGetReRegionsWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CGetReRegionsWidget),
    d(new CGetReRegionsWidgetPrivate())
{
    ui->setupUi(this);
    iniLayout();
}

CGetReRegionsWidget::~CGetReRegionsWidget()
{
    delete ui;
}

void CGetReRegionsWidget::setPam(CGetReRegions* pam)
{
    d->pam = pam;
}

void CGetReRegionsWidget::setImage(HObject& img, ReverRegionS& regions)
{
    try {
        CError err = Algorithm::objIsEmpty(img);
        if (err.isWrong()) SENDERR(err.msg());

        d->img = img;
        GetImageSize(d->img, &d->imgWid, &d->imgHit);
        d->regions = regions;
        d->regions.m_oriImg = img;

        for (auto i : d->widgetList)
            i->setShowImage(d->img);

    }  catch (...) {
        SENDERR("setImage func happen unexcepted!");
    }
}

const ReverRegionS& CGetReRegionsWidget::getRegons() const
{
    return d->regions;
}

CError CGetReRegionsWidget::detect()
{
    err = Algorithm::objIsEmpty(d->img);
    if (err.isWrong()){
        QMessageBox::warning(this, "warnning", err.msg());
        return 1;
    }

    err = d->pam->detect(d->img, d->regions);
    if (err.isWrong()){
        QMessageBox::warning(this, "warnning", err.msg());
        return 1;
    }

    d->widgetList[0]->showObj(d->regions.batteryRegion);
    d->widgetList[1]->showObj(d->regions.midRegion);
    //d->widgetList[2]->showObj(d->regions.blueTapesReg);
    //d->widgetList[3]->showObj(d->regions.dblTapeReg);
    emit detectDone();

    return 0;
}

void CGetReRegionsWidget::setDetectImage(const QString& temPath)
{
    if (temPath.isEmpty()) return;

    try {
        CError err;

        ReadImage(&d->img, temPath.toStdString().c_str());
        err = Algorithm::objIsEmpty(d->img);
        if (err.isWrong()) SENDERR(err.msg());

        GetImageSize(d->img, &d->imgWid, &d->imgHit);

        for (auto i : d->widgetList)
            i->setShowImage(d->img);

    }  catch (...) {
        emit SENDERR("getclicked func happen unexcepted!");
    }
}

void CGetReRegionsWidget::iniLayout()
{
    ui->tabWidget->setCurrentIndex(0);

    d->pamGroupList.push_back(ui->grpbx_battery);
    d->pamGroupList.push_back(ui->grpbx_mid);
    d->pamGroupList.push_back(ui->grpbx_tapes);
    d->pamGroupList.push_back(ui->grpbx_dbltapes);

    for (auto i : d->pamGroupList)
        i->setVisible(false);

    d->pamGroupList[0]->setVisible(true);

    ui->layout_battery->addWidget(d->widgetList[0]);
    ui->layout_midregion->addWidget(d->widgetList[1]);
    ui->layout_blueTape->addWidget(d->widgetList[2]);
    ui->layout_dblTape->addWidget(d->widgetList[3]);
}

void CGetReRegionsWidget::on_tabWidget_currentChanged(int index)
{
    for (int i = 0; i < CGetReRegionsWidgetPrivate::DETECTSIZE; i++) {
        if(i == index)
            d->widgetList[i]->frashWindow();
        d->pamGroupList[i]->setVisible((i == index));
    }
}

void CGetReRegionsWidget::on_btn_battery_clicked()
{
    int minThre = ui->spin_batteryMinThre->value();
    int ero = ui->spin_batterryEro->value();
    int inx = ui->tabWidget->currentIndex();
    int slctNum =ui->spin_batterySlctNum->value();
    int gridW = ui->spin_batteryGridW->value();
    int gridH = ui->spin_batteryGridH->value();
    err = d->pam->getBatteryRegion(d->img, d->regions.batteryRegion, gridW, gridH, minThre, ero, slctNum);

    if (err.isWrong()) SENDERR(err.msg());

    d->widgetList[inx]->showObj(d->regions.batteryRegion);
}

void CGetReRegionsWidget::on_btn_mid_clicked()
{
    int dire = ui->comb_batDire->currentIndex();
    int thre = ui->spin_midThre->value();
    int ero  = ui->spin_midero->value();
    int dila = ui->spin_midDila->value();

    err = d->pam->getMidRegion(d->img, d->regions.batteryRegion, d->regions.midRegion, dire, thre, dila, ero);
    if (err.isWrong()) SENDERR(err.msg());

    int inx = ui->tabWidget->currentIndex();
    d->widgetList[inx]->showObj(d->regions.midRegion);
}

void CGetReRegionsWidget::on_btn_tapeNum_clicked()
{
    int hDila = ui->spin_tapeHDila->value();
    int wDila = ui->spin_tapeWDila->value();

    err = Algorithm::objIsEmpty(d->regions.midRegion);
    if (err.isWrong()) SENDERR(err.msg());

    int tapeSize = 0;
    err = d->pam->getBlueTapeNum(d->regions.midRegion, d->regions.blueTapesReg, hDila, wDila, tapeSize);
    if (err.isWrong()) SENDERR(err.msg());

    int inx = ui->tabWidget->currentIndex();
    d->widgetList[inx]->showObj(d->regions.blueTapesReg);
}

void CGetReRegionsWidget::on_btn_dblTape_clicked()
{
    int ero = ui->spin_dblEro->value();
    int dire = ui->cmbx_dblDirect->currentIndex();

    err = Algorithm::objIsEmpty(d->regions.blueTapesReg);
    if (err.isWrong())
        SENDERR(err.msg());

    err = d->pam->getDoubleTapeRoi(d->regions.blueTapesReg, d->regions.dblTapeReg, dire, ero);
    if (err.isWrong())
        SENDERR(err.msg());

    int inx = ui->tabWidget->currentIndex();
    d->widgetList[inx]->showObj(d->regions.dblTapeReg);
}

void CGetReRegionsWidget::on_btn_test_clicked()
{
    detect();
}

}

