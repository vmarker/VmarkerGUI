/* VmarkerGUI by Robin Theunis 
 * 2011-2012 
 * For use with the vmarker IR sensor */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VMARKER_H
#define VMARKER_H

#include <QObject>
#include <QTimer>
#include <stdint.h>
#include "hidapi/hidapi.h"

#define VID 0x2572
#define PID 0x0002
#define INTERFACE 0x01



#define REG_MOUSE_ENABLED 0x0F
#define REG_CALIBRATION_ENABLED 0x0E
#define REG_MULTITOUCH_ENABLED 0x10
#define REG_MAC_X_RATIO 0x13
#define REG_MAC_Y_RATIO 0x14
#define REG_GAIN1 0x01
#define REG_GAIN2 0x02
#define REG_THR 0x03
#define REG_AREA 0x04
#define REG_BRIGHT 0x05
#define REG_CAL_X1 0x06
#define REG_CAL_Y1 0x07
#define REG_CAL_X2 0x08
#define REG_CAL_Y2 0x09
#define REG_CAL_X3 0x0A
#define REG_CAL_Y3 0x0B
#define REG_CAL_X4 0x0C
#define REG_CAL_Y4 0x0D
#define REG_CALSIZE 0x11
#define REG_CALTIMEOUT 0x12

class vmarker : public QObject
{
    Q_OBJECT
private:
    bool connected;
    QTimer *condistimer;
    hid_device *vmarkerdev;
public:
    bool isConnected();
    explicit vmarker(QObject *parent = 0);
    bool readreg(uint8_t reg, uint16_t *data);
    bool writereg(uint8_t reg, uint16_t *data);
    bool commiteeprom();
    bool reinitsensor();
    bool startCalibration();
    void Start();
    bool getLifeData(unsigned int *X,unsigned int *Y,unsigned int *size);
    bool getMouseState(unsigned char *state , unsigned char *point );
signals:
    void Connected();
    void Disconnect();

public slots:
    void condisHandler();


};

#endif // VMARKER_H
