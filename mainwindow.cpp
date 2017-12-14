#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    offset = ui->sliderOffset->value();
    iterations = ui->spinBoxIterations->value();

    originalImage = QImage(":/img/lenna.png");

    updateWindowImages();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_sliderOffset_valueChanged(int value)
{
    offset = value;

    timer.start();
    ui->labelBlurredImage->setPixmap(QPixmap::fromImage(GLBluring.blurImage_kawase(shownImage, offset, iterations)));
    ui->labelRenderTime->setText(QString::number(timer.nsecsElapsed() / 1000000.) + " ms");
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateWindowImages();
}

void MainWindow::on_buttonOpenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty())
    {
        QImageReader reader(fileName);
        if (reader.canRead())
        {
            originalImage = QImage(fileName);
            updateWindowImages();
        }
    }
}

void MainWindow::on_spinBoxIterations_valueChanged(int arg1)
{
    iterations = arg1;
    timer.start();
    ui->labelBlurredImage->setPixmap(QPixmap::fromImage(GLBluring.blurImage_kawase(shownImage, offset, iterations)));
    ui->labelRenderTime->setText(QString::number(timer.nsecsElapsed() / 1000000.) + " ms");
}

void MainWindow::updateWindowImages()
{
    shownImage = originalImage.scaled(ui->labelOriginalImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    ui->labelOriginalImage->setPixmap(QPixmap::fromImage(shownImage));
    timer.start();
    ui->labelBlurredImage->setPixmap(QPixmap::fromImage(GLBluring.blurImage_kawase(shownImage, offset, iterations)));
    ui->labelRenderTime->setText(QString::number(timer.nsecsElapsed() / 1000000.) + " ms");
}
