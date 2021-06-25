#ifndef CIMAGEVIEWHALCON_H
#define CIMAGEVIEWHALCON_H

#include <QWidget>
#include "halconGraphic/halconwidget.h"

namespace Ui {
class CImageViewHalcon;
}

namespace Graphics {

class CImageViewHalconPrivate;

class CImageViewHalcon : public QWidget
{
    Q_OBJECT

public:
    explicit CImageViewHalcon(QWidget *parent = nullptr);
    ~CImageViewHalcon();

    /**
     * @brief getImageWindow::获取图像窗口
     */
    QWidget* imageWindow();

    /**
     * @brief image: 返回当前图像
     * @return
     */
    HalconCpp::HObject currentImage();

public slots:
    void setFilePath(const QString& filePath);

private:
    void iniLayout();

private:
    Ui::CImageViewHalcon *ui;
    CImageViewHalconPrivate *d;
};

}




#endif // CIMAGEVIEWHALCON_H
