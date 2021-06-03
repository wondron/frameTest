#include "cgetregionswidget.h"
#include "ui_cgetregionswidget.h"
#include "commonUsage/imageviewerhalcon.h"
#include "qfiledialog.h"
#include "obverseSide/cgetregions.h"
#include "qdebug.h"
#include "commonUsage/algorithm.h"
#include "qradiobutton.h"
#include "qmessagebox.h"
#include "commonUsage/commonDef.h"

#define CHECKRES(err) \
    if(err.isWrong()){\
        emit sendErr(err.msg());\
        return;\
}

using namespace Graphics;

class CGetRegionsWidgetPrivate
{
public:
    enum {DETECTSIZE = 8};
    CGetRegionsWidgetPrivate(CGetRegionsWidget *parent): p(parent){
        pam = new CSHDetect::CGetRegions();

        for(int i = 0; i < DETECTSIZE; i++){
            ImageViewerHalcon* widget = new ImageViewerHalcon();
            widgetList.push_back(widget);
        }
    }

    HObject img;
    HTuple imgWid;
    HTuple imgHit;
    CGetRegions* pam;
    RegionS regions;

    QList<QRadioButton *> radioBtnList;
    QList<QLayout*> layoutList;
    QList<ImageViewerHalcon*> widgetList;
    QHash<int, HObject> channelImgs;
    QList<QGroupBox *> pamGroupList;

private:
    CGetRegionsWidget *p;
};

CGetRegionsWidget::CGetRegionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CGetRegionsWidget),
    d(new CGetRegionsWidgetPrivate(this))
{
    ui->setupUi(this);
    iniLayout();

    connect(this, SIGNAL(sendErr(QString)), this, SLOT(showErrMsg(const QString&)));
}

CGetRegionsWidget::~CGetRegionsWidget()
{
    delete ui;
}

void CGetRegionsWidget::setPam(CGetRegions *pam)
{
    d->pam = pam;
}

void CGetRegionsWidget::setImage(HObject &img, RegionS &regions)
{
    try {
        CHECKRES(Algorithm::objIsEmpty(img));
        d->img = img;
        GetImageSize(d->img, &d->imgWid, &d->imgHit);

        d->regions = regions;
        updateChannelImg(d->regions);

        for(auto i : d->widgetList)
            i->setShowImage(d->img);
    }  catch (...) {
        emit sendErr("setImage func happen unexcepted!");
    }
}

const RegionS& CGetRegionsWidget::getRegons() const
{
    return d->regions;
}

void CGetRegionsWidget::setDetectImage(const QString &temPath)
{
    if (temPath.isEmpty()) return;

    try {
        ReadImage(&d->img, temPath.toStdString().c_str());
        CHECKRES(Algorithm::objIsEmpty(d->img));

        GetImageSize(d->img, &d->imgWid, &d->imgHit);
        err = d->pam->transformImg(d->img, d->regions);
        CHECKRES(err);

        updateChannelImg(d->regions);

        for(auto i : d->widgetList)
            i->setShowImage(d->img);

    }  catch (...) {
        emit sendErr("getclicked func happen unexcepted!");
    }
}

void CGetRegionsWidget::iniLayout()
{
    d->radioBtnList.clear();
    d->radioBtnList.push_back(ui->radiobtn_Ori);
    d->radioBtnList.push_back(ui->radiobtn_R);
    d->radioBtnList.push_back(ui->radiobtn_G);
    d->radioBtnList.push_back(ui->radiobtn_B);
    d->radioBtnList.push_back(ui->radiobtn_H);
    d->radioBtnList.push_back(ui->radiobtn_S);
    d->radioBtnList.push_back(ui->radiobtn_V);

    d->layoutList.push_back(ui->layout_blackBang);
    d->layoutList.push_back(ui->layout_Dark);
    d->layoutList.push_back(ui->layout_White);
    d->layoutList.push_back(ui->layout_HsvRoi);
    d->layoutList.push_back(ui->layout_deepRegion);
    d->layoutList.push_back(ui->layout_blue);
    d->layoutList.push_back(ui->layout_weld);
    d->layoutList.push_back(ui->layout_tape);

    d->pamGroupList.push_back(ui->grpbx_blackBang);
    d->pamGroupList.push_back(ui->grpbx_dark);
    d->pamGroupList.push_back(ui->grpbx_white);
    d->pamGroupList.push_back(ui->grpbx_hsvRoi);
    d->pamGroupList.push_back(ui->grpbx_deepRed);
    d->pamGroupList.push_back(ui->grpbx_blue); //蓝色胶带预留位
    d->pamGroupList.push_back(ui->grpbx_weld);
    d->pamGroupList.push_back(ui->grpbx_tape);

    for(int i = 0; i < CGetRegionsWidgetPrivate::DETECTSIZE; i++){
        d->layoutList[i]->addWidget(d->widgetList[i]);
    }

    for(auto i : d->pamGroupList)
        i->setVisible(false);

    d->pamGroupList[0]->setVisible(true);
}

