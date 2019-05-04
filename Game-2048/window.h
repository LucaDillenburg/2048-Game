#ifndef WINDOW_H
#define WINDOW_H

//window and paint
#include <QMainWindow>
#include <QPainter>

//events
#include <QKeyEvent>
#include <QPaintEvent>

//game
#include "game2048.h"

using namespace std;

namespace Ui {
class Window;
}

class Window : public QMainWindow
{
protected:
    Game2048 *game;

    //window size
    constexpr static const float proportionWidthHeight = 0.78f;
    void configureWindowDefaultSize();
    int widthFromHeight(const int &height);
    int heightFromWidth(const int &width);

    //events
    virtual void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

    //other methods
    void changeMatrixOrder(int matrixOrder); //used from menu
    void configurePaintWindowAndGame();

    //Paint
    static const int percWidthBtnRestart = 7;
    static const int percHeightBtnRestart = 4;
    static const int percBtnRestartFontSize = 50;
    static const int percXBtnRestart = Game2048::percMatrixHorizMargins;

    //others
    static const int numberRandomMovesPerClick = 500;

    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();

private slots:
    //menu
    void on_actionOrder2_triggered();
    void on_actionOrder3_triggered();
    void on_actionOrder4_triggered();
    void on_actionOrder5_triggered();
    void on_actionOrder6_triggered();
    void on_actionOrder8_triggered();
    void on_actionOrder10_triggered();
    void on_actionOther_triggered();

    //buttons
    void on_btnRestart_clicked();
    void on_btnRandomMoves_clicked();

private:
    Ui::Window *ui;
};

#endif // WINDOW_H
