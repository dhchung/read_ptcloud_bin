#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QElapsedTimer>
#include <QTimer>

#include <string>
#include <vector>

#include <iostream> 
#include <fstream>
#include <filesystem>
#include <numeric>
#include <algorithm>

#include "myglwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
public:

    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    std::string dir_path;

    QElapsedTimer q_elaspedtime;
    QTimer * q_timer;

private slots:
    void TimeLineSliderMoved(int position);
    void TimeLineSliderPressed();
    void TimeLineSliderReleased();
    void LoadButtonClicked();

    void UpdateTime();

private:
    Ui::MainWindow *ui;

    std::vector<int> sort_indices(const std::vector<long double> &v);
    void LoadTimeLine(std::string & dir_path);

    long double curr_time;
    long double min_time;
    long double max_time;

    long double start_time;

    std::vector<long double> times;
    std::vector<std::string> file_dir;

    bool loaded;

    int FindLastIdx(long double time);
};
#endif // MAINWINDOW_H
