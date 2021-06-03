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

    void setRegions(const ReverRegionS& region);

    void setPam(CCheckHanReg* pam);

private slots:
    void on_btn_detect_clicked();

    void checkDyn();

private:

private:
    Ui::CCheckHanRegWidget* ui;
    CCheckHanRegWidgetPrivate* d;
};

}

