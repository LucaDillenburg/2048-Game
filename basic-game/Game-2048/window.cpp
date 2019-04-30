#include "window.h"
#include "ui_window.h"

#include <string>
#include <QPen>
#include <QDesktopWidget>
#include <QMessageBox>

//CONSTRUCTOR
Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);

    //create game (with default number of lines and columns)
    this->game = new Game2048();
    this->game->addNewBlock();

    //configure window size and print
    this->configureWindowDefaultSize();
    this->game->configurePaint(this->width(), this->height());
    this->update();
}

//DESTRUCTOR
Window::~Window()
{
    delete ui;
    delete this->game;
}

//METHODS
void Window::configureWindowDefaultSize()
{
    // default window will be 70% of the height of the desktop
    QDesktopWidget desktop;
    this->setFixedHeight(desktop.height() * 0.3);//0.70);
    this->setFixedWidth(this->height() * Window::proportionWidthHeight);
}

//EVENTS
void Window::keyPressEvent(QKeyEvent *event) //move matrix
{
    //if user lost and didn't restarted the game so far, the keys pressed doesn't do anything
    if (this->game->userLost())
        return;

    //get the direction that user wants to move the matrix if it is a key to move it
    bool move = true;
    enum Direction direction;
    switch(event->key())
    {
        case Qt::Key_Right:
            direction = Direction::Right;
            break;
        case Qt::Key_Left:
            direction = Direction::Left;
            break;
        case Qt::Key_Up:
            direction = Direction::Up;
            break;
        case Qt::Key_Down:
            direction = Direction::Down;
            break;
        default:
            //other keys do nothing
            move = false;
    }

    if (move)
    {
        //actually moves the matrix
        bool firstTime2048Block;
        bool actuallyMoved = this->game->move(direction, firstTime2048Block);

        if (actuallyMoved) //soh faz alguma coisa se moveu
        {
            if (firstTime2048Block)
                QMessageBox::question(this, "You win!", "You win! Keep it going...", QMessageBox::Ok);


            //adds new block
            bool lost = this->game->addNewBlock();
            this->update();
            if (lost)
            {
                char userLostText[30];
                sprintf(userLostText, "You made %d points!", this->game->getScore());
                QMessageBox::question(this, "You lost!", userLostText, QMessageBox::Ok);
            }
        }
    }
}

void Window::paintEvent(QPaintEvent*)
{
    //create painter and configure basic
    QPainter painter (this);
    painter.setRenderHint(QPainter::Antialiasing); //looks like increases resolution (jaggys remover)

    //draws background
    painter.fillRect(QRect(0,0,this->width(),this->height()), QBrush(QColor(251, 248, 241), Qt::SolidPattern));

    //draw everything
    this->game->drawMatrix(painter);
    this->game->drawScore(painter);
}
