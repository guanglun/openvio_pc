#include "glview.h"

#include "workspace.h"

GLView::GLView(QWidget *parent)
{

}

GLView::~GLView()
{

}


void GLView::initializeGL()
{
    initializeOpenGLFunctions();

    glShadeModel(GL_SMOOTH);
    glClearColor(0.0,0.0,0.0,0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    

    //glOrtho(0.0,1.0,0.0,1.0,-1.0,1.0);//指定坐标系的三维范围
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, 1, 0.2, 40); //产生一个投影矩阵 gluPerspective的是创建一个投影矩阵并且与当前矩阵相乘
    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity();                                         
    gluLookAt (0.0, 0.0, 50.0,
               0.0, 0.0, 0.0, 
               0.0, 1.0, 50.0);  
    
    
    
    QTimer *timer = new QTimer(this);                   //创建一个定时器
    //将定时器的计时信号与updateGL()绑定
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(100);                                   //以10ms为一个计时周期
    
    //QMetaObject::invokeMethod(this,"updateGL",Qt::QueuedConnection);

}

double icount = 1.0;

void GLView::updateGL()
{

//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    gluLookAt (0.0, 0.0, icount,
//               0.0, 0.0, 0.0, 
//               0.0, 1.0, icount);  
//    icount += 0.1;

//    DBG("updateGL");

//    update();
}

static bool isFirst = true;

void GLView::paintGL()
{
    
    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (0.0, 0.0, icount,
               0.0, 0.0, 0.0, 
               0.0, 1.0, icount);  
    icount += 0.1;

    DBG("paintGL %f",icount);
    
    if(isFirst)
    {
        isFirst = false;
        DBG("First %f",icount);
        glBegin(GL_TRIANGLES);  //绘制三角形
            glVertex3f(0.0, 1.0,    0.0);
            glVertex3f(-1.0,-1.0,   0.0);
            glVertex3f(1.0, -1.0,   0.0);
        glEnd();
    }


    
    QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection); 

    
}

void GLView::resizeGL(int w, int h)
{
    //static float icount = -1.0;
    
    glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();                                          //设置单位矩阵，会重置投影矩阵
    gluPerspective(90.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);   //产生一个投影矩阵 gluPerspective的是创建一个投影矩阵并且与当前矩阵相乘
    
    DBG("resizeGL");
    
}

void GLView::initShader()
{

}

void GLView::initTexture()
{

}

void GLView::sendDataToOpenGL()
{

}

void GLView::AddRotateAngle()
{

}

