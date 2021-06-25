#include "myglwidget.h"

MyGLWidget::MyGLWidget(QWidget * parent) : QOpenGLWidget(parent){
    lidar1_ptcld.clear();
    mouse_last_pose = QPointF(0, 0);
    mouse_left_pressed = false;
    mouse_middle_pressed = false;
}
MyGLWidget::~MyGLWidget(){}

void MyGLWidget::initializeGL(){
    initializeOpenGLFunctions();
    glClearColor(28.0/255.0, 40.0/255.0, 79.0/255.0, 0.5f);
    glEnable(GL_DEPTH_TEST);
    m_program = new QOpenGLShaderProgram(this); 
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "shader/vertex_shader.vs");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "shader/fragment_shader.fs");
    m_program->link();
    m_model_loc = m_program->uniformLocation("model");
    m_view_loc = m_program->uniformLocation("view");
    m_projection_loc = m_program->uniformLocation("projection");
    m_color_loc = m_program->uniformLocation("input_color");

    m_color_bool = m_program->uniformLocation("color_bool");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    roll = 0.0f;
    pitch = 0.0f;
    yaw = 0.0f;

    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    
}
void MyGLWidget::resizeGL(int w, int h){
    width = w;
    height = h;
    glViewport(0, 0, w, h);
}
void MyGLWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(28.0/255.0, 40.0/255.0, 79.0/255.0, 0.5f);


    QMatrix4x4 model;
    QMatrix4x4 view;
    QMatrix4x4 projection;
    projection.perspective(45.0f, width/height, 0.1f, 100.0f);

    view = cam_pose.view;

    model.setToIdentity();

    m_program->bind();
    m_program->setUniformValue(m_model_loc, model);
    m_program->setUniformValue(m_view_loc, view);
    m_program->setUniformValue(m_projection_loc, projection);
    m_program->setUniformValue(m_color_bool, 1);

    int point1_num = lidar1_ptcld.size();

    if(point1_num == 0) {
        m_program->release();

    } else {

        float vertices[3*point1_num];

        for(int i = 0; i < point1_num; ++i) {
            vertices[3*i] = lidar1_ptcld[i][0];
            vertices[3*i+1] = lidar1_ptcld[i][1];
            vertices[3*i+2] = lidar1_ptcld[i][2];
        }


        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        QVector3D color = QVector3D(1.0, 0.0, 0.0);

        m_program->setUniformValue(m_color_loc, color);

        glDrawArrays(GL_POINTS, 0, point1_num);

        // m_program->release();
    }

    QQuaternion quaternion = QQuaternion::fromEulerAngles(roll, pitch, yaw);
    model.translate(x, y, z);
    model.rotate(quaternion);


    // m_program->bind();
    m_program->setUniformValue(m_model_loc, model);
    m_program->setUniformValue(m_view_loc, view);
    m_program->setUniformValue(m_projection_loc, projection);
    m_program->setUniformValue(m_color_bool, 1);

    int point2_num = lidar2_ptcld.size();

    if(point2_num == 0) {
        m_program->release();

    } else {

        float vertices[3*point2_num];

        for(int i = 0; i < point2_num; ++i) {
            vertices[3*i] = lidar2_ptcld[i][0];
            vertices[3*i+1] = lidar2_ptcld[i][1];
            vertices[3*i+2] = lidar2_ptcld[i][2];
        }


        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        QVector3D color = QVector3D(1.0, 1.0, 0.0);
        m_program->setUniformValue(m_color_loc, color);

        glDrawArrays(GL_POINTS, 0, point2_num);

    }

    m_program->release();


}


void MyGLWidget::paintstuff(){
    paintGL();
    update();
}


void MyGLWidget::set_roll(float roll_){
    roll = roll_;
}
void MyGLWidget::set_pitch(float pitch_){
    pitch = pitch_;
}
void MyGLWidget::set_yaw(float yaw_){
    yaw = yaw_;
}
void MyGLWidget::set_x(float x_){
    x = x_;
}
void MyGLWidget::set_y(float y_){
    y = y_;
}
void MyGLWidget::set_z(float z_){
    z = z_;
}


void MyGLWidget::mousePressEvent(QMouseEvent *event){
    mouse_last_pose = event->pos();
    if(event->button() == Qt::LeftButton) {
        mouse_left_pressed = true;
    }
    if(event->button() == Qt::MidButton) {
        mouse_middle_pressed = true;
    }
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton) {
        mouse_left_pressed = false;
    }
    if(event->button() == Qt::MidButton) {
        mouse_middle_pressed = false;
    }
}


void MyGLWidget::mouseMoveEvent(QMouseEvent *event){
    float delta_x = event->x() - mouse_last_pose.x();
    float delta_y = event->y() - mouse_last_pose.y();
    if(mouse_left_pressed){
        cam_pose.mouse_movement_update(delta_x, delta_y);
        paintGL();
        update();
    }
    if(mouse_middle_pressed) {
        cam_pose.mouse_scroll_movement_update(delta_x, delta_y);
        paintGL();
        update();
    }
    mouse_last_pose = event->pos();
}


void MyGLWidget::wheelEvent(QWheelEvent * event) {
    float delta_zoom = event->angleDelta().y();
    cam_pose.mouse_scroll_update(delta_zoom);
    paintGL();
    update();

}