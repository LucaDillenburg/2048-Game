#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QPainter>

#include <QKeyEvent>
#include <QPaintEvent>

using namespace std;

namespace Ui {
class Window;
}

class Window : public QMainWindow
{
protected:
    int qtd = 0;
    char key;

    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);

    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();

private:
    Ui::Window *ui;
};

#endif // WINDOW_H
