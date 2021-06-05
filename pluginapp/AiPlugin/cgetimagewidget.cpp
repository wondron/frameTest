#include "cgetimagewidget.h"
#include "ui_cgetimagewidget.h"
#include "commonWidget/citemlistwidget.h"
#include "qfiledialog.h"

CGetImageWidget::CGetImageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CGetImageWidget)
{
    ui->setupUi(this);
    widget = new Graphics::CImageViewHalcon();
    ui->frame_img->addWidget(widget);
}

CGetImageWidget::~CGetImageWidget()
{
    delete ui;
}

void CGetImageWidget::on_pushButton_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, "select the filepath");

    widget->setFilePath(directory);
}
