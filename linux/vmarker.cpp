/* VmarkerGUI by Robin Theunis 
 * 2011-2012 
 * For use with the vmarker IR sensor */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vmarker.h"
#include "hidapi/hidapi.h"
#include <stdio.h>
#include <QTimer>
#include <QDebug>
vmarker::vmarker(QObject *parent) : QObject(parent)
{
    this->connected = false;
    this->condistimer = new QTimer(this);
    connect(this->condistimer, SIGNAL(timeout()), this, SLOT(condisHandler()));

}
void vmarker::Start(){
       this->condistimer->start(1000);
}

void vmarker::condisHandler(){
    if(this->connected){
        //CHECK IF DISCONNECTED
        unsigned char *data;
        if(hid_read_timeout(this->vmarkerdev,data,0,200)<0){
            this->connected = false;
            qDebug("Vmarker Disconnected");
            emit(Disconnect());
            hid_close(this->vmarkerdev);
        }
    }else{
        //SEARCH FOR VMARKER
        struct hid_device_info *devs,*cur_dev;
        devs = hid_enumerate(VID,PID);
        cur_dev = devs;
        while(cur_dev){
            qDebug() << QString::number(cur_dev->vendor_id,16)  << " " << QString::number(cur_dev->product_id,16)  << " "<< QString(cur_dev->path)  << " " << QString::number(cur_dev->interface_number);
            if((cur_dev->interface_number == 1)){
                this->vmarkerdev = hid_open_path(cur_dev->path);
                if(this->vmarkerdev){
                    this->connected = true;
                    qDebug("Vmarker Connected");
                    emit (Connected());
                }
            }
            cur_dev = cur_dev->next;
        }
        hid_free_enumeration(devs);
    }
}

bool vmarker::readreg(uint8_t reg, uint16_t *data){
    if(this->connected){
        unsigned char buf[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        buf[1] = 0x04;
        buf[2] = reg;
        if(hid_write(this->vmarkerdev,buf,9)<0) { qDebug() << "WRITE ERROR: " << hid_error(this->vmarkerdev); return false;}
        if(hid_read_timeout(this->vmarkerdev,buf,8,200)<0)  { qDebug() << "READ ERROR: " <<  hid_error(this->vmarkerdev); return false;}
        *data = (buf[1]<<8)+buf[2];
        qDebug() << (uint16_t)buf[0] << " A  " << (uint16_t)buf[1] << " B " << (uint16_t)buf[2] << " ";
        return true;
    }else{
        return false;
    }
}
bool vmarker::writereg(uint8_t reg, uint16_t *data){
    if(this->connected){
        unsigned char buf[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        buf[1] = 0x05;
        buf[2] = reg;
        buf[3] = *data >> 8;
        buf[4] = *data;
        if(hid_write(this->vmarkerdev,buf,9)<0) return false;
        if(hid_read_timeout(this->vmarkerdev,buf,9,200)<0) return false;
        return true;
    }else{
        return false;
    }

}
bool vmarker::commiteeprom(){
    if(this->connected){
        unsigned char buf[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        buf[1] = 0x06;
        if(hid_write(this->vmarkerdev,buf,9)<0) return false;
        if(hid_read_timeout(this->vmarkerdev,buf,9,200)<0) return false;
        return true;
    }else{
        return false;
    }
}
bool vmarker::reinitsensor(){
    if(this->connected){
        unsigned char buf[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        buf[1] = 0x07;
        if(hid_write(this->vmarkerdev,buf,9)<0) return false;
        if(hid_read_timeout(this->vmarkerdev,buf,9,200)<0) return false;
        return true;
    }else{
        return false;
    }

}

bool vmarker::startCalibration(){
    if(this->connected){
        unsigned char buf[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        buf[1] = 0x03;
        if(hid_write(this->vmarkerdev,buf,9)<0) return false;
        if(hid_read_timeout(this->vmarkerdev,buf,9,200)<0) return false;
        return true;
    }else{
        return false;
    }
}
bool vmarker::getLifeData(unsigned int *X,unsigned int *Y,unsigned int *size){
    if(this->connected){
        unsigned char buf[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        buf[1] = 0x08;
        if(hid_write(this->vmarkerdev,buf,9)<0) return false;
        if(hid_read_timeout(this->vmarkerdev,buf,9,200)<0) return false;
        *size = (buf[1]<<8) + buf[2];
        *X = (buf[3]<<8) + buf[4];
        *Y = (buf[5]<<8) + buf[6];
       // qDebug() << "Live data size:" << *size << " X:" << *X << " Y:" << *Y;
        return true;
    }else{
        return false;
    }
}
bool vmarker::getMouseState(unsigned char *state , unsigned char *point ){
    if(this->connected){
        unsigned char buf[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        buf[1] = 0x01;
        if(hid_write(this->vmarkerdev,buf,9)<0) return false;
        if(hid_read_timeout(this->vmarkerdev,buf,9,200)<0) return false;
        //qDebug() << "MOUSE STATE: "<< buf[1] << " POINT: " << buf[2];
        *state = buf[1];
        *point = buf[2];
        return 1;
    }else{
        return -1;
    }
}
