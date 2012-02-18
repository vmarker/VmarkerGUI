/* VmarkerGUI by Robin Theunis 
 * 2011-2012 
 * For use with the vmarker IR sensor */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vmarkergui.h"
#include "ui_vmarkergui.h"
#include "vmarker.h"
#include <QtGui/QLayout>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QGraphicsScene>
#include <string>
#include <stdio.h>
#include <QDebug>
#include <qmath.h>
#include <QGraphicsSimpleTextItem>
#include <QMessageBox>

VmarkerGUI::VmarkerGUI(QWidget *parent) : QMainWindow(parent), ui(new Ui::VmarkerGUI)
{
    ui->setupUi(this);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize );

    ui->grpCalibration->setEnabled(false);
    ui->grpCommon->setEnabled(false);
    ui->grpSensor->setEnabled(false);
    ui->grpLive->setEnabled(false);
    ui->btnCalibrate->setEnabled(false);
    ui->btnSave->setEnabled(false);

    this->saveToEeprom = false;
    //VMARKER
    this->_vmarker = new vmarker();
    connect(this->_vmarker, SIGNAL(Connected()),this,SLOT(Connected()));
    connect(this->_vmarker, SIGNAL(Disconnect()),this,SLOT(DisConnected()));
    this->_vmarker->Start();

    //LIFE DATA
    this->livetimer = new QTimer(this);
    connect(livetimer, SIGNAL(timeout()), this, SLOT(updateLive()));

    //UPDATE SENSOR
    this->sensorupdatetimer = new QTimer(this);
    connect(sensorupdatetimer, SIGNAL(timeout()), this, SLOT(updateSensor()));
    this->sensorupdatetimer->setSingleShot(true);

    //CAL SIZE
    connect(ui->sldCalSize,SIGNAL(valueChanged(int)),ui->spnCalSize,SLOT(setValue(int)));
    connect(ui->spnCalSize,SIGNAL(valueChanged(int)),ui->sldCalSize,SLOT(setValue(int)));
    connect(ui->sldCalSize,SIGNAL(sliderReleased()),this,SLOT(writeCalSize()));
    connect(ui->spnCalSize,SIGNAL(editingFinished()),this,SLOT(writeCalSize()));

    //CAL TIMEOUT
    connect(ui->sldCalTimeout,SIGNAL(valueChanged(int)),ui->spnCalTimeout,SLOT(setValue(int)));
    connect(ui->spnCalTimeout,SIGNAL(valueChanged(int)),ui->sldCalTimeout,SLOT(setValue(int)));
    connect(ui->sldCalTimeout,SIGNAL(sliderReleased()),this,SLOT(writeCalTimeout()));
    connect(ui->spnCalTimeout,SIGNAL(editingFinished()),this,SLOT(writeCalTimeout()));

    //SENSOR GAIN1
    connect(ui->sldGain1,SIGNAL(valueChanged(int)),ui->spnGain1,SLOT(setValue(int)));
    connect(ui->spnGain1,SIGNAL(valueChanged(int)),ui->sldGain1,SLOT(setValue(int)));
    connect(ui->sldGain1,SIGNAL(sliderReleased()),this,SLOT(writeSenGain1()));
    connect(ui->spnGain1,SIGNAL(editingFinished()),this,SLOT(writeSenGain1()));

    //SENSOR GAIN2
    connect(ui->sldGain2,SIGNAL(valueChanged(int)),ui->spnGain2,SLOT(setValue(int)));
    connect(ui->spnGain2,SIGNAL(valueChanged(int)),ui->sldGain2,SLOT(setValue(int)));
    connect(ui->sldGain2,SIGNAL(sliderReleased()),this,SLOT(writeSenGain2()));
    connect(ui->spnGain2,SIGNAL(editingFinished()),this,SLOT(writeSenGain2()));

    //SENSOR GAIN1
    connect(ui->sldArea,SIGNAL(valueChanged(int)),ui->spnArea,SLOT(setValue(int)));
    connect(ui->spnArea,SIGNAL(valueChanged(int)),ui->sldArea,SLOT(setValue(int)));
    connect(ui->sldArea,SIGNAL(sliderReleased()),this,SLOT(writeSenArea()));
    connect(ui->spnArea,SIGNAL(editingFinished()),this,SLOT(writeSenArea()));

    //SENSOR GAIN1
    connect(ui->sldBright,SIGNAL(valueChanged(int)),ui->spnBright,SLOT(setValue(int)));
    connect(ui->spnBright,SIGNAL(valueChanged(int)),ui->sldBright,SLOT(setValue(int)));
    connect(ui->sldBright,SIGNAL(sliderReleased()),this,SLOT(writeSenBright()));
    connect(ui->spnBright,SIGNAL(editingFinished()),this,SLOT(writeSenBright()));

    //SETTING MOUSE
    connect(ui->chkMouse,SIGNAL(clicked()),this,SLOT(writeSetMouse()));

    //SETTING MULTITOUCH
    connect(ui->chkMultiTouch,SIGNAL(clicked()),this,SLOT(writeSetMulti()));

    //SETTING CALIBRATION
    connect(ui->chkManCal,SIGNAL(clicked()),this,SLOT(writeSetManCal()));

    //SETTING MAC
    connect(ui->chkAppleMac,SIGNAL(clicked()),this,SLOT(writeSetMac()));

    //SAVE
    connect(ui->btnSave,SIGNAL(clicked()),this,SLOT(saveEeprom()));

    //CALIBRATE
    connect(ui->btnCalibrate,SIGNAL(clicked()),this,SLOT(startCalibration()));

    this->scene.setSceneRect(0,0,199,149);
    this->scene.addItem(&numbers_1);
    this->scene.addItem(&numbers_2);
    this->scene.addItem(&numbers_3);
    this->scene.addItem(&numbers_4);
    this->scene.addItem(&polygonitem);
}

