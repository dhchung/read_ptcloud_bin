#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H
#include <QOpenGLWidget>
// #include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QQuaternion>
#include <vector>
#include <QMouseEvent>
#include <QWheelEvent>
#include <math.h>
#include <iostream>

struct CameraPosition{
    float mouse_sensitivity = 0.1;
    float scroll_sensitivity = 0.001;
    float scroll_move_sensitivity = 0.01;


    QVector3D up = QVector3D(0.0, 0.0, 1.0);
    QVector3D center = QVector3D(0.0, 0.0, 0.0);
    float distance = 4.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;

    float x = center.x() + distance * cos(pitch * M_PI/180.0) * cos(yaw * M_PI/180.0);
    float y = center.y() + distance * cos(pitch * M_PI/180.0) * sin(yaw * M_PI/180.0);
    float z = center.z() + distance * sin(pitch * M_PI/180.0);

    QVector3D position = QVector3D(x, y ,z);
    QMatrix4x4 view;

    CameraPosition(){
        update_camera_matrix();
    }


    void mouse_movement_update(float dx, float dy){
        yaw -= dx * mouse_sensitivity;
        pitch += dy * mouse_sensitivity;

        if(abs(pitch) > 89.9) {
            pitch = pitch/abs(pitch) * 89.9;
        }


        update_camera_matrix();
    }

    void mouse_scroll_movement_update(float dx, float dy) {

        QVector3D front = center - position;
        front.normalize();

        QVector3D right = QVector3D::crossProduct(front, up);
        right.normalize();

        QVector3D cam_up = QVector3D::crossProduct(right, front);
        cam_up.normalize();

        center -= scroll_move_sensitivity * right * dx -
                  scroll_move_sensitivity * cam_up * dy;


        update_camera_matrix();
    }

    void cout_vector(QVector3D a){
        std::cout<<"["<<a.x()<<", "<<a.y()<<", "<<a.z()<<"]"<<std::endl;
    }



    void mouse_scroll_update(float dscroll) {
        distance -= scroll_sensitivity * dscroll;
        if(distance < 0.1){
            distance = 0.1;
        }
        update_camera_matrix();
    }

    void update_camera_matrix(){
        x = center.x() + distance * cos(pitch * M_PI/180.0) * cos(yaw * M_PI/180.0);
        y = center.y() + distance * cos(pitch * M_PI/180.0) * sin(yaw * M_PI/180.0);
        z = center.z() + distance * sin(pitch * M_PI/180.0);
        position = QVector3D(x, y ,z);
        view.setToIdentity();
        view.lookAt(position, center, up);
    }


};


class MyGLWidget : public QOpenGLWidget, public QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    MyGLWidget(QWidget * parent = nullptr);
    ~MyGLWidget();

    void paintstuff();

    std::vector<std::vector<float>> lidar1_ptcld;
    std::vector<std::vector<float>> lidar2_ptcld;


signals:

public slots:
    void set_roll(float roll_);
    void set_pitch(float pitch_);
    void set_yaw(float yaw_);
    void set_x(float x_);
    void set_y(float y_);
    void set_z(float z_);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent * event) override;

private:
    QOpenGLShaderProgram * m_program;

    GLuint m_model_loc;
    GLuint m_view_loc;
    GLuint m_projection_loc;
    GLuint m_color_loc;

    GLuint m_color_bool;

    unsigned int VBO, VAO, EBO, FBO;

    int width;
    int height;
    
    float roll;
    float pitch;
    float yaw;
    float x;
    float y;
    float z; 

    QPointF mouse_last_pose;
    bool mouse_left_pressed;
    bool mouse_middle_pressed;

    CameraPosition cam_pose;

};

#endif // MYGLWIDGET_H
