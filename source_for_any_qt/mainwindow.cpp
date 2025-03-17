#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QClipboard>

#include <QFileDialog>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainLay->insertWidget(0,&m_gview,1);
    m_gview.setScene(new QGraphicsScene(this));
    centralWidget()->setLayout(ui->mainLay);
    m_settings.setFileName("imageCollision.ini");
    m_settings.open(QFile::ReadWrite);
    auto lines = m_settings.readAll();
    auto linesList = QString(lines).split("\n");
    for(auto line : linesList)
    {
        auto keyValue = line.split("=");
        if(keyValue.size() == 2)
        {
            m_settingsHash[keyValue[0]] = keyValue[1];
        }
    }
    m_lastFolder = m_settingsHash["lastFolder"];
    connect(&m_gview,&GView::createAnchor,this,&MainWindow::createAnchors);
}

MainWindow::~MainWindow()
{
    delete ui;
    m_settings.close();
}

void MainWindow::on_widthEdit_textChanged(const QString &arg1)
{
    if(arg1.toInt() <= 0){
        m_spriteWidth = 0;
        return;
    }
    m_spriteWidth = m_imageSize.width() / arg1.toInt();
    m_gview.changeAnchor(QPointF(m_spriteWidth * m_percentAnch.x(),m_spriteHeight * m_percentAnch.y()));
    drawLines();
}


void MainWindow::on_heightEdit_textChanged(const QString &arg1)
{
    if(arg1.toInt() <= 0){
        m_spriteHeight = 0;
        return;
    }
    m_spriteHeight = m_imageSize.height() / arg1.toInt();
    m_gview.changeAnchor(QPointF(m_spriteWidth * m_percentAnch.x(),m_spriteHeight * m_percentAnch.y()));
    drawLines();
}

void MainWindow::drawLines()
{
    for(auto item : m_gview.m_coordLines){
        delete item;
    }
    QPen pen;
    pen.setColor(Qt::red);
    pen.setStyle(Qt::DotLine);
    pen.setWidth(1);
    m_gview.m_coordLines.clear();
    for(int i = 0; i <  m_imageSize.width() + 1 && m_spriteWidth != 0; i += m_spriteWidth){
        m_gview.m_coordLines << m_gview.scene()->addLine(i,0,i,m_imageSize.height(),pen);
    }
    for(int i = 0; i <  m_imageSize.height() + 1 && m_spriteHeight != 0; i += m_spriteHeight){
        m_gview.m_coordLines << m_gview.scene()->addLine(0,i,m_imageSize.width(),i,pen);
    }
    // createAnchors();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(m_gview.m_points.isEmpty()){
        return;
    }
    if(Qt::Key_Z == event->key() && (Qt::ControlModifier & event->modifiers())){
        delete m_gview.m_points.last();
        m_gview.m_points.resize(m_gview.m_points.size() - 1);
    }
    m_gview.refreshTempLines();
}

void MainWindow::on_openImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open Image"), m_lastFolder, tr("Image Files (*.png *.jpg *.bmp *.webp)"));
    if(fileName == ""){
        return;
    }
    m_lastFolder = fileName;
    m_settingsHash["lastFolder"] = m_lastFolder;
    m_settings.remove();
    m_settings.open(QFile::ReadWrite | QFile::Append);
    for(auto key : m_settingsHash.keys())
    {
        m_settings.write(key.toUtf8() + "=" + m_settingsHash[key].toUtf8() + "\n");
    }
    m_settings.close();
    m_gview.scene()->clear();
    m_gview.clearInternalData();
    m_gview.scene()->addPixmap(QPixmap(fileName));
    m_gview.scene()->setSceneRect(QPixmap(fileName).rect());
    m_imageSize = QPixmap(fileName).size();
    ui->widthEdit->setText("");
    ui->heightEdit->setText("");
    ui->xAnchor->setText("0.5");
    ui->yAnchor->setText("0.5");
    // drawLines();
    ui->imgSizeLabel->setText("W: " + QByteArray::number(m_imageSize.width()) + ",H: " + QByteArray::number(m_imageSize.height()));
    m_gview.m_isLoadImg = true;
}

void MainWindow::createAnchors(const QPoint &point)
{
    if(m_spriteWidth == 0 || m_spriteHeight == 0){
        return;
    }
    if(point.x() != -100 && point.y() != -100){
        m_anchPoint = point;
    }
    double newX = (point.x() / m_spriteWidth) * m_spriteWidth;
    double newY = (point.y() / m_spriteHeight) * m_spriteHeight;
    m_percentAnch.setX(((double)point.x() - newX)/ m_spriteWidth);
    m_percentAnch.setY(((double)point.y() - newY) / m_spriteHeight);
    ui->xAnchor->setText(QString::number(m_percentAnch.x()));
    ui->yAnchor->setText(QString::number(m_percentAnch.y()));
}


void MainWindow::on_clearPoints_clicked()
{
    for(auto item : m_gview.m_points){
        delete item;
    }
    m_gview.m_points.clear();
    for(auto item : m_gview.m_tempRectLines){
        delete item;
    }
    m_gview.m_tempRectLines.clear();
    m_gview.m_underMouseCircle = nullptr;
}


void MainWindow::on_savePoints_clicked()
{
    QByteArray copyToClip = "[\n";
    for(const auto &item : m_gview.m_points){
        double newX = item->boundingRect().center().x() - ((int)item->boundingRect().center().x() / m_spriteWidth) * m_spriteWidth;
        double newY = item->boundingRect().center().y() - ((int)item->boundingRect().center().y() / m_spriteHeight) * m_spriteHeight;
        copyToClip += "Vector2(" + QString::number(newX - m_percentAnch.x() * m_spriteWidth) + "," + QString::number(newY - m_percentAnch.y() * m_spriteHeight) + ")\n,";
    }
    copyToClip += "];";
    QApplication::clipboard()->setText(copyToClip);
}

void MainWindow::on_yAnchor_textEdited(const QString &arg1)
{
    bool isOk;
    double value = arg1.toDouble(&isOk);
    if(!isOk){
        QString temp = arg1;
        temp = temp.replace(',', '.');
        value = temp.toDouble(&isOk);
        if(!isOk){
            return;
        }
    }
    m_percentAnch.setY(value);
    m_gview.changeAnchor(QPointF(m_spriteWidth * m_percentAnch.x(),m_spriteHeight * m_percentAnch.y()));
}


void MainWindow::on_xAnchor_textEdited(const QString &arg1)
{
    bool isOk;
    double value = arg1.toDouble(&isOk);
    if(!isOk){
        QString temp = arg1;
        temp = temp.replace(',', '.');
        value = temp.toDouble(&isOk);
        if(!isOk){
            return;
        }
    }
    m_percentAnch.setX(value);
    m_gview.changeAnchor(QPointF(m_spriteWidth * m_percentAnch.x(),m_spriteHeight * m_percentAnch.y()));
}

