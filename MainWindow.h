#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

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
    void slotDrawPlot( const QModelIndex &index1 = QModelIndex(),
                       const QModelIndex &index2 = QModelIndex() );

private slots:
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
    const int m_nrows;
    const int m_ncols;
    QStandardItemModel m_model;
};

#endif // MAINWINDOW_H
