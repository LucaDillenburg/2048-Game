#include "window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //a.setWindowIcon(QIcon("icon.ico"));
    Window w;
    w.show();

    return a.exec();
}
