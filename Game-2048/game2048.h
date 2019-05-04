#ifndef GAME2048_H
#define GAME2048_H

#include <QMainWindow>
#include <QPainter>

enum Direction {Right = 1, Left, Up, Down};

class Game2048
{
protected:
    //Auxiliar Paint (for not repeating the same calculation each time that it paints)
    int xScore, yScore, scoreWidth, scoreHeight, scoreFontSize;
    int matrixSize, xMatrix, yMatrix;
    int sizeBlock, blockFontSize, spaceBetweenBlocks;
    //ps: not float type, because QRect only receives int as parameters

    //Basic/Structural
    static const int DEFAULT_MATRIX_ORDER = 4;
    int matrixOrder;
    long** blocksMatrix;
    long long score;
    int numberOfFreeBlocks; //this variable will contain the number of free blocks (elem==0) in the matrix
    bool lost;
    bool hasMade2048Block;

    //initialize
    void initialize(const int &matrixOrder, bool createNewMatrix);

    //movement
    bool movement(const enum Direction&, bool actuallyMove, bool &firstTimeMade2048Block); //moves or checks if the movement to that direction will change something (if move=true, it moves)
    bool checkLost(); //returns if user lost (doesn't set this->lost). ps: it assumes that this->lost is false and that the number of free blocks is zero

public:
    //start and restart game
    Game2048(); //for default matrix order
    Game2048(const int &matrixOrder);
    void start();
    void restartGame(); //it doesn't change the matrix order
    void restartGame(const int &matrixOrder);

    //destructor
    ~Game2048();

    //methods
    bool move(const enum Direction&, bool &firstTimeMade2048Block); //returns if it actually moved anything
    bool addNewBlock(); //returns if the user lost

    //getters
    long long getScore() const;
    bool userLost() const;

    //draw
    void configurePaint(const int &windowWidth, const int &windowHeight);
    void drawMatrix(QPainter&) const;
    void drawScore(QPainter&) const;


    //Const Paint
    //Score
    static const int percScoreTopMargin     = 10;
    static const int percScoreWidth         = 30;
    static const int percScoreHeight        = 5;
    static const int percScoreRightMargin   = 0; //relative to the matrix
    static const int percScoreFontSize      = 50; //relative to the score height
    //Matrix
    static const int percMatrixHorizMargins = 10;
    static const int percMatrixTopMargin    = 15 + Game2048::percScoreTopMargin + Game2048::percScoreHeight;
    static const int percBeteweenBlocks     = 10; //relative to the size of the block
    static const int percBlockFontSize      = 30; //relative to the block size
};

#endif // GAME2048_H
