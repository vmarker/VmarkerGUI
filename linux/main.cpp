#include <QtGui/QApplication>
#include "vmarkergui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VmarkerGUI w;
    w.show();

    return a.exec();
}
