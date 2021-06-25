#pragma once

#include <QWidget>
#include "commonUsage/comStruct.h"
#include "commonUsage/cerror.h"
#include "cgetreregions.h"

namespace Ui {
class CGetReRegionsWidget;
}

namespace CSHDetect {

class CGetReRegionsWidgetPrivate;

class CGetReRegionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CGetReRegionsWidget(QWidget* parent = nullptr);
    ~CGetReRegionsWidget();

    void setPam(CGetReRegions* pam);

    void setImage(HalconCpp::HObject& img, ReverRegionS& regions);

    const ReverRegionS& getRegons() const;

    CError detect();

public slots:
    void setDetectImage(const QString& temPath);

private slots:
    void on_tabWidget_currentChanged(int index);

    void on_btn_battery_clicked();

    void on_btn_mid_clicked();

    void on_btn_tapeNum_clicked();

    void on_btn_dblTape_clicked();

    void on_btn_test_clicked();

private:
    void iniLayout();

signals:
    void detectDone();

private:
    CError err;
    Ui::CGetReRegionsWidget* ui;
    CGetReRegionsWidgetPrivate* d;
};

}

