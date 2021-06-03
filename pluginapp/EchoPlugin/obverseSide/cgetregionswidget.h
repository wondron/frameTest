#ifndef CGETREGIONSWIDGET_H
#define CGETREGIONSWIDGET_H

#include <QWidget>
#include "commonUsage/comStruct.h"
#include "obverseSide/cgetregions.h"
#include "commonUsage/cerror.h"


namespace Ui {
class CGetRegionsWidget;
}

class CGetRegionsWidgetPrivate;
class CGetRegionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CGetRegionsWidget(QWidget *parent = nullptr);
    ~CGetRegionsWidget();

    void setPam(CSHDetect::CGetRegions *pam);
    void setImage(HalconCpp::HObject& img, RegionS& regions);
    const RegionS &getRegons() const;

public slots:
    void setDetectImage(const QString& filename);

private slots:
    void on_btn_getImage_clicked();

    void on_btn_test_clicked();

    void showErrMsg(const QString& msg);

    void on_btn_showImg_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_btn_savePam_clicked();

    void on_btn_blackBang_clicked();

    void on_btn_dark_clicked();

    void on_btn_white_clicked();

    void on_btn_hsvRoi_clicked();

    void on_btn_deepRed_clicked();

    void on_btn_blue_clicked();

    void on_btn_Tape_clicked();

    void on_btn_weld_clicked();

signals:
    void sendErr(QString);
    void detectDone();

private:
    void iniLayout();
    void updateChannelImg(RegionS& imgs);
    int getShowImgType();

private:
    Ui::CGetRegionsWidget *ui;
    CGetRegionsWidgetPrivate *d;
    CSHDetect::CError err;
};

#endif // CGETREGIONSWIDGET_H
