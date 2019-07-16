#include "window.h"
#include "ui_window.h"

#include <string>
#include <QPen>
#include <QDesktopWidget>
#include <QMessageBox>

//CONSTRUCTOR
Window::Window(QWidget *parent) : QMainWindow(parent), ui(new Ui::Window)
{
    ui->setupUi(this);

    //create game (with default number of lines and columns)
    this->game = new Game2048();
    this->game->start();

    //configure window size and print
    this->configureWindowDefaultSize();

    this->setFocus(); //so onKeyPressed is called when the user presses a key
}


//destructor
Window::~Window()
{
    delete ui;
    delete this->game;
}


//window events
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
                sprintf(userLostText, "You made %ld points!", this->game->getScore());
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
    this->game->drawHeader(painter);
}


//resize
void Window::configureWindowDefaultSize()
{
    // default window will be 70% of the height of the desktop
    const int defaultHeight = (int) QDesktopWidget().availableGeometry().height() * 0.5f;
    this->resize(this->widthFromHeight(defaultHeight), defaultHeight);
}

void Window::resizeEvent(QResizeEvent *event)
{
    this->configureGamePaint();
}

void Window::configureGamePaint()
{
    //just the available space is considered (menu is not available)
    const int marginTop = (int) this->ui->menuBar->height() + (float)Window::percGameMarginTop/100.f * this->height();
    const int height = this->height() - marginTop;
    const int width = this->width();
    //do not use this->height and this->width anymore in this method...

    //same division as Window::proportionWidthHeight
    const float sizeProportion = (float)width / (float)height;

    int gameWidth, gameHeight;
    if (sizeProportion > (float)Window::percPropDefaultSize/100.f)
    //width is larger than needed, so height will be completely filled, but width will have blank spaces in both sides
    {
        gameHeight = height;
        gameWidth = this->widthFromHeight(gameHeight);
    }else //height is larger than needed, so width will be completely filled, but height will have blank spaces in both top and bottom
    {
        gameWidth = width;
        gameHeight = this->heightFromWidth(gameWidth);
    }

    const int xGame = (width - gameWidth)/2;
    const int yGame = (height - gameHeight)/2 + marginTop;

    //configure game elements
    this->game->configurePaint(xGame, yGame, gameWidth, gameHeight);

    //configure window elements
    //... (nothing to configure now, because there is no elements in the window except the menu)
}

int Window::widthFromHeight(const int &height)
{
    return (int) height * (float)Window::percPropDefaultSize/100.f;
}

int Window::heightFromWidth(const int &width)
{
    return (100 * width) / Window::percPropDefaultSize;
}


//restart
void Window::on_actionRestart_triggered()
{
    this->game->restartGame();
    this->update();
    this->setFocus(); //so onKeyPressed is called when the user presses a key
}

//random moves
void Window::on_actionRandomMoves_triggered()
{
    for(int i=0; i<Window::numberRandomMovesPerClick; i++)
    {
        int key, randomNum = rand() % 4;
        switch(randomNum)
        {
            case 0: key = Qt::Key_Right; break;
            case 1: key = Qt::Key_Left; break;
            case 2: key = Qt::Key_Up; break;
            default: key = Qt::Key_Down; break;
        }

        //simulate user move (will show messages and show the change of the matrix)
        QKeyEvent *keyPressedEvent = new QKeyEvent (QEvent::KeyPress, key, Qt::NoModifier);
        QCoreApplication::postEvent(this, keyPressedEvent);
        QKeyEvent *keyReleasedEvent = new QKeyEvent (QEvent::KeyRelease, key, Qt::NoModifier);
        QCoreApplication::postEvent(this, keyReleasedEvent);

        if (this->game->userLost())
            break;
    }
}


//change order
void Window::on_actionOrder2_triggered()
{
    this->changeMatrixOrder(2);
}
void Window::on_actionOrder3_triggered()
{
    this->changeMatrixOrder(3);
}
void Window::on_actionOrder4_triggered()
{
    this->changeMatrixOrder(4);
}
void Window::on_actionOrder5_triggered()
{
    this->changeMatrixOrder(5);
}
void Window::on_actionOrder6_triggered()
{
    this->changeMatrixOrder(6);
}
void Window::on_actionOrder8_triggered()
{
    this->changeMatrixOrder(8);
}
void Window::on_actionOrder10_triggered()
{
    this->changeMatrixOrder(10);
}
void Window::on_actionOther_triggered()
{

}

void Window::changeMatrixOrder(int matrixOrder)
{
    //restart game with a diferent matrixOrder
    this->game->restartGame(matrixOrder);
    this->configureGamePaint(); //it doesn't need to configure Window elements
    this->update();
    this->setFocus(); //so onKeyPressed is called when the user presses a key
}
