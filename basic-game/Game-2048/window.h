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
    //const for window size
    constexpr static const float proportionWidthHeight = 0.78;

    Game2048 *game;

    void configureWindowDefaultSize();

    virtual void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);

    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();

private:
    Ui::Window *ui;
};

#endif // WINDOW_H
