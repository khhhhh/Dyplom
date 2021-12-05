#include "widget.h"
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <algorithm>

Widget::~Widget()
{
}

void Widget::initializeGL() {
    //frame = 0;
    de = 0;
    angle = 0.0f;
    ax=ay=dax=day=0;

    lightPosition = {10, 5, 0};

    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    camera = new Camera();

    Mesh *meshTree, *meshTwig;
    meshTwig = new Mesh(GL_TRIANGLES);

    Texture *treeTex, *twigTex;
    treeTex = new Texture();
    twigTex = new Texture();
    treeTex->loadFromImage("C:/Users/al/Desktop/Dyplom/glapp_z7/glapp/tree.png");
    twigTex->loadFromImage("C:/Users/al/Desktop/Dyplom/glapp_z7/glapp/twig1.png");
    textures.push_back(treeTex);
    textures.push_back(twigTex);

    meshTree = Mesh::generateTree(*meshTwig);
    meshTree->pos = {2, 0, -1};
    meshTwig->pos = {2, 0, -1};
    meshes.push_back(meshTree);
    meshes.push_back(meshTwig);

    meshTree->material.ambient = vec3{ 0.329412f, 0.223529f, 0.027451f };
    meshTree->material.diffuse = vec3{ 0.780392f, 0.568627f, 0.113725f };
    meshTree->material.specular = vec3{ 0.992157f, 0.941176f, 0.807843f };
    meshTree->material.shiness = 27.8974f;

    meshTwig->material.shiness = 27.8974f;

    gourardProgram = new GLSLProgram();
    gourardProgram->compileShaderFromFile("C:/Users/al/Desktop/Dyplom/glapp_z7/glapp/vshader.vert", GL_VERTEX_SHADER);
    gourardProgram->compileShaderFromFile("C:/Users/al/Desktop/Dyplom/glapp_z7/glapp/fshader.fsh", GL_FRAGMENT_SHADER);
    gourardProgram->link();

    phongProgram = new GLSLProgram();
    phongProgram->compileShaderFromFile("C:/Users/al/Desktop/Dyplom/glapp_z7/glapp/phong.vert", GL_VERTEX_SHADER);
    phongProgram->compileShaderFromFile("C:/Users/al/Desktop/Dyplom/glapp_z7/glapp/phong.fsh", GL_FRAGMENT_SHADER);
    phongProgram->link();

    program = phongProgram; //gourardProgram;

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.setInterval(10);
    timer.start();
}
void Widget::paintGL() {
    processCamera();
    rotateLight();
    glClearColor(0.5, 0.5, 0.5, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->use();
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    program->setUniform("ViewMat", camera->matrix());
    program->setUniform("ProjMat", projMat);
    program->setUniform("LightPosition", lightPosition);
    //program->setUniform("LightPower", 50);


    for(size_t i=0; i< meshes.size(); i++) {
        Mesh *mesh = meshes[i];
        Texture *texture = textures[i];
        program->setUniform("ModelMat", mesh->matrix());
        texture->bind(0);

//        program->setUniform("MaterialAmbient", mesh->material.ambient);
//        program->setUniform("MaterialDiffuse", mesh->material.diffuse);
//        program->setUniform("MaterialSpecular", mesh->material.specular);
        program->setUniform("MaterialShiness", mesh->material.shiness);
        program->setUniform("ColorTexture", 1);

        mesh->render();
        texture->unbind();
    }
    glDisable(GL_BLEND);
}
void Widget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    projMat = perspectiveMat(60.0f, w/(float)h, 0.01f, 150.0f);
}

void Widget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_Tab: switchProgram(); break;
        default: keys.insert(event->key());
    }
}


void Widget::keyReleaseEvent(QKeyEvent *event) {
    keys.erase(event->key());
}

void Widget::switchProgram() {
    if(program == gourardProgram) {
        program = phongProgram;
        qDebug() << "phong";
    }
    else {
        program = gourardProgram;
        qDebug() << "gourard";
    }
}

void Widget::mousePressEvent(QMouseEvent * e) {
    if(e->button() == Qt::LeftButton){
        e->pos().x();
        ax = e->pos().x();
        ay = e->pos().y();
    }
}

void Widget::mouseMoveEvent(QMouseEvent * e) {
    if(e->buttons() == Qt::LeftButton) {
        dax += ax-e->pos().x();
        day += ay-e->pos().y();

        ax = e->pos().x();
        ay = e->pos().y();
    }
}

void Widget::processCamera() {
    float dv = 0.1f;

    if(keys.find(Qt::Key_W) != keys.end())
        camera->pos = camera->pos + camera->forward*dv;
    else if(keys.find(Qt::Key_S) != keys.end())
        camera->pos = camera->pos - camera->forward*dv;
    if(keys.find(Qt::Key_A) != keys.end())
        camera->pos = camera->pos - camera->x()*dv;
    else if(keys.find(Qt::Key_D) != keys.end())
        camera->pos = camera->pos + camera->x()*dv;
    if(keys.find(Qt::Key_Q) != keys.end())
        camera->pos = camera->pos + camera->up*dv;
    else if(keys.find(Qt::Key_Z) != keys.end())
        camera->pos = camera->pos - camera->up*dv;


    camera->forward = {0,0,-1};
    camera->forward = camera->forward * rotationMat(dax, 0, 1, 0);
    vec3 cx = normal(camera->x());
    camera->forward = camera->forward * rotationMat(std::clamp<float>(day,-89.9,89.9), cx.x, cx.y, cx.z);
}

void Widget::rotateLight() {
    //lightPosition = lightPosition * rotationMat(1, 0,1,0);
    //lightPosition = lightPosition * rotationMat(1, 0, 1, 0);
}

