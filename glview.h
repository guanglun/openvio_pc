#ifndef GLVIEW_H
#define GLVIEW_H


#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <windows.h>
#include <glut.h>
#include <QTimer>

class GLView : public QOpenGLWidget ,
                     protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLView(QWidget *parent = 0);
    ~GLView();
protected:
    //Three basic override function
    void initializeGL() Q_DECL_OVERRIDE;          //Initialize the OpenGL funciton and send data
    void paintGL() Q_DECL_OVERRIDE;               //Do opengl paint job
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;  //When the widget size changed it will work to
                                                  //adapt to window

private:
    QOpenGLBuffer        *mIndices;               //The buffer to store drawing indices
    QOpenGLBuffer        *mVBO;                   //The buffer to store vertexes
    QOpenGLTexture       *mTexture;               //Storage of OpenGL texture
    QOpenGLShaderProgram *mShaderProgram;         //The shader program

    void initShader();
    void initTexture();
    void sendDataToOpenGL();

    GLint vertexLocation;                         //The location of vertex array in GPU
    GLint texcoordLocation;                       //The location of texture coordinate in GPU 

    QMatrix4x4 projection;

    float  rotateAngle;
    QTimer timer;

private slots:
    void AddRotateAngle();                         //To do ratation
    void updateGL();
};
#endif // GLVIEW_H
