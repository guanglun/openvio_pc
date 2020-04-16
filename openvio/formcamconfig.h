#ifndef FORMCAMCONFIG_H
#define FORMCAMCONFIG_H

#include <QWidget>
#include "workspace.h"
#include "winusbdriver.h"

namespace Ui {
class FormCamConfig;
}

class FormCamConfig : public QWidget
{
    Q_OBJECT
    
public:
    explicit FormCamConfig(QWidget *parent = 0);
    ~FormCamConfig();
    WinUSBDriver *qwinusb;
    void setQData(WinUSBDriver *qwinusb);
private slots:
    void on_pb_set_config_clicked();
    
    void on_pb_exit_clicked();
    
    void on_pb_cam_start_clicked();
    
    void on_pb_cam_stop_clicked();
    
private:
    Ui::FormCamConfig *ui;
};

#endif // FORMCAMCONFIG_H
