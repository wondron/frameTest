#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Echonterface.h"

QT_BEGIN_NAMESPACE

namespace Ui { class MainWindow; }
class QString;
class QLineEdit;
class QLabel;
class QPushButton;
class QGridLayout;

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendEcho();

private:
    Ui::MainWindow *ui;
    void createGUI();
    //加载插件
    bool loadPlugin();

    EchoInterface *echoInterface;
    QLineEdit *lineEdit;
    QLabel *label;
    QPushButton *button;
    QGridLayout *layout;
};
#endif // MAINWINDOW_H
