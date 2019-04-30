#include "window.h"
#include "ui_window.h"

#include <string>
#include <QPen>

//CONSTRUCTOR
Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);
}

//DESTRUCTOR
Window::~Window()
{
    delete ui;
}

//EVENTS
void Window::keyPressEvent(QKeyEvent *event)
{
    this->qtd++;
    this->key = (char)event->key();
    this->update();
}

void Window::paintEvent(QPaintEvent *event)
{
    //create painter
    QPainter painter (this);

    //configure paint
    painter.setBrush(Qt::SolidPattern);
    QPen pen;
    pen.setColor(Qt::yellow);//black);
    pen.setWidth(2);
    painter.setPen(pen);

    //draw rectangle
    painter.drawRect(10, 10, 100, 200);

    //draw a text
    painter.setPen(Qt::green);
    char print[25];
    sprintf(print, "Number of timer: %d; Last key: %c", this->qtd, this->key);
    painter.drawText(10, 50, print);

    //to call paintEvent: this->update()
}
