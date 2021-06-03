#pragma once
#include <QWidget>
#include "cgettapeangle.h"

namespace Ui {
class CGetTapeAngleWidget;
}

namespace CSHDetect {

class CGetTapeAngleWidgetPrivate;

class CGetTapeAngleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CGetTapeAngleWidget(QWidget* parent = nullptr);
    ~CGetTapeAngleWidget();

    void setRegions(const ReverRegionS& region);

    void setPam(CGetTapeAngle* pam);

private:
    HLineData getUpPam();

    HLineData getDnPam();

    VLineData getVPam();

private slots:
    void on_pushButton_clicked();

    void on_btn_singleDetct_clicked();

    void on_btn_vDetect_clicked();

    void on_btn_upDetect_clicked();

    void on_btn_doenDetect_clicked();

    void on_btn_detect_clicked();

private:
    Ui::CGetTapeAngleWidget* ui;
    CGetTapeAngleWidgetPrivate* d;
};


}
