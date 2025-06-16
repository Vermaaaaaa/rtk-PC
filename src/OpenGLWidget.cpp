#include "OpenGLWidget.h"
#include <QPointer>
#include <QtMath>
#include <QQuaternion>
#include <QDateTime>

static QQuaternion quatFromEuler(double rollRad, double pitchRad, double yawRad) {

    double r = qRadiansToDegrees(rollRad);
    double p = qRadiansToDegrees(pitchRad);
    double y = qRadiansToDegrees(yawRad);

    return QQuaternion::fromEulerAngles(r, y, p);
}

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , roll(0)
    , pitch(0)
    , yaw(0)
{
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);

    interpDuration = 50;
    lastTimeMs     = QDateTime::currentMSecsSinceEpoch();
    interpTimer    = new QTimer(this);
    interpTimer->setInterval(16);  // ~60 Hz
    connect(interpTimer, &QTimer::timeout, this, &OpenGLWidget::onInterpStep);
    interpTimer->start();

    currentQuat = QQuaternion::fromEulerAngles(0,0,0);
    targetQuat  = currentQuat;

}

OpenGLWidget::~OpenGLWidget() {

}

void OpenGLWidget::cleanup(){
    makeCurrent();

    roverVAO.destroy();
    roverVBO.destroy();
    roverIBO.destroy();

    gizmoVAO.destroy();
    gizmoVBO.destroy();
    gizmoIBO.destroy();

    doneCurrent();
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();



    if (auto *ctx = context()) {
        QPointer<OpenGLWidget> guard(this);
        connect(ctx, &QOpenGLContext::aboutToBeDestroyed, this, [guard]() mutable {
            if (guard) guard->cleanup();
        });
    }


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Load and compile shaders
    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader.vert"))
        qDebug() << "Vertex Shader Error:" << shaderProgram.log();

    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader.frag"))
        qDebug() << "Fragment Shader Error:" << shaderProgram.log();

    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shader.geom"))
        qDebug() << "Geometry Shader Error:" << shaderProgram.log();

    if (!shaderProgram.link())
        qDebug() << "Shader Linking Error:" << shaderProgram.log();


    unsigned int indices[] = {
        // Shaft (cube body)
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        0, 4, 7,  7, 3, 0,
        1, 5, 6,  6, 2, 1,
        3, 2, 6,  6, 7, 3,
        0, 1, 5,  5, 4, 0,


        8, 9, 12,
        9,10, 12,
        10,11, 12,
        11, 8, 12
    };


    float roverVertices[] = {
        -0.05f, -0.5f, -0.05f,  0.05f, -0.5f, -0.05f,
        0.05f,  0.5f, -0.05f, -0.05f,  0.5f, -0.05f,
        -0.05f, -0.5f,  0.05f,  0.05f, -0.5f,  0.05f,
        0.05f,  0.5f,  0.05f, -0.05f,  0.5f,  0.05f,
        -0.15f,  0.5f, -0.15f,  0.15f,  0.5f, -0.15f,
        0.15f,  0.5f,  0.15f, -0.15f,  0.5f,  0.15f,
        0.0f,   0.8f,  0.0f
    };


    roverVAO.create();
    roverVAO.bind();


    roverVBO.create();
    roverVBO.bind();
    roverVBO.allocate(roverVertices, sizeof(roverVertices));


    shaderProgram.enableAttributeArray(0);
    shaderProgram.setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

    roverVBO.release();


    roverIBO.create();
    if (!roverIBO.create()) {
        qDebug() << "🚨 IBO creation FAILED!";
    }
    roverIBO.bind();
    roverIBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
    roverIBO.allocate(indices, sizeof(indices));

    roverVAO.release();
    roverIBO.release();

    // ----------- Gizmo Arrow (thin) -----------
    float gizmoVertices[] = {
        -0.01f, -0.5f, -0.01f,  0.01f, -0.5f, -0.01f,
        0.01f,  0.5f, -0.01f, -0.01f,  0.5f, -0.01f,
        -0.01f, -0.5f,  0.01f,  0.01f, -0.5f,  0.01f,
        0.01f,  0.5f,  0.01f, -0.01f,  0.5f,  0.01f,
        -0.15f,  0.5f, -0.15f,  0.15f,  0.5f, -0.15f,
        0.15f,  0.5f,  0.15f, -0.15f,  0.5f,  0.15f,
        0.0f,   0.8f,  0.0f
    };

    gizmoVAO.create();
    gizmoVAO.bind();

    gizmoVBO.create();
    gizmoVBO.bind();
    gizmoVBO.allocate(gizmoVertices, sizeof(gizmoVertices));

    shaderProgram.enableAttributeArray(0);
    shaderProgram.setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

    gizmoIBO.create();
    gizmoIBO.bind();
    gizmoIBO.allocate(indices, sizeof(indices));

    gizmoVAO.release();
    gizmoVBO.release();
    gizmoIBO.release();


}


