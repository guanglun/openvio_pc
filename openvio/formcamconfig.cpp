#include "formcamconfig.h"
#include "ui_formcamconfig.h"

FormCamConfig::FormCamConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCamConfig)
{
    ui->setupUi(this);
}

FormCamConfig::~FormCamConfig()
{
    delete ui;
}
