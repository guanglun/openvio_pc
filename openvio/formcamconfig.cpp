#include "formcamconfig.h"
#include "ui_formcamconfig.h"

const QStringList FrameSizeStr={
    "0x0",
	"88x72 QQCIF",
	"176x144 QCIF",
	"352x288 CIF",
	"88x60 QQSIF",
	"176x120 QSIF",
	"352x240 SIF",
	"40x30 QQQQVGA",
	"80x60 QQQVGA",
	"160x120 QQVGA",
	"320x240 QVGA",
	"640x480 VGA",
	"60x40 HQQQVGA",
	"120x80 HQQVGA",
	"240x160 HQVGA",
	"64x32",
	"64x64",
	"128x64",
	"128x128",
	"128x160 LCD",
	"128x160 QQVGA2",
	"720x480 WVGA",
	"752x480 WVGA2",
	"800x600 SVGA",
	"1024x768 XGA",
	"1280x1024 SXGA",
	"1600x1200 UXGA"
};

FormCamConfig::FormCamConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCamConfig)
{
    ui->setupUi(this);
    this->setWindowTitle("摄像头设置");
    ui->comboBoxCamSize->addItems(FrameSizeStr);
}

void FormCamConfig::setQData(WinUSBDriver *qwinusb)
{
    this->qwinusb = qwinusb;
}

FormCamConfig::~FormCamConfig()
{
    delete ui;
}

void FormCamConfig::on_pb_set_config_clicked()
{
    int num = ui->comboBoxCamSize->currentIndex();
    DBG("set frame num %d",num);
    qwinusb->ctrlCamSetFrameSizeNum(num);
}