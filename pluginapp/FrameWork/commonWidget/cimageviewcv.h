#ifndef CIMAGEVIEWCV_H
#define CIMAGEVIEWCV_H

#include <QWidget>

namespace Ui {
class CImageViewCv;
}

namespace Graphics {

class CImageViewCvPrivate;

class CImageViewCv : public QWidget
{
    Q_OBJECT

public:
    explicit CImageViewCv(QWidget *parent = nullptr);
    ~CImageViewCv();

    /**
     * @brief getImage: 获取图像
     */
    QImage getImage();

    /**
     * @brief setImage: 设置图像
     */
    void setImage(const QImage& img);

    /**
     * @brief getImageWindow::获取图像窗口
     */
    QWidget *imageWindow();

public slots:
    void setFilePath(const QString& filePath);
    
private:
    void iniLayout();

private:
    Ui::CImageViewCv *ui;
    CImageViewCvPrivate* d;
};
}


#endif // CIMAGEVIEWCV_H
