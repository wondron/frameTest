#ifndef IMAGEDETECTBASE_H
#define IMAGEDETECTBASE_H

#include <QWidget>
#include "imagegroupbase.h"
#include "halconGraphic/halconwidget.h"

namespace Ui {
class ImageDetectBase;
}

class ImageDetectBasePrivate;
class ImageDetectBase : public QWidget
{
    Q_OBJECT

public:
    explicit ImageDetectBase(QWidget *parent = nullptr);
    ~ImageDetectBase();

    void setPam(CSHDetect::imageGroupBase *pam);

    Graphics::halconWidget* getHalconWgt();

private slots:
    void on_btn_getImage_clicked();

    void getFilename(const QString& file);

    void on_btn_readPam_clicked();

    void on_btn_detect_clicked();

signals:
    void detectClicked();

private:
    int getImage(const QString &file);
    void iniLayout();

private:
    Ui::ImageDetectBase *ui;
    ImageDetectBasePrivate* d;
};

#endif // IMAGEDETECTBASE_H
