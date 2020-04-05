#ifndef FORMCAMCONFIG_H
#define FORMCAMCONFIG_H

#include <QWidget>

namespace Ui {
class FormCamConfig;
}

class FormCamConfig : public QWidget
{
    Q_OBJECT
    
public:
    explicit FormCamConfig(QWidget *parent = 0);
    ~FormCamConfig();
    
private:
    Ui::FormCamConfig *ui;
};

#endif // FORMCAMCONFIG_H
