#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QTimer>
#include <QMatrix4x4>
#include <QQuaternion>


class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget() override;

    //TODO
    void updateRotation(double newRoll, double newPitch, double newYaw);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void cleanup();

private:
    QOpenGLShaderProgram shaderProgram;
    QOpenGLBuffer roverVBO, gizmoVBO;
    QOpenGLBuffer roverIBO { QOpenGLBuffer::IndexBuffer };
    QOpenGLBuffer gizmoIBO { QOpenGLBuffer::IndexBuffer };
    QOpenGLVertexArrayObject roverVAO, gizmoVAO;
    QMatrix4x4 modelMatrix;
    double roll, pitch, yaw;
    QTimer *timer;
    float colour;
    // Define a 3D arrow (shaft + arrowhead)
    float vertices[27];
    unsigned int indices[48];

    QQuaternion currentQuat;   // “where we were” at the last frame
    QQuaternion targetQuat;    // “where we want to go” set on each new packet
    qint64    lastTimeMs;      // timestamp when targetQuat was updated
    int       interpDuration;  // e.g. 100 (ms to smoothly interpolate)
    QTimer   *interpTimer;     // drives onInterpStep()


    void onInterpStep();

};


#endif // OPENGLWIDGET_H
