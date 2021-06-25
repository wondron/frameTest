#include "cgettapeanglewidget.h"
#include "ui_cgettapeanglewidget.h"
#include "halconGraphic/halconwidget.h"
#include "qmessagebox.h"
#include "commonUsage/algorithm.h"
#include "commonUsage/commonDef.h"
#include "qdebug.h"
#include "cgettapeanglepam.h"
#include "qgroupbox.h"

#define SENDERR(a) \
    {QMessageBox::warning(this, "warnning", a);\
    return;}

namespace CSHDetect {

class CGetTapeAngleWidgetPrivate
{
public:
    CGetTapeAngleWidgetPrivate()
    {
        m_widget = new Graphics::halconWidget();
        m_pam = new CGetTapeAngle();

        pamWidgetList.clear();
        measureList.clear();
        biaList.clear();

        for(int i = 0; i < 4; i++){
            CGetTapeAnglePam* widget = new CGetTapeAnglePam();
            pamWidgetList.push_back(widget);
        }

    }

public:
    Graphics::halconWidget* m_widget;
    ReverRegionS m_region;
    CGetTapeAngle* m_pam;
    QList<CGetTapeAnglePam*> pamWidgetList;
    QList<QGroupBox*> grpbxList;
    HObject slctReg;

    QList<MeasureposPam> measureList;
    QList<PosBia> biaList;

    bool isSetRegion = false;

};

CGetTapeAngleWidget::CGetTapeAngleWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CGetTapeAngleWidget),
    d(new CGetTapeAngleWidgetPrivate())
{
    ui->setupUi(this);
    initialWidget();

    for(auto i : d->pamWidgetList)
        connect(i, SIGNAL(pamChanged()), this, SLOT(getPam()));
}

CGetTapeAngleWidget::~CGetTapeAngleWidget()
{
    delete ui;
}

void CGetTapeAngleWidget::setRegions(const ReverRegionS& region)
{
    CError err = Algorithm::objIsEmpty(region.batteryRegion);
    if (err.isWrong())
        SENDERR("CGetTapeAngleWidget::setRegions input regiono is empty");

    d->m_region = region;
    d->isSetRegion = true;
    d->m_widget->setShowImage(region.m_oriImg);
}

void CGetTapeAngleWidget::setPam(CGetTapeAngle* pam)
{
    d->m_pam = pam;
}

CError CGetTapeAngleWidget::detect()
{
    d->m_pam->pamRead("D:\\work\\detect\\detection\\bin\\conf\\algorithm\\cshdefault\\detection.xml");
    QList<QLine> gdLine, ngLine;
    CError err = d->m_pam->detect(d->m_region.m_oriImg, d->m_region, gdLine, ngLine);

    HObject lines, line;
    GenEmptyRegion(&lines);
    GenEmptyRegion(&line);

    try {
        for(auto i : gdLine){
            GenRegionLine(&line, i.y1(), i.x1(), i.y2(), i.x2());
            Union2(line, lines, &lines);
        }

        for(auto i : ngLine){
            GenRegionLine(&line, i.y1(), i.x1(), i.y2(), i.x2());
            Union2(line, lines, &lines);
        }

        d->m_widget->showObj(lines);
    }  catch (...) {
        QMessageBox::warning(this, "warnning", "CGetTapeAngleWidget::detect crashed!");
        return 1;
    }

    return err;
}

void CGetTapeAngleWidget::initialWidget()
{
    ui->vlay_widget->addWidget(d->m_widget);
    ui->lay_1->addWidget(d->pamWidgetList[0]);
    ui->lay_2->addWidget(d->pamWidgetList[1]);
    ui->lay_3->addWidget(d->pamWidgetList[2]);
    ui->lay_4->addWidget(d->pamWidgetList[3]);

    d->grpbxList.push_back(ui->grpbx_1);
    d->grpbxList.push_back(ui->grpbx_2);
    d->grpbxList.push_back(ui->grpbx_3);
    d->grpbxList.push_back(ui->grpbx_4);

    for(int i = 1; i < 4; i++){
        d->grpbxList[i]->setVisible(false);
    }
}

void CGetTapeAngleWidget::on_cmbx_quadrant_currentIndexChanged(int index)
{
    for(int i = 0; i < 4; i++){
        bool visual = (index == i);
        d->grpbxList[i]->setVisible(visual);
    }

}

void CGetTapeAngleWidget::getPam()
{
    d->measureList.clear();
    d->biaList.clear();

    for(auto i : d->pamWidgetList){
        MeasureposPam pam;
        PosBia bia;

        i->getPamValue(pam, bia);
        d->measureList.push_back(pam);
        d->biaList.push_back(bia);
    }
}


void CGetTapeAngleWidget::on_btn_pamRead_clicked()
{
  d->m_pam->pamRead("D:\\work\\detect\\detection\\bin\\conf\\algorithm\\cshdefault\\detection.xml");

  d->m_pam->getPamValue(d->measureList, d->biaList);

  for(int i = 0; i < 4; i++){
      d->pamWidgetList[i]->setPamValue(d->measureList[i], d->biaList[i]);
  }
}

}

void CSHDetect::CGetTapeAngleWidget::on_btn_singleDetct_clicked()
{
    try {

        CError err = Algorithm::objIsEmpty(d->m_region.hanregion);
        if (err.isWrong())
            SENDERR("mregion.hanregion is empty");

        LineInfo line;
        getPam();

        int index = ui->cmbx_quadrant->currentIndex();
        err = d->m_pam->getSignalLines(d->m_region.m_oriImg, d->m_region.hanregion, line, index, d->biaList[index], d->measureList[index]);
        if(err.isWrong()) SENDERR(err.msg());

        HObject lineH;
        GenRegionLine(&lineH, line.startRow, line.startCol, line.endRow, line.endCol);
        d->m_widget->showObj(lineH);
    }  catch (...) {
        SENDERR("singleDetct crashed!");
    }
}

void CSHDetect::CGetTapeAngleWidget::on_btn_detect_clicked()
{
    detect();
}
