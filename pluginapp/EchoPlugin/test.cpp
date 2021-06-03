#include "test.h"
#include "ui_test.h"
#include "halconcpp/HalconCpp.h"

test::test(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::test)
{
    ui->setupUi(this);

    using namespace HalconCpp;
    HObject hcon;
    HTuple windHandle;
    Hlong windId = (Hlong)ui->label->winId();

    OpenWindow(0, 0, ui->label->geometry().width(),
                     ui->label->geometry().height(),
               windId, "visible", "", &windHandle);

}

test::~test()
{
    delete ui;
}
