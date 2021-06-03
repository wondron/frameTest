#pragma once

#include <QWidget>
#include "reverseSide/creversesidedetect.h"

namespace Ui {
class totalTestWidgetR;
}

class totalTestWidgetRPrivate;

class totalTestWidgetR : public QWidget
{
    Q_OBJECT

public:
    explicit totalTestWidgetR(QWidget *parent = nullptr);
    ~totalTestWidgetR();

    void setPam(CSHDetect::CReverseSideDetect* pam);

private slots:
    void getFilename(const QString& file);

    void on_btn_getFilePath_clicked();

    void OnPamSetDetectDone();

private:
    int getImage(const QString &file);

private:
    Ui::totalTestWidgetR *ui;
    totalTestWidgetRPrivate *d;
};

