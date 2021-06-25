#ifndef TOTALTESTWIDGET_H
#define TOTALTESTWIDGET_H

#include <QWidget>
#include "obverseSide/cobversesidedetect.h"

namespace Ui {
class totalTestWidget;
}

class totalTestWidgetPrivate;

class totalTestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit totalTestWidget(QWidget *parent = nullptr);
    ~totalTestWidget();

    void setPam(CSHDetect::CObverseSideDetect* pam);

private slots:
    void on_btn_getFilePath_clicked();

    void getFilename(const QString& file);

    void OnPamSetDetectDone();

    void on_btn_loadPam_clicked();

    void on_btn_detect_clicked();

    void on_btn_algoTest_clicked();

private:
    int getImage(const QString &file);

private:
    Ui::totalTestWidget *ui;
    totalTestWidgetPrivate* d;
};

#endif // TOTALTESTWIDGET_H
