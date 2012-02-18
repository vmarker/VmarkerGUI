/* VmarkerGUI by Robin Theunis 
 * 2011-2012 
 * For use with the vmarker IR sensor */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QtGui/QApplication>
#include "vmarkergui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VmarkerGUI w;
    w.show();

    return a.exec();
}
