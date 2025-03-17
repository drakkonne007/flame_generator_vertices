#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gview.h"

#include <QFile>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_widthEdit_textChanged(const QString &arg1);
    void on_heightEdit_textChanged(const QString &arg1);
    void on_openImage_clicked();
    void createAnchors(const QPoint& point = QPoint(-100,-100));
    void on_clearPoints_clicked();
    void on_savePoints_clicked();
    void on_yAnchor_textEdited(const QString &arg1);
    void on_xAnchor_textEdited(const QString &arg1);

private:
    Ui::MainWindow *ui;
    GView m_gview;
    QHash<QString,QString> m_settingsHash;

    QPointF m_percentAnch = QPointF(0.5,0.5);
    QPoint m_anchPoint;

    QFile m_settings;
    int m_spriteWidth = 0;
    int m_spriteHeight = 0;
    QString m_lastFolder;
    void drawLines();
    QSize m_imageSize;

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
};
#endif // MAINWINDOW_H
