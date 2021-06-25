#ifndef TEST_H
#define TEST_H

#include <QWidget>
#include "commonWidget/cimageviewhalcon.h"

namespace Ui {
class test;
}

class test : public QWidget
{
    Q_OBJECT

public:
    explicit test(QWidget *parent = nullptr);
    ~test();

private slots:
    void on_btn__clicked();

    void on_pushButton_clicked();

private:
    Ui::test *ui;
    Graphics::CImageViewHalcon* widget;
};

#endif // TEST_H