VmarkerGUI::~VmarkerGUI()
{
    if(this->saveToEeprom){
        QMessageBox msgBox;
        msgBox.setText(QString("Save the settings?"));
        msgBox.setInformativeText(QString("Save the settings to the vmarker?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Yes){
            _vmarker->commiteeprom();
        }
    }
    delete ui;
}
void VmarkerGUI::DisConnected(){
    this->livetimer->stop();
    ui->grpCalibration->setEnabled(false);
    ui->grpCommon->setEnabled(false);
    ui->grpSensor->setEnabled(false);
    ui->grpLive->setEnabled(false);
    ui->btnCalibrate->setEnabled(false);
    ui->btnSave->setEnabled(false);
    ui->graphicsView->hide();
}

void VmarkerGUI::Connected(){
    uint16_t data = 0;

    ui->grpCalibration->setEnabled(true);
    ui->grpCommon->setEnabled(true);
    ui->grpSensor->setEnabled(true);
    ui->grpLive->setEnabled(true);
    ui->btnCalibrate->setEnabled(true);
    ui->btnSave->setEnabled(true);
    this->livetimer->start(100);

    _vmarker->readreg(REG_CALSIZE,&data);
    ui->spnCalSize->setValue(data);

    _vmarker->readreg(REG_CALTIMEOUT,&data);
    ui->spnCalTimeout->setValue(data);

    _vmarker->readreg(REG_GAIN1,&data);
    ui->spnGain1->setValue(data);

    _vmarker->readreg(REG_GAIN2,&data);
    ui->spnGain2->setValue(data);

    _vmarker->readreg(REG_AREA,&data);
    ui->spnArea->setValue(data);

    _vmarker->readreg(REG_BRIGHT,&data);
    ui->spnBright->setValue(data);

    _vmarker->readreg(REG_MOUSE_ENABLED,&data);
    ui->chkMouse->setChecked(data!=0);

    _vmarker->readreg(REG_MULTITOUCH_ENABLED,&data);
    ui->chkMultiTouch->setChecked(data!=0);

    _vmarker->readreg(REG_CALIBRATION_ENABLED,&data);
    ui->chkManCal->setChecked(data!=0);

    _vmarker->readreg(REG_MAC_Y_RATIO,&data);
    ui->chkAppleMac->setChecked(data==85);
}

void VmarkerGUI::writeCalSize(){
    uint16_t data = (uint16_t)(ui->spnCalSize->value());
    this->saveToEeprom = true;
    _vmarker->writereg(REG_CALSIZE,&data);
}

void VmarkerGUI::writeCalTimeout(){
    uint16_t data = (uint16_t)(ui->spnCalTimeout->value());
    this->saveToEeprom = true;
    _vmarker->writereg(REG_CALSIZE,&data);
}

void VmarkerGUI::writeSenGain1(){
    uint16_t data = (uint16_t)(ui->spnGain1->value());
    this->saveToEeprom = true;
    _vmarker->writereg(REG_GAIN1,&data);
    this->sensorupdatetimer->start(500);
}
void VmarkerGUI::writeSenGain2(){
    uint16_t data = (uint16_t)(ui->spnGain2->value());
    this->saveToEeprom = true;
    _vmarker->writereg(REG_GAIN2,&data);
    this->sensorupdatetimer->start(500);
}
void VmarkerGUI::writeSenArea(){
    uint16_t data = (uint16_t)(ui->spnArea->value());
    this->saveToEeprom = true;
    _vmarker->writereg(REG_AREA,&data);
    this->sensorupdatetimer->start(500);
}
void VmarkerGUI::writeSenBright(){
    uint16_t data = (uint16_t)(ui->spnBright->value());
    this->saveToEeprom = true;
    _vmarker->writereg(REG_BRIGHT,&data);
    this->sensorupdatetimer->start(500);
}
void VmarkerGUI::writeSetMouse(){
    uint16_t itrue = 0x01;
    uint16_t ifalse = 0x00;
    this->saveToEeprom = true;
    if(ui->chkMouse->isChecked()){
        ui->chkMultiTouch->setCheckState(Qt::Unchecked);
         _vmarker->writereg(REG_MULTITOUCH_ENABLED, &(ifalse));
         _vmarker->writereg(REG_MOUSE_ENABLED, &(itrue));
    }else{
         _vmarker->writereg(REG_MOUSE_ENABLED, &(ifalse));
    }
}
void VmarkerGUI::writeSetMulti(){
    uint16_t itrue = 0x01;
    uint16_t ifalse = 0x00;
    this->saveToEeprom = true;
    if(ui->chkMultiTouch->isChecked()){
        ui->chkMouse->setCheckState(Qt::Unchecked);
         _vmarker->writereg(REG_MOUSE_ENABLED, &(ifalse));
         _vmarker->writereg(REG_MULTITOUCH_ENABLED, &(itrue));
    }else{
         _vmarker->writereg(REG_MULTITOUCH_ENABLED, &(ifalse));
    }
}
void VmarkerGUI::writeSetManCal(){
    uint16_t itrue = 0x01;
    uint16_t ifalse = 0x00;
    this->saveToEeprom = true;
    if(ui->chkManCal->isChecked()){
         _vmarker->writereg(REG_CALIBRATION_ENABLED, &(itrue));
    }else{
         _vmarker->writereg(REG_CALIBRATION_ENABLED, &(ifalse));
    }
}
void VmarkerGUI::writeSetMac(){
    uint16_t itrue = 85;
    uint16_t ifalse = 100;
    this->saveToEeprom = true;
    if(ui->chkAppleMac->isChecked()){
         _vmarker->writereg(REG_MAC_Y_RATIO, &(itrue));
         _vmarker->writereg(REG_MAC_X_RATIO, &(itrue));
    }else{
         _vmarker->writereg(REG_MAC_Y_RATIO, &(ifalse));
         _vmarker->writereg(REG_MAC_X_RATIO, &(ifalse));
    }
}

void VmarkerGUI::updateLive()
{
    uint16_t data = 0;
    QPolygon polygon;
    unsigned int x,y,size,temp;
    bool update = true;
    _vmarker->getLifeData(&x,&y,&size);
    ui->lcdSize->display((int)size);
    ui->lcdX->display((int)x);
    ui->lcdY->display((int)y);

    unsigned char state, point;
    _vmarker->getMouseState(&state,&point);
    if(state == 2){
        ui->lblCalPoint->setText("P" + QString::number(point+1));
        ui->lblCalStatus->setText("Calibration running");
    }else{
        ui->lblCalPoint->setText(" ");
    }
    if(state == 1){
       ui->lblCalStatus->setText("Calibration OK");
    }
    int points[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    _vmarker->readreg(REG_CAL_X1,&data);
    points[0] = data*200/1024;
    _vmarker->readreg(REG_CAL_Y1,&data);
    points[1] = data*150/1024;
    _vmarker->readreg(REG_CAL_X2,&data);
    points[2] = data*200/1024;
    _vmarker->readreg(REG_CAL_Y2,&data);
    points[3] = data*150/1024;
    _vmarker->readreg(REG_CAL_X3,&data);
    points[4] = data*200/1024;
    _vmarker->readreg(REG_CAL_Y3,&data);
    points[5] = data*150/1024;
    _vmarker->readreg(REG_CAL_X4,&data);
    points[6] = data*200/1024;
    _vmarker->readreg(REG_CAL_Y4,&data);
    points[7] = data*150/1024;
    for(int i = 0; i < 8; i++){
        if(points[i] == 0){
            update = false;
        }
        //qDebug() << points[i];
    }
    if(update){
        if(abs(points[0]-points[2])<abs(points[1]-points[3])){
            for(int i = 0; i < 8; i+=2){
                temp = points[i];
                points[i] = points[i+1];
                points[i+1] = temp;
            }
        }
        if(points[0]>points[2]){
            for(int i = 0; i < 8; i+=2){
                points[i] = 200-points[i];
            }
        }
        if(points[1]>points[7]){
            for(int i = 1; i < 8; i+=2){
                points[i] = 150-points[i];
            }
        }
        polygon.setPoints(4,points);
        polygonitem.setPolygon(polygon);
        numbers_1.setText("1");
        numbers_1.setPos(points[0],points[1]);
        numbers_2.setText("2");
        numbers_2.setPos(points[2],points[3]);
        numbers_3.setText("3");
        numbers_3.setPos(points[4],points[5]);
        numbers_4.setText("4");
        numbers_4.setPos(points[6],points[7]);
        ui->graphicsView->setScene(&scene);
        ui->graphicsView->show();
        qDebug() << "Updated view";
    }
}

void VmarkerGUI::updateSensor()
{
   _vmarker->reinitsensor();
}

void VmarkerGUI::saveEeprom()
{
   this->saveToEeprom = false;
   _vmarker->commiteeprom();
}

void VmarkerGUI::startCalibration()
{
   _vmarker->startCalibration();
}
