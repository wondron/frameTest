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
    CError detect();

private:
    void initialWidget();

private slots:
    void on_btn_singleDetct_clicked();
    void on_btn_detect_clicked();
    void on_cmbx_quadrant_currentIndexChanged(int index);
    void getPam();
    void on_btn_pamRead_clicked();

private:
    Ui::CGetTapeAngleWidget* ui;
    CGetTapeAngleWidgetPrivate* d;
};


}
