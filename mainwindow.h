#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QImageReader>

#include <QElapsedTimer>

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
    void on_sliderOffset_valueChanged(int value);
    void on_spinBoxIterations_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;

    int offset;
    int iterations;

    GLBlurFunctions GLBluring;
    QImage originalImage;
    QImage shownImage;

    QElapsedTimer timer;

    void resizeEvent(QResizeEvent *event);
    void updateWindowImages();
};

#endif // MAINWINDOW_H