void CGetRegionsWidget::updateChannelImg(RegionS &imgs)
{
    if(!Algorithm::objIsEmpty(d->img))
        d->channelImgs.insert(ORIGINAL, d->img);
    if(!Algorithm::objIsEmpty(imgs.m_RImg))
        d->channelImgs.insert(RGBR, imgs.m_RImg);
    if(!Algorithm::objIsEmpty(imgs.m_GImg))
        d->channelImgs.insert(RGBG, imgs.m_GImg);
    if(!Algorithm::objIsEmpty(imgs.m_RImg))
        d->channelImgs.insert(RGBB, imgs.m_BImg);
    if(!Algorithm::objIsEmpty(imgs.m_RImg))
        d->channelImgs.insert(HSVH, imgs.m_hsvHImg);
    if(!Algorithm::objIsEmpty(imgs.m_RImg))
        d->channelImgs.insert(HSVS, imgs.m_hsvSImg);
    if(!Algorithm::objIsEmpty(imgs.m_RImg))
        d->channelImgs.insert(HSVV, imgs.m_hsvVImg);
}

int CGetRegionsWidget::getShowImgType()
{
    for(int i = 0; i < d->radioBtnList.size(); i++){
        if(d->radioBtnList[i]->isChecked())
            return i;
    }
    return 0;
}

void CGetRegionsWidget::on_btn_getImage_clicked()
{
    QString temPath = QFileDialog::getOpenFileName(this, "open file");
    if (temPath.isEmpty()) return;

    try {
        ReadImage(&d->img, temPath.toStdString().c_str());
        GetImageSize(d->img, &d->imgWid, &d->imgHit);
        CHECKRES(Algorithm::objIsEmpty(d->img));

        err = d->pam->transformImg(d->img, d->regions);
        CHECKRES(err);

        updateChannelImg(d->regions);

        for(auto i : d->widgetList)
            i->setShowImage(d->img);

    }  catch (...) {
        emit sendErr("getclicked func happen unexcepted!");
    }

}

void CGetRegionsWidget::on_btn_test_clicked()
{
    err = Algorithm::objIsEmpty(d->img);

    CGetRegions result;
    err = result.detect(d->img, d->regions);

    if(err.isWrong()){
        emit sendErr(err.msg());
        return;
    }

    emit detectDone();
    d->widgetList[0]->showObj(d->regions.BlackBangRegion);
    d->widgetList[1]->showObj(d->regions.DarkRegion);
    d->widgetList[2]->showObj(d->regions.WhiteRegion);
    d->widgetList[3]->showObj(d->regions.HSVRoi);
    d->widgetList[4]->showObj(d->regions.DeepRedRegion);
    d->widgetList[5]->showObj(d->regions.BlueRegion);
    d->widgetList[6]->showObj(d->regions.WeldRoi);
    d->widgetList[7]->showObj(d->regions.TapeRegion);
}

void CGetRegionsWidget::showErrMsg(const QString &msg)
{
    QMessageBox::warning(this, "error", msg);
}

void CGetRegionsWidget::on_btn_showImg_clicked()
{
    int index =getShowImgType();
    int widgetIndex = ui->tabWidget->currentIndex();

    d->widgetList[widgetIndex]->setShowImage(d->channelImgs.value(index));
}

