#ifndef CITEMLISTWIDGET_H
#define CITEMLISTWIDGET_H

#include <QWidget>
#include "QFileInfoList"
#include "qthread.h"

namespace Ui {
class CItemListWidget;
}

class CGetImageThread: public QThread
{
    Q_OBJECT
public:
    CGetImageThread(){}
    void setFileList(const QFileInfoList& list){
        fileList = list;
    }

    void setMapSize(const int size){
        imageSize = size;
    }

protected:
    void run();

signals:
    void sendPixmap(QImage map, const QString& filename);

private:
    int imageSize;
    QFileInfoList fileList;
};

class CItemListWidgetPrivate;
class CItemListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CItemListWidget(QWidget *parent = nullptr);
    ~CItemListWidget();

    enum {CMBXSIZE = 15, ICONSIZE = 25, IMGSIZE = 128};

    int setFilePath(const QString& filepath);
    const QString currentFilePath() const;

    void setFilter(const QString& filter);

    QString elidedLineText(QWidget *pWidget, int nLine, QString strText);

signals:
    void currentFileName(const QString& fileName);

public slots:
    void getImage(QImage map, const QString& filename);

private slots:
    void on_listWidget_currentRowChanged(int currentRow);

    void on_btn_up_clicked();

    void on_btn_down_clicked();


private:
    Ui::CItemListWidget *ui;
    CItemListWidgetPrivate* d;
    CGetImageThread* thread;
};

#endif // CITEMLISTWIDGET_H
