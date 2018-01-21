#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QImageReader>

#include "glblurfunctions.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_buttonOpenFile_clicked();
    void on_spinBoxIterations_valueChanged(int arg1);
    void on_spinBoxOffset_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;

    int offset;
    int iterations;

    GLBlurFunctions GLBlurDualKawase;
    QImage originalImage;
    QImage shownImage;

    void resizeEvent(QResizeEvent *event);
    void showBlurredImage();
    void updateShownImage();
};

#endif // MAINWINDOW_H
