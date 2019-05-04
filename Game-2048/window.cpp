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
                sprintf(userLostText, "You made %lld points!", this->game->getScore());
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


//resize
void Window::configureWindowDefaultSize()
{
    // default window will be 70% of the height of the desktop
    QDesktopWidget desktop;
    const int defaultHeight = (int) desktop.height() * 0.5f;
    this->resize(this->widthFromHeight(defaultHeight), defaultHeight);
}

void Window::resizeEvent(QResizeEvent *event)
{
    //auxiliar consts
    const int newWidth = event->size().width();
    const int newHeight = event->size().height();

    const float currProportionWidthHeight = (float)newWidth/(float)newHeight;
    //if the proportion is approximately equal to the proportion (it's not always equal because the width and high can only be integers and not float)
    if (currProportionWidthHeight >= Window::proportionWidthHeight - 0.005f &&
            currProportionWidthHeight <= Window::proportionWidthHeight + 0.005f)
    {
        this->configurePaintWindowAndGame();
        this->update();
        return;
    }

    //porcentage of the new size compared to the old one
    const float newWidthPerc  = (float)newWidth / (float)event->oldSize().width();
    const float newHeightPerc = (float)newHeight / (float)event->oldSize().height();

    //the direction that has changed the most will prevail
    if (abs(1 - newWidthPerc) > abs(1 - newHeightPerc))
    //height will change proportionally to the width
        this->resize(newWidth, this->heightFromWidth(newWidth));
    else
    if (abs(1 - newWidthPerc) < abs(1 - newHeightPerc))
    //width will change proportionally to the height
        this->resize(this->widthFromHeight(newHeight), newHeight);
}

int Window::widthFromHeight(const int &height)
{
    return height * Window::proportionWidthHeight;
}

int Window::heightFromWidth(const int &width)
{
    return width / Window::proportionWidthHeight;
}


//buttons
void Window::on_btnRestart_clicked()
{
    this->game->restartGame();
    this->update();
    this->setFocus(); //so onKeyPressed is called when the user presses a key
}


//menu
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
    this->game->configurePaint(this->width(), this->height()); //it doesn't need to configure Window elements
    this->update();
    this->setFocus(); //so onKeyPressed is called when the user presses a key
}


//other methods
void Window::configurePaintWindowAndGame()
{
    //configure game elements (matrix and score)
    this->game->configurePaint(this->width(), this->height());

    //configure window elements
    //...
}

void Window::on_btnRandomMoves_clicked()
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
