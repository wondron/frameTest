#include "totalalg.h"
#include "ui_totalalg.h"

totalAlg::totalAlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::totalAlg)
{
    ui->setupUi(this);

    obverWidget = new totalTestWidget();
    reverWidget = new totalTestWidgetR();
    testWidget = new test();

    ui->lay_obver->addWidget(obverWidget);
    ui->lay_rever->addWidget(reverWidget);
    ui->lay_test->addWidget(testWidget);
}

totalAlg::~totalAlg()
{
    delete ui;
}
