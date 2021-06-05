#ifndef CLEANCIRCLEWIDGET_H
#define CLEANCIRCLEWIDGET_H

#include <QWidget>
#include "imageviewerhalcon.h"

namespace Ui {
class cleanCircleWidget;
}

class cleanCircleWidgetPrivate;

class cleanCircleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit cleanCircleWidget(QWidget *parent = nullptr);
    ~cleanCircleWidget();

private slots:
    void on_btn_getImage_clicked();
    void on_btn_detect_clicked();
    void recivePoint(int x, int y);

private:
    Ui::cleanCircleWidget *ui;
    cleanCircleWidgetPrivate *d;
};

#endif // CLEANCIRCLEWIDGET_H
