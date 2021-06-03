#ifndef HALCONDLG_H
#define HALCONDLG_H

#include <QDialog>

#include "halconcpp/HalconCpp.h"
using namespace HalconCpp;

namespace Ui {
class halconDlg;
}

class halconDlgPrivate;
class halconDlg : public QDialog
{
    Q_OBJECT

public:
    explicit halconDlg(QWidget *parent = nullptr);
    ~halconDlg();

    void setShowImage(HObject obj);
public:
    /**
     * @brief getRegion: 获取粗糙面区域(可能为焊条的区域)。
     * @param img： 检测图片
     * @param obj： 筛选得到的区域
     * @return true：不存在区域， false：存在区域
     */
    bool getRegion(HObject img, HObject & obj, HObject &roiImage);
    void showObj(HObject img, HObject obj);

public slots:
    bool genRingRoi(HObject &obj);

protected:

    //滚轮事件
    void wheelEvent(QWheelEvent *ev);

    //鼠标按下事件
    void mousePressEvent(QMouseEvent *ev);

    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *ev);

    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *ev);

    //事件过滤器
    bool eventFilter(QObject *obj, QEvent *event);

private slots:

    void on_btn_getImg_clicked();

    void on_btn_detect_clicked();

    void on_btn_showRegion_clicked();

    void on_spin_threMin_valueChanged(int arg1);

    void on_spin_smallR_valueChanged(int arg1);

    void on_spin_bigR_valueChanged(int arg1);

    void on_btn_lapuDetect_clicked();

    void on_spin_laputhreMin_valueChanged(int arg1);

    void on_btn_polarChange_clicked();

    void on_spin_minWid_valueChanged(int arg1);

signals:
    void pointClicked(int x, int y);

private:
    Ui::halconDlg *ui;
    halconDlgPrivate *d;
};

#endif // HALCONDLG_H