void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);

    shaderProgram.bind();


    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt(QVector3D(0, 0, 3), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, float(width()) / float(height()), 0.1f, 100.0f);


    QMatrix4x4 model;
    model.translate(0, 0, -3);

    model.rotate(roll, 1, 0, 0);
    model.rotate(pitch, 0, 1, 0);
    model.rotate(yaw, 0, 0, 1);

    shaderProgram.setUniformValue("modelMatrix", model);
    shaderProgram.setUniformValue("viewMatrix", viewMatrix);
    shaderProgram.setUniformValue("projectionMatrix", projectionMatrix);
    shaderProgram.setUniformValue("color", QVector3D(0.0f, 0.0f, 1.0f));

    roverVAO.bind();
    roverIBO.bind();

    glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_INT, nullptr);

    roverIBO.release();
    roverVAO.release();

    shaderProgram.setUniformValue("viewMatrix", viewMatrix);
    shaderProgram.setUniformValue("projectionMatrix", projectionMatrix);


    QMatrix4x4 gizmoView;
    gizmoView.setToIdentity();

    glClear(GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 gizmoProj;
    gizmoProj.ortho(-1, 1, -1, 1, -1, 1);



    shaderProgram.setUniformValue("viewMatrix", gizmoView);
    shaderProgram.setUniformValue("projectionMatrix", gizmoProj);

    float size = 0.05f;

    gizmoVAO.bind();
    gizmoIBO.bind();

    QMatrix4x4 modelX;
    modelX.translate(-0.9f, -0.9f, 0);
    modelX.scale(size);
    modelX.rotate(90, 0, 0, 1);

    shaderProgram.setUniformValue("modelMatrix", modelX);
    shaderProgram.setUniformValue("color", QVector3D(1, 0, 0));
    glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_INT, nullptr);

    QMatrix4x4 modelY;
    modelY.translate(-0.9f, -0.9f, 0);
    modelY.scale(size);

    shaderProgram.setUniformValue("modelMatrix", modelY);
    shaderProgram.setUniformValue("color", QVector3D(0, 1, 0));
    glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_INT, nullptr);

    QMatrix4x4 modelZ;
    modelZ.translate(-0.9f, -0.9f, 0);
    modelZ.scale(size);
    modelZ.rotate(-90, 1, 0, 0);

    shaderProgram.setUniformValue("modelMatrix", modelZ);
    shaderProgram.setUniformValue("color", QVector3D(0, 0, 1));
    glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_INT, nullptr);

    gizmoIBO.release();
    gizmoVAO.release();

    shaderProgram.release();
}



void OpenGLWidget::updateRotation(double newRoll, double newPitch, double newYaw)
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();


    currentQuat = targetQuat;

    targetQuat  = quatFromEuler(newRoll, newPitch, newYaw);

    lastTimeMs  = now;



}

void OpenGLWidget::onInterpStep()
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    float  t   = float(now - lastTimeMs) / interpDuration;
    t = qBound(0.0f, t, 1.0f);


    QQuaternion interpQuat =
        QQuaternion::slerp(currentQuat, targetQuat, t);


    QVector3D e = interpQuat.toEulerAngles();
    roll  = e.z() * -1;
    pitch = e.x();
    yaw   = e.y() * -1;


    update();
}


