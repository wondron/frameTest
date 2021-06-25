#ifndef TOTALALG_H
#define TOTALALG_H

#include <QWidget>
#include "totaltestwidget.h"
#include "totaltestwidgetr.h"
#include "test.h"

namespace Ui {
class totalAlg;
}

class totalAlg : public QWidget
{
    Q_OBJECT

public:
    explicit totalAlg(QWidget *parent = nullptr);
    ~totalAlg();

private:
    totalTestWidget* obverWidget;
    totalTestWidgetR* reverWidget;
    test* testWidget;
    Ui::totalAlg *ui;
};

#endif // TOTALALG_H
