#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    q_timer = new QTimer(this);

    connect(q_timer, SIGNAL(timeout()), this, SLOT(UpdateTime()));

    connect(ui->timeLineSlider, SIGNAL(valueChanged(int)), this, SLOT(TimeLineSliderMoved(int)));
    connect(ui->timeLineSlider, SIGNAL(sliderReleased()), this, SLOT(TimeLineSliderReleased()));
    connect(ui->timeLineSlider, SIGNAL(sliderPressed()), this, SLOT(TimeLineSliderPressed()));
    connect(ui->loadButton, SIGNAL(clicked()), this, SLOT(LoadButtonClicked()));
    connect(ui->playpushButton, SIGNAL(clicked()), this, SLOT(PlayPushButtonClicked()));
    m_loaded = false;
    m_play = false;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::TimeLineSliderMoved(int position)
{
    ui->percentlabel->setText(QString::number(position));
}

void MainWindow::LoadButtonClicked()
{
    QString dir;
    dir = QFileDialog::getExistingDirectory(this, "Choose Directory", QDir::currentPath(), QFileDialog::ShowDirsOnly);
    ui->dirlabel->setText(dir);
    dir_path = dir.toUtf8().constData();
    LoadTimeLine(dir_path);
}

std::vector<int> MainWindow::sort_indices(const std::vector<long double> &v) {
    std::vector<int> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);
    
    std::stable_sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});
    return idx;
}


void MainWindow::LoadTimeLine(std::string & dir_path){
    times.clear();
    file_dir.clear();
    // for(const auto & entry : std::filesystem::directory_iterator(dir_path)){
    //     std::string filename = entry.path();
    //     // size_t binloc = filename.find(dir);
    //     size_t slash_loc = filename.find_last_of("/");
    //     size_t bin_loc = filename.find(".bin");
    //     size_t length = bin_loc - slash_loc -1;
    //     times.push_back(std::stold(filename.substr(slash_loc+1, length)));
    //     file_dir.push_back(filename);
    // }

    DIR *dir = opendir(dir_path.c_str());
    struct dirent *ent;
    // if ((dir = opendir(dir_path.c_str())) != NULL) {
    if(dir!= NULL){
        while ((ent = readdir (dir)) != NULL) {
            std::string filename = std::string(ent->d_name);
            if(filename=="." || filename=="..") {
                continue;
            }

            // size_t binloc = filename.find(dir);
            size_t slash_loc = filename.find_last_of("/");
            size_t bin_loc = filename.find(".bin");
            size_t length = bin_loc - slash_loc -1;
            times.push_back(std::stold(filename.substr(slash_loc+1, length)));
            file_dir.push_back(dir_path + "/" + filename);
        }
        closedir (dir);
    } else {
        perror ("");
    }

    std::vector<int> sorted_idx = sort_indices(times);
    min_time = times[sorted_idx[0]];
    max_time = times[sorted_idx[sorted_idx.size()-1]];

    // for(size_t i = 0; i<times.size(); ++i) {
    //     printf("%0.9Lf\n", times[i]);
    // }

    m_loaded = true;
}

