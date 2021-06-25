#include "test.h"
#include "ui_test.h"
#include "halconcpp/HalconCpp.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "commonUsage/algorithm.h"
#include "commonUsage/commonDef.h"
#include "halconGraphic/halconwidget.h"

#define SENDERR(a) \
    {QMessageBox::warning(this, "warnning", a);\
    return;}

test::test(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::test)
{
    ui->setupUi(this);
    widget = new Graphics::CImageViewHalcon();
    ui->lay_image->addWidget(widget);
}

test::~test()
{
    delete ui;
}

void test::on_btn__clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, "select the filepath");
    widget->setFilePath(directory);
}

void test::on_pushButton_clicked()
{
    try {
        HalconCpp::HObject res;

        HObject dst = widget->currentImage();

        HObject RImg, GImg, BImg, hShow;

        Decompose3(dst, &RImg, &GImg, &BImg);

        QVector<RectInfo> rectList, gdInfo, ngInfo;
        RectInfo info;
        //1
        info.XBia = -1000;
        info.YBia = -850;
        info.phi  = 0;
        info.len1 = 700;
        info.len2 = 100;
        rectList.push_back(info);

        //2
        info.XBia = -3400;
        info.YBia = -850;
        info.phi  = 0;
        info.len1 = 700;
        info.len2 = 100;
        rectList.push_back(info);

        //3
        info.XBia = -3400;
        info.YBia = 850;
        info.phi  = 0;
        info.len1 = 700;
        info.len2 = 100;
        rectList.push_back(info);

        //3
        info.XBia = -1000;
        info.YBia = 850;
        info.phi  = 0;
        info.len1 = 700;
        info.len2 = 100;
        rectList.push_back(info);

        QPoint pts(4958, 1835);
        rectList.push_back(info);
        Algorithm::detectRegionExit(RImg, hShow, rectList, gdInfo, ngInfo, pts, 0, 80, 1.5, QSize(1000, 1));
        //RectInfo in = ngInfo[0];
        //GenRectangle2(&hShow, in.row, in.col, in.phi, in.len1, in.len2);

        Graphics::halconWidget* widgett = static_cast<Graphics::halconWidget*>(widget->imageWindow());
        widgett->showObj(RImg, hShow);

    }  catch (...) {
        SENDERR("singleDetct crashed!");
    }
}
