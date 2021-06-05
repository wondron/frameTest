#ifndef DUANHANWIDGET_H
#define DUANHANWIDGET_H

#include <QWidget>
#include "halconcpp/HalconCpp.h"

namespace Ui {
class duanhanWidget;
}

class duanhanWidgetPrivate;

class duanhanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit duanhanWidget(QWidget *parent = nullptr);
    ~duanhanWidget();

    void initialWindow();

private slots:
    void on_btn_getImg_clicked();

    void on_btn_detect_clicked();

    void on_btn_lapuDetect_clicked();

    void on_btn_polarChange_clicked();

private:
    /**
    * @brief genRingRoi ::生成圆环
    * @param obj：：生成对象
    * @return true:表示成功生成
    */
    bool genRingRoi(HalconCpp::HObject &obj);

private:
    Ui::duanhanWidget *ui;
    duanhanWidgetPrivate *d;
};

#endif // DUANHANWIDGET_H
