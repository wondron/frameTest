#ifndef CGETTAPEANGLEPAM_H
#define CGETTAPEANGLEPAM_H

#include <QWidget>
#include "cgettapeangle.h"
#include "../commonUsage/algorithm.h"

namespace Ui {
class CGetTapeAnglePam;
}

namespace CSHDetect {

class CGetTapeAnglePam : public QWidget
{
    Q_OBJECT

public:
    explicit CGetTapeAnglePam(QWidget *parent = nullptr);
    ~CGetTapeAnglePam();

    void getPamValue(MeasureposPam& pam, PosBia& posBia);

    void setPamValue(const MeasureposPam& pam, const PosBia& posBia);

private slots:
    void on_btn_getPam_clicked();

signals:
    void pamChanged();

private:
    Ui::CGetTapeAnglePam *ui;
    bool sendSignal = true;
};

}

#endif // CGETTAPEANGLEPAM_H
