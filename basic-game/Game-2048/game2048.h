#ifndef GAME2048_H
#define GAME2048_H

#include <QMainWindow>
#include <QPainter>

enum Direction {Right = 1, Left, Up, Down};

class Game2048
{
protected:
    //consts to paint
    //a) Score
    static const int percScoreTopMargin     = 10;
    static const int percScoreRightMargin   = 5;
    static const int percScoreWidth         = 10;
    static const int percScoreHeight        = 5;
    //b) Matrix
    static const int percMatrixHorizMargins = 10;
    static const int percMatrixTopMargin    = 15 + Game2048::percScoreTopMargin + Game2048::percScoreHeight;
    static const int percBeteweenBlocks     = 10; //percentage of the size of the block
    //c) Auxiliar paint (for not repeating the same calculation each time that it paints)
    int xScorePaint, yScorePaint, scoreWidth, scoreHeight;
    int matrixSizePaint, xMatrixPaint, yMatrixPaint;
    int sizeBlockPaint, spaceBetweenBlocksPaint;

    int matrixOrder;
    int** blocksMatrix;
    int score = 0;
    int numberOfFreeBlocks; //this variable will contain the number of free blocks (elem==0) in the matrix
    bool lost = false;
    bool hasMade2048Block = false;

public:
    //constructor and destructor
    Game2048() : Game2048(4) { }
    Game2048(const int &matrixOrder);
    ~Game2048();

    //methods
    bool move(const enum Direction&, bool &firstTimeMade2048Block); //returns if it actually moved anything
    bool addNewBlock(); //returns if the user lost

    //getters
    int getScore() const;
    bool userLost() const;

    //draw
    void configurePaint(const int &windowWidth, const int &windowHeight);
    void drawMatrix(QPainter&) const;
    void drawScore(QPainter&) const;
};

#endif // GAME2048_H
