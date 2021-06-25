#include "citemlistwidget.h"
#include "ui_citemlistwidget.h"
#include "qdir.h"
#include "qdebug.h"
#include "qimage.h"

void CGetImageThread::run()
{
    for(int i = 0 ; i< fileList.size() ; i++){
        QImage img(fileList[i].absoluteFilePath());
        qreal pixra = (((qreal)img.height()) / img.width());
        img.scaled(QSize(imageSize, (int)imageSize * pixra));
        emit sendPixmap(img, fileList[i].fileName());
        QThread::msleep(100);
    }
}

class CItemListWidgetPrivate
{
public:
    CItemListWidgetPrivate(){
    }

    QDir dir;
    QString filePath;
    QStringList filter;
    QFileInfoList infoList;

    int currentIndex = 0;
    bool isLoading = false;
};

CItemListWidget::CItemListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CItemListWidget),
    d(new CItemListWidgetPrivate())
{
    ui->setupUi(this);

    thread = new CGetImageThread();
    ui->listWidget->setIconSize(QSize(IMGSIZE, IMGSIZE));
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setSpacing(10);

    connect(thread, SIGNAL(sendPixmap( QImage, const QString&)),
                this, SLOT(getImage(QImage, const QString&)));
}

CItemListWidget::~CItemListWidget()
{
    delete ui;
}

int CItemListWidget::setFilePath(const QString &filepath)
{
    try {
        QDir dir(filepath);
        if( !dir.exists(filepath))
            return 1;

        dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        dir.setSorting(QDir::Name);

        for(auto i : d->filter)
            qDebug()<<"filters:"<< i;

        if(d->filter.size())
            dir.setNameFilters(d->filter);

        d->filePath = filepath;
        d->dir = dir;

        d->infoList = dir.entryInfoList();

        int listsize = this->width() - 120;

        d->isLoading = true;
        ui->listWidget->clear();
        d->isLoading = false;

        thread->setFileList(d->infoList);
        thread->setMapSize(listsize);
        thread->start();

        return 0;
    }  catch (...) {
        qDebug() << "CItemListWidget::setFilePath happen unexception";
        return 1;
    }
}

const QString CItemListWidget::currentFilePath() const
{
    return d->filePath;
}

void CItemListWidget::setFilter(const QString &filter)
{
    d->filter = filter.split(QRegExp("[ ]"));
}

QString CItemListWidget::elidedLineText(QWidget *pWidget, int nLine, QString strText)
{
    if(nLine == 0)
           return "";

       QFontMetrics fontMetrics(pWidget->font());

       if(nLine == 1){
           return fontMetrics.elidedText(strText,Qt::ElideMiddle,pWidget->width()/2);
       }

       QStringList strListLine;
       for(int i = 0; i < strText.size(); i++){
           if(fontMetrics.width(strText.left(i)) >= pWidget->width()/2){ //不知为何除2才显示正常
               strListLine.append(strText.left(i));
               strText = strText.right(strText.size() - i);
               i = 0;
           }
       }
       if(!strText.isEmpty()){
           strListLine.append(strText);
       }

       bool bHasElided = false; //标记是否有省略内容
       while(strListLine.count() > nLine){
           if(strListLine.count() >= 2){
               strListLine.removeAt(strListLine.count()-2);
               bHasElided = true;
           }
       }

       if(bHasElided && !strListLine.isEmpty()){
           QString strLast = strListLine.last();
           QString strElided = "...";
           strLast.insert(0,strElided);
           while(fontMetrics.width(strLast) >= pWidget->width()/2){
               strLast = strLast.replace(strElided.size(),1,"");
           }

           strListLine.replace(strListLine.count() - 1,strLast);
       }
       QString strResult = strListLine.join("\n");

       return strResult;
}

void CItemListWidget::on_listWidget_currentRowChanged(int currentRow)
{
    if(d->isLoading) return;

    QString filename = d->infoList[currentRow].absoluteFilePath();
    d->currentIndex = currentRow;
    emit currentFileName(filename);
}

void CItemListWidget::on_btn_up_clicked()
{
    if(d->isLoading) return;
    if(d->currentIndex <= 0) return;
    ui->listWidget->setCurrentRow(--d->currentIndex);
}

void CItemListWidget::on_btn_down_clicked()
{
    if(d->isLoading) return;
    if (d->currentIndex + 1 == ui->listWidget->count()) return;
    ui->listWidget->setCurrentRow(++d->currentIndex);
}

void CItemListWidget::getImage(QImage map, const QString &filename)
{
    QString title = elidedLineText(this, 2, filename);

    QListWidgetItem *item = new QListWidgetItem(QIcon(QPixmap::fromImage(map)), title);
    item->setToolTip(filename);
    ui->listWidget->addItem(item);
}
