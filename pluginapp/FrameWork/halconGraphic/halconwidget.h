#pragma once

#include <QWidget>
#include "halconcpp/HalconCpp.h"

namespace Ui {
class halconWidget;
}

namespace Graphics {
class halconWidgetPrivate;

class halconWidget : public QWidget
{
    Q_OBJECT

public:
    explicit halconWidget(QWidget *parent = nullptr);
    enum DrawMode{ FILL = 0, MARGIN = 1};
    ~halconWidget();

public:
    /**
     * @brief setShowImage :设置需要显示的halcon图像
     * @param obj
     */
    bool setShowImage(HalconCpp::HObject obj);

    /**
     * @brief getShowImage: 获取图像
     */
    HalconCpp::HObject getShowImage();

    /**
     * @brief frashWindow :刷新窗口，方式某些obj没有显示。
     */
    void frashWindow();

    /**
     * @brief showObj :显示图像
     * @param img：图像
     * @param obj：region区域
     */
    void showObj(HalconCpp::HObject img, HalconCpp::HObject obj);

    void showObj(HalconCpp::HObject obj);
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

    /**
     * @brief setFont : 设置字体
     * @param fontType: 类型
     * @param size： 大小
     * @param bold： 是否加粗
     * @param slant：倾斜
     */
    void setFont(QString fontType = "Times New Roman", int size = 30, bool bold = false, bool slant = false);

    /**
     * @brief showString :显示文字信息
     * @param str
     * @param pts
     */
    void showMsg(const QString str, const QPoint pts = QPoint(0, 0));

    void showMsg(QList<QString> strList, const QPoint pts = QPoint(0, 0));

    /**
     * @brief getWindowHandle :获取句柄
     */
    HalconCpp::HTuple getWindowHandle();

public slots:

    /**
     * @brief setImageFile:通过设定图像来显示图像
     */
    void setImageFile(const QString& filepath);

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
    void on_radio_margin_clicked();
    void on_radio_fill_clicked();

private:
    Ui::halconWidget *ui;
    halconWidgetPrivate* d;
};

}

