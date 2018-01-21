#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    offset = ui->spinBoxOffset->value();
    iterations = ui->spinBoxIterations->value();

    originalImage = QImage(":/img/lenna.png");

    updateShownImage();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonOpenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty()) {
        QImageReader reader(fileName);

        if (reader.canRead()) {
            originalImage = QImage(fileName);
            updateShownImage();
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateShownImage();
}

void MainWindow::on_spinBoxOffset_valueChanged(int arg1)
{
    offset = arg1;
    showBlurredImage();
}

void MainWindow::on_spinBoxIterations_valueChanged(int arg1)
{
    iterations = arg1;
    showBlurredImage();
}

void MainWindow::updateShownImage()
{
    shownImage = originalImage.scaled(ui->labelBlurredImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    showBlurredImage();

    QString textureSize(QString::number(shownImage.size().width()) + "x" + QString::number(shownImage.size().height()));
    ui->labelTextureSize->setText(textureSize);
}

void MainWindow::showBlurredImage()
{
    if (iterations == 0) {
        ui->labelBlurredImage->setPixmap(QPixmap::fromImage(shownImage));
        return;
    }

    ui->labelBlurredImage->setPixmap(QPixmap::fromImage(GLBlurDualKawase.blurImage_DualKawase(shownImage, offset, iterations)));
    ui->labelGPUTime->setText(QString::number(GLBlurDualKawase.getGPUTime()) + " ms");
    ui->labelCPUTime->setText(QString::number(GLBlurDualKawase.getCPUTime()) + " ms");
}
