#pragma once

#include <QWidget>
#include "halconcpp/HalconCpp.h"

namespace Ui {
class ImageViewerHalcon;
}

namespace Graphics {

class ImageViewerHalconPrivate;
class ImageViewerHalcon : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewerHalcon(QWidget *parent = nullptr);
    enum DrawMode{ FILL = 0, MARGIN = 1};
    ~ImageViewerHalcon();

public:
    /**
     * @brief setShowImage :设置需要显示的halcon图像
     * @param obj
     */
    bool setShowImage(HalconCpp::HObject obj);

    /**
     * @brief showObj :显示图像
     * @param img：图像
     * @param obj：region区域
     */
    void showObj(HalconCpp::HObject img, HalconCpp::HObject obj);

    /**
     * @brief objIsEmpty: 判定obj是否可用
     * @param obj：所要检测的对象
     * @return ：false，可以用（非空集）； true， 空的
     */
    bool objIsEmpty(const HalconCpp::HObject &obj);

    /**
     * @brief tupleisEmpty: 判断Halcon数组是都为空
     * @param tuple：检测对象
     * @return ：false，可以用（非空集）； true， 空的
     */
    bool tupleisEmpty(const HalconCpp::HTuple &tuple);

    /**
     * @brief setDrawMode：设置region的显示方式
     * @param Mode : 0填充，1边界
     */
    void setDrawMode(const int Mode);

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

signals:
    void pointClicked(int x, int y);

private slots:

private:
    Ui::ImageViewerHalcon *ui;
    ImageViewerHalconPrivate *d;
};

}