void MainWindow::UpdateTime(){

    int msecs = q_elaspedtime.elapsed();
    curr_time = min_time + (long double)(msecs) * 0.001f - initial_time + start_time;
    if(curr_time > max_time) {
        q_timer->stop();
    }
    int percent = int((curr_time - min_time)/(max_time - min_time)*100.0f);
    ui->timeLineSlider->setSliderPosition(percent);

    char time_char[30];
    sprintf(time_char, "%0.9Lf", curr_time);
    ui->timelabel->setText(QString::fromStdString(std::string(time_char)));

    int last_data_idx = FindLastIdx(curr_time);

    if(last_data_idx!= -1) {
        ui->lastdatalabel->setText(QString::fromStdString(file_dir[last_data_idx]));

        std::vector<std::vector<float>> pc;
        std::string curr_file = file_dir[last_data_idx];
        std::ifstream is(curr_file, std::ios::in | std::ios::binary);

        while(is){
            float x;
            float y;
            float z;
            float intensity;
            uint32_t time;
            uint16_t reflectivity;
            uint16_t ambient;
            uint32_t range;

            is.read((char*)&x, sizeof(float));
            is.read((char*)&y, sizeof(float));
            is.read((char*)&z, sizeof(float));
            is.read((char*)&intensity, sizeof(float));
            is.read((char*)&time, sizeof(uint32_t));
            is.read((char*)&reflectivity, sizeof(uint16_t));
            is.read((char*)&ambient, sizeof(uint16_t));
            is.read((char*)&range, sizeof(uint32_t));
            std::vector<float> point{x, y, z};
            pc.push_back(point);
        }
        ui->renderwindowwidget->lidar1_ptcld = pc;
    }

    ui->renderwindowwidget->resizeGL(ui->renderwindowwidget->width(), ui->renderwindowwidget->height());
    ui->renderwindowwidget->paintstuff();


}

int MainWindow::FindLastIdx(long double time){
    long double min_time = 2000;
    int min_time_idx = -1;

    for(int i = 0; i < times.size(); ++i) {


        if(time - times[i] < 0) {
            continue;
        }
        if(time - times[i] > 0.1) {
            continue;
        }
        if(min_time > time - times[i]) {
            min_time = time - times[i];
            min_time_idx = i;
        }
    }
    return min_time_idx;

}
void MainWindow::TimeLineSliderPressed(){
    q_timer->stop();
}
void MainWindow::TimeLineSliderReleased(){
    // q_elaspedtime.restart();
    if(m_loaded) {
        initial_time = float(q_elaspedtime.elapsed()) * 0.001f;
        start_time = (max_time - min_time)/100.0 * (long double)(ui->timeLineSlider->sliderPosition());
        if(m_play){
            q_timer->start();
        }

        curr_time = min_time + start_time;

        int last_data_idx = FindLastIdx(curr_time);

        if(last_data_idx!= -1) {
            ui->lastdatalabel->setText(QString::fromStdString(file_dir[last_data_idx]));

            std::vector<std::vector<float>> pc;
            std::string curr_file = file_dir[last_data_idx];
            std::ifstream is(curr_file, std::ios::in | std::ios::binary);

            while(is){
                float x;
                float y;
                float z;
                float intensity;
                uint32_t time;
                uint16_t reflectivity;
                uint16_t ambient;
                uint32_t range;

                is.read((char*)&x, sizeof(float));
                is.read((char*)&y, sizeof(float));
                is.read((char*)&z, sizeof(float));
                is.read((char*)&intensity, sizeof(float));
                is.read((char*)&time, sizeof(uint32_t));
                is.read((char*)&reflectivity, sizeof(uint16_t));
                is.read((char*)&ambient, sizeof(uint16_t));
                is.read((char*)&range, sizeof(uint32_t));
                std::vector<float> point{x, y, z};
                pc.push_back(point);
            }
            ui->renderwindowwidget->lidar1_ptcld = pc;
        }

        ui->renderwindowwidget->resizeGL(ui->renderwindowwidget->width(), ui->renderwindowwidget->height());
        ui->renderwindowwidget->paintstuff();

    }

}

void MainWindow::PlayPushButtonClicked(){
    if(m_loaded) {
        if(!m_play) {

            q_timer->start(1);
            q_elaspedtime.start();
            initial_time = (long double)(q_elaspedtime.elapsed()) * 0.001f;

            start_time = (max_time - min_time)/100.0 * (long double)(ui->timeLineSlider->sliderPosition());
            curr_time = min_time + start_time;

            // initial_time = float(q_elaspedtime.elapsed()) * 0.001f;
            // start_time = (max_time - min_time)/100.0 * (long double)(ui->timeLineSlider->sliderPosition());
            // q_timer->start();
            m_play = true;        
            ui->playpushButton->setText("Stop");

        } else {
            q_timer->stop();
            m_play = false;
            ui->playpushButton->setText("Play");
        }
    }
}