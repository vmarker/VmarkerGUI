#ifndef VMARKERGUI_H
#define VMARKERGUI_H

#include <QMainWindow>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include "vmarker.h"
#include <QtConcurrentRun>

namespace Ui {
    class VmarkerGUI;
}

class VmarkerGUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit VmarkerGUI(QWidget *parent = 0);
        void updateLiveTH(void);
    ~VmarkerGUI();

public slots:
    void Connected(void);
    void DisConnected(void);
    void writeCalSize(void);
    void writeCalTimeout(void);
    void writeSenGain1(void);
    void writeSenGain2(void);
    void writeSenArea(void);
    void writeSenBright(void);
    void writeSetMouse(void);
    void writeSetMulti(void);
    void writeSetManCal(void);
    void writeSetMac(void);
    void writeKeyboard(int val);
    void updateLive(void);
    void updateSensor(void);
    void saveEeprom(void);
    void startCalibration(void);
    void writeEnKeyboard(void);
private:

    Ui::VmarkerGUI *ui;
    vmarker *_vmarker;
    QTimer *livetimer;
    QTimer *sensorupdatetimer;
    QGraphicsScene scene;
    QGraphicsSimpleTextItem numbers_1;
    QGraphicsSimpleTextItem numbers_2;
    QGraphicsSimpleTextItem numbers_3;
    QGraphicsSimpleTextItem numbers_4;
    QGraphicsPolygonItem polygonitem;
    QGraphicsEllipseItem circle;
    bool saveToEeprom;
    int updateCalScreen;
     QFuture<void> futureUpdate;
};

#endif // VMARKERGUI_H
