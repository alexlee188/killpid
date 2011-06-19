#include <QtGui/QApplication>
#include "killpid.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    killPID w;
    w.show();

    return a.exec();
}
