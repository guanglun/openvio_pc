#ifndef FINDSTR_H
#define FINDSTR_H

#include "workspace.h"
#include <QObject>

class FindStr : public QObject
{
    Q_OBJECT
public:
    
    unsigned char *str;
    int len = 0;
    int index = 0;
    FindStr();
    config(unsigned char *str,int len);
    reset();
    int input(unsigned char *data,int len);
};

#endif // FINDSTR_H
