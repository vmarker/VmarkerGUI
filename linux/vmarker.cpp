#include "vmarker.h"
#include "hidapi/hidapi.h"

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
        int i = 0;
        for(i = 0; i<numPIDlist;i++){
            devs = hid_enumerate(VID,PIDlist[i]);
            cur_dev = devs;
            while(cur_dev){
                qDebug(cur_dev->path);
                if(cur_dev->interface_number == MIlist[i]){
                    this->vmarkerdev = hid_open_path(cur_dev->path);
                    if(this->vmarkerdev){
                        this->productID = PIDlist[i];
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
}

bool vmarker::readreg(uint8_t reg, uint16_t *data){
    if(this->connected){
        unsigned char buf[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        buf[1] = 0x04;
        buf[2] = reg;
        if(hid_write(this->vmarkerdev,buf,9)<0) return false;
        if(hid_read_timeout(this->vmarkerdev,buf,9,200)<0) return false;
        *data = (buf[1]<<8)+buf[2];
        qDebug() << (uint16_t)buf[0] << " " << (uint16_t)buf[1] << " " << (uint16_t)buf[2] << " " << (uint16_t)buf[3] << " " << (uint16_t)buf[4] << " ";
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
        qDebug() << "Live data size:" << *size << " X:" << *X << " Y:" << *Y;
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
        qDebug() << "MOUSE STATE: "<< buf[1] << " POINT: " << buf[2];
        *state = buf[1];
        *point = buf[2];
        return 1;
    }else{
        return -1;
    }
}
