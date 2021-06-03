#pragma once
#include <QWidget>
#include "../commonUsage/cerror.h"
#include "../commonUsage/comStruct.h"
#include "cgetcircleLen.h"

namespace Ui {
class CGetCircleLenWidget;
}

namespace CSHDetect {

class CGetCircleLenWidgetPrivate;

class CGetCircleLenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CGetCircleLenWidget(QWidget *parent = nullptr);
    ~CGetCircleLenWidget();

    void setRegions(const RegionS& region);
    /**
     * @brief setPam: 设置检测参数对象
     */
    void setPam(CGetCircleLen* pam);

private slots:

    void on_btn_circleDetect_clicked();

    void on_btn_lineDetect_clicked();

    void on_btn_test_clicked();

private:
    Ui::CGetCircleLenWidget *ui;
    CGetCircleLenWidgetPrivate* d;
};
}