void CGetRegionsWidget::on_tabWidget_currentChanged(int index)
{
    for(int i =0; i < CGetRegionsWidgetPrivate::DETECTSIZE; i++){
        if(i == index)
            d->widgetList[i]->frashWindow();
        d->pamGroupList[i]->setVisible((i == index));
    }
}

void CGetRegionsWidget::on_btn_savePam_clicked()
{

}

void CGetRegionsWidget::on_btn_blackBang_clicked()
{
    err = d->pam->getBlackBangRoi(d->channelImgs[HSVV], d->regions.BlackBangRegion, ui->spin_blkBangMaxThre->value());
    CHECKRES(err);

    int i = ui->tabWidget->currentIndex();
    d->widgetList[i]->showObj(d->regions.BlackBangRegion);
}

void CGetRegionsWidget::on_btn_dark_clicked()
{
    err = d->pam->getDarkRegion(d->channelImgs[RGBR], d->channelImgs[RGBG], d->channelImgs[RGBB], d->regions.DarkRegion,
                                ui->spin_darkMaxR->value(), ui->spin_darkMaxG->value(), ui->spin_darkMaxB->value());
    CHECKRES(err);
    int i = ui->tabWidget->currentIndex();
    d->widgetList[i]->showObj(d->regions.DarkRegion);
}

void CGetRegionsWidget::on_btn_white_clicked()
{
    err = d->pam->getWhiteRegion(d->channelImgs[RGBR], d->channelImgs[RGBG], d->channelImgs[RGBB], d->regions.WhiteRegion,
                                ui->spin_whiteMinR->value(), ui->spin_whiteMinG->value(), ui->spin_whiteMinB->value());
    CHECKRES(err);
    int i = ui->tabWidget->currentIndex();
    d->widgetList[i]->showObj(d->regions.WhiteRegion);
}

void CGetRegionsWidget::on_btn_hsvRoi_clicked()
{
    err = d->pam->getHsvRoi(d->channelImgs[HSVH], d->channelImgs[HSVS], d->channelImgs[HSVV], d->regions.HSVRoi,
                                ui->spin_minhsvH->value(), ui->spin_maxhsvH->value(), ui->spin_minhsvS->value(), ui->spin_maxhsvV->value());
    CHECKRES(err);
    int i = ui->tabWidget->currentIndex();
    d->widgetList[i]->showObj(d->regions.HSVRoi);
}

void CGetRegionsWidget::on_btn_deepRed_clicked()
{
    err = d->pam->getDeepRedRegion(d->channelImgs[RGBR], d->regions.DeepRedRegion, ui->spin_deepRedThr->value());
    CHECKRES(err);

    int i = ui->tabWidget->currentIndex();
    d->widgetList[i]->showObj(d->regions.DeepRedRegion);
}

void CGetRegionsWidget::on_btn_blue_clicked()
{
    err = d->pam->getBlueRegion(d->regions.HSVRoi, d->regions.DarkRegion, d->regions.DeepRedRegion, d->regions.BlueRegion);
    CHECKRES(err);

    int i = ui->tabWidget->currentIndex();
    d->widgetList[i]->showObj(d->regions.BlueRegion);
}

void CGetRegionsWidget::on_btn_Tape_clicked()
{
    err = d->pam->getTapeRegion(d->channelImgs[RGBR], d->regions.WhiteRegion, d->regions.TapeRegion,
                                ui->spin_tapeholeSize->value(), ui->dspin_slctRange->value());
    CHECKRES(err);

    int i = ui->tabWidget->currentIndex();
    d->widgetList[i]->showObj(d->regions.TapeRegion);
}

void CGetRegionsWidget::on_btn_weld_clicked()
{
    int size;
    err = d->pam->getWeldRoi(d->regions.BlackBangRegion, d->regions.BlueRegion, d->regions.WeldRoi, size,
                             ui->spin_weldDilation->value(), ui->spin_weldMinArea->value());
    CHECKRES(err);

    int i = ui->tabWidget->currentIndex();
    d->widgetList[i]->showObj(d->regions.WeldRoi);
}
