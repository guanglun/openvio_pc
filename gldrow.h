#ifndef GLDROW_H
#define GLDROW_H

#include <QOpenGLWidget>
#include <glut.h>

class GLDrow
{
public:
    GLDrow();
    static void DrowGrid(void);
    static void DrowCube(void);
};

#endif // GLDROW_H
