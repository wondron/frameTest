#pragma once
#include <QWidget>
#include "halconcpp/HalconCpp.h"
#include "cgetcustrappos.h"

namespace Ui {
class CGetCuStrapPosWidget;
}

namespace CSHDetect {

class CGetCuStrapPosWidgetPrivate;
class CGetCuStrapPosWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CGetCuStrapPosWidget(QWidget *parent = nullptr);

    ~CGetCuStrapPosWidget();

    void setRegions(const RegionS& region);

    void setImage(const HalconCpp::HObject& obj);
    void setPam(CGetCuStrapPos* pam);

private slots:
    void on_btn_detect_clicked();

    void on_btn_getMidRegion_clicked();

    void on_btn_getCuPos_clicked();

    void on_btn_pamRead_clicked();

private:
    Ui::CGetCuStrapPosWidget *ui;
    CGetCuStrapPosWidgetPrivate* d;
};

}

