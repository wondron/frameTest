#ifndef CGETIMAGEWIDGET_H
#define CGETIMAGEWIDGET_H

#include <QWidget>
#include "commonWidget/cimageviewhalcon.h"

namespace Ui {
class CGetImageWidget;
}

class CGetImageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CGetImageWidget(QWidget *parent = nullptr);
    ~CGetImageWidget();

private slots:
    void on_pushButton_clicked();

private:
    Ui::CGetImageWidget *ui;
    Graphics::CImageViewHalcon* widget;
};

#endif // CGETIMAGEWIDGET_H
