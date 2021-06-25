#pragma once
#include <QWidget>
#include "../commonUsage/cerror.h"
#include "../commonUsage/comStruct.h"
#include "ccheckhanreg.h"

namespace Ui {
class CCheckHanRegWidget;
}

namespace CSHDetect {

class CCheckHanRegWidgetPrivate;

class CCheckHanRegWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CCheckHanRegWidget(QWidget* parent = nullptr);
    ~CCheckHanRegWidget();

    CError setRegions(const ReverRegionS& region);

    ReverRegionS& getRegions();

    void setPam(CCheckHanReg* pam);

    CError detect();

signals:
    void detectDone();

private slots:
    void on_btn_detect_clicked();
    CError checkDyn();

private:

private:
    Ui::CCheckHanRegWidget* ui;
    CCheckHanRegWidgetPrivate* d;
};

}

