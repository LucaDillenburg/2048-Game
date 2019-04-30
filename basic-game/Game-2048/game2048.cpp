#include "game2048.h"

#include <stdlib.h> //rand()
using namespace std;

//constructor and destructor
Game2048::Game2048(const int &matrixOrder)
{
    //set matrixOrder
    this->matrixOrder = matrixOrder;

    //alocate blocksMatrix and set the positions to 0
    this->blocksMatrix = new int*[this->matrixOrder];
    int ** const endMatrixPointer = this->blocksMatrix + this->matrixOrder;
    for (int **matrixPointer = this->blocksMatrix; matrixPointer!=endMatrixPointer; matrixPointer++)
    {
        *matrixPointer = new int[this->matrixOrder];

        int * const endArrayPointer = *matrixPointer + this->matrixOrder;
        for (int *arrayPointer = *matrixPointer; arrayPointer != endArrayPointer; arrayPointer++)
            *arrayPointer = 0;
    }

    this->blocksMatrix[0][1] = 8;
    this->blocksMatrix[0][2] = 8;

    //set other variables
    this->numberOfFreeBlocks = this->matrixOrder * this->matrixOrder;
}

Game2048::~Game2048()
{
    //deletes the dynamic alocated memory of the matrix
    int ** const endMatrixPointer = this->blocksMatrix + this->matrixOrder;
    for (int **matrixPointer = this->blocksMatrix; matrixPointer!=endMatrixPointer; matrixPointer++)
        delete[] *matrixPointer;
    delete[] this->blocksMatrix;
}


//methods
bool Game2048::move(const enum Direction &direction, bool &firstTimeMade2048Block)
{
    firstTimeMade2048Block = false;

    bool moved = false;

    // the algorithm will do different things acording to the direction
        // Right: slow = i, fast = j (decrescent)
        // Left:  slow = i, fast = j (crescent)
        // Up:    slow = j, fast = i (crescent)
        // Down:  slow = j, fast = i (decrescent)

    // bool consts
    const bool isCrescent = (direction==Direction::Left || direction==Direction::Up);
    const bool slowIsI = (direction==Direction::Right || direction==Direction::Left);

    // left or up = crescent, right or down = decrescent
    const int fastInit = (isCrescent ? 0 : this->matrixOrder-1);
    const int fastStep = (isCrescent ? 1 : -1);

    for (int slow = 0; slow < this->matrixOrder; slow++)
    {
        int currNumOfFreeBlocksInLine = 0; //represents the number of blocks that the current one will be deslocated to {direction}
        bool lastElemNotFreeSummed = false; //each block can be sumed once at most

        int iLastNotFreeBlock = -1;
        int jLastNotFreeBlock = -1;

        for (int fast = fastInit; (isCrescent ? fast<this->matrixOrder : fast>=0); fast += fastStep)
        {
            const int i = (slowIsI) ? slow : fast;
            const int j = (slowIsI) ? fast : slow;
            const int curr = this->blocksMatrix[i][j];

            if (curr == 0)
                currNumOfFreeBlocksInLine++;
            else
            {
                if (iLastNotFreeBlock >= 0 //if it has already found an element with value different from zero except for this one
                        && !lastElemNotFreeSummed //each block can be sumed once at most
                        && curr == this->blocksMatrix[iLastNotFreeBlock][jLastNotFreeBlock])
                {
                    //input sum in last block with value
                    const int newValueSumedBlock = curr * 2;
                    this->blocksMatrix[iLastNotFreeBlock][jLastNotFreeBlock] = newValueSumedBlock;
                    lastElemNotFreeSummed = true;
                    moved = true; //sum of elements

                    //checks if the user has made a 2048 block for the first time
                    if (!this->hasMade2048Block && newValueSumedBlock==2048)
                    {
                        this->hasMade2048Block = true;
                        firstTimeMade2048Block = true;
                    }

                    //zero in current element
                    this->blocksMatrix[i][j] = 0;

                    currNumOfFreeBlocksInLine++; //as it removed current
                    this->numberOfFreeBlocks++; //as a not free block became free

                    //points
                    this->score += newValueSumedBlock;
                }else
                {
                    if (currNumOfFreeBlocksInLine != 0)
                    {
                        //move curr block {currNumOfFreeBlocksInLine} positions to the {direction}
                        //1. find the new position (i,j) of the current block
                            // Direction::Right :
                        int newICurrBlock, newJCurrBlock;
                        switch(direction)
                        {
                            case Direction::Right:
                                newICurrBlock = i;
                                newJCurrBlock = j + currNumOfFreeBlocksInLine;
                                break;
                            case Direction::Left:
                                newICurrBlock = i;
                                newJCurrBlock = j - currNumOfFreeBlocksInLine;
                                break;
                            case Direction::Up:
                                newICurrBlock = i - currNumOfFreeBlocksInLine;
                                newJCurrBlock = j;
                                break;
                            case Direction::Down:
                                newICurrBlock = i + currNumOfFreeBlocksInLine;
                                newJCurrBlock = j;
                                break;
                        }

                        //2. swap blocks (not same element)
                        this->blocksMatrix[newICurrBlock][newJCurrBlock] = curr;
                        this->blocksMatrix[i][j] = 0;

                        moved = true; //curr block has moved to the {direction}

                        //set position last element not free (moved)
                        iLastNotFreeBlock = newICurrBlock;
                        jLastNotFreeBlock = newJCurrBlock;
                    }else
                    {
                        //set position last element not free (didn't move)
                        iLastNotFreeBlock = i;
                        jLastNotFreeBlock = j;
                    }

                    lastElemNotFreeSummed = false;
                }
            }
        }
    }

    return moved;
}

bool Game2048::addNewBlock()
{
    //if the user lost and hasn't restarted the game
    if (this->lost)
        throw logic_error("User lost and the game hasn't been restarted!");

    //you only looses when there is no more space to add a block
    this->lost = this->numberOfFreeBlocks == 0;
    if (this->lost)
        return true;

    //randomly chooses an index of the free blocks
    int newBlockIndex = rand() % this->numberOfFreeBlocks;

    //randomly chooses if this block will be a 2 or a 4
    bool newBlockValueIs4 = (rand() % 2) == 0;

    //finds (i,j) of the block to be added
    int numberOfFreeBlocksFound = 0;
    int ** const endMatrixPointer = this->blocksMatrix + this->matrixOrder;
    bool continueLoop = true; //to know if it will continue to execute the first loop from the nested loop
    for (int **matrixPointer = this->blocksMatrix; matrixPointer!=endMatrixPointer && continueLoop; matrixPointer++)
    {
        int * const endArrayPointer = *matrixPointer + this->matrixOrder;
        for (int *arrayPointer = *matrixPointer; arrayPointer != endArrayPointer; arrayPointer++)
            if (*arrayPointer == 0)
            {
                if (numberOfFreeBlocksFound == newBlockIndex)
                {
                    //here *arrayPointer is at the newBlockIndex^th free blocks
                    *arrayPointer = (newBlockValueIs4 ? 4 : 2); //adds the new block
                    continueLoop = false; //to break first loop
                    break;
                }
                numberOfFreeBlocksFound++;
            }
    }

    this->numberOfFreeBlocks--; //because added one block on a free block

    //if it came until here, the user hasn't lost, because the block was successfully added
    return false;
}


//getters
int Game2048::getScore() const
{
    return this->score;
}

bool Game2048::userLost() const
{
    return this->lost;
}


//draw
void Game2048::configurePaint(const int &windowWidth, const int &windowHeight)
{
    //score
    this->scoreWidth  = (float)Game2048::percScoreWidth/100.0f * windowWidth;
    this->scoreHeight = (float)Game2048::percScoreHeight/100.0f * windowHeight;
    this->xScorePaint = (float)(100-Game2048::percScoreRightMargin)/100.0f * windowWidth - this->scoreWidth;
    this->yScorePaint = (float)Game2048::percScoreTopMargin/100.0f * windowHeight;

    //matrix
    this->matrixSizePaint = (float)(100 - 2*Game2048::percMatrixHorizMargins)/100.0f * windowWidth;
    this->xMatrixPaint = (float)Game2048::percMatrixHorizMargins/100.0f * windowWidth;
    this->yMatrixPaint = (float)Game2048::percMatrixTopMargin/100.0f * windowWidth;

    //each block
    this->sizeBlockPaint = matrixSizePaint / this->matrixOrder;
    this->spaceBetweenBlocksPaint = 0;
}

void Game2048::drawMatrix(QPainter& painter) const
{
    // draw big rectangle (behind)
    painter.fillRect(QRect(this->xMatrixPaint, this->yMatrixPaint, this->matrixSizePaint, this->matrixSizePaint),
                     QBrush(QColor(188, 175, 166), Qt::SolidPattern));

    // draw each block
    const int xBlockFirstColumn = this->xMatrixPaint + this->spaceBetweenBlocksPaint;
    QRect blockRect (xBlockFirstColumn, this->yMatrixPaint + this->spaceBetweenBlocksPaint,
                     this->sizeBlockPaint, this->sizeBlockPaint); //position first block
    int ** const endMatrixPointer = this->blocksMatrix + this->matrixOrder;
    for (int **matrixPointer = this->blocksMatrix; matrixPointer!=endMatrixPointer; matrixPointer++)
    {
        int * const endArrayPointer = *matrixPointer + this->matrixOrder;
        for (int *arrayPointer = *matrixPointer; arrayPointer != endArrayPointer; arrayPointer++)
        {
            const int currBlock = *arrayPointer;

            //get block color
            QColor *blockColor, *textColor;
            if (currBlock > 4)
                textColor = new QColor(255, 243, 235);
            else
                textColor = new QColor(126, 116, 106);
            switch(currBlock)
            {
                case 0:    blockColor = new QColor(204, 192, 180); break;
                case 2:    blockColor = new QColor(238, 228, 218); break;
                case 4:    blockColor = new QColor(235, 223, 201); break;
                case 8:    blockColor = new QColor(244, 176, 115); break;
                case 16:   blockColor = new QColor(248, 136,  80); break;
                case 32:   blockColor = new QColor(240, 127,  91); break;
                case 64:   blockColor = new QColor(227,  92,  48); break;
                case 128:  blockColor = new QColor(237, 207, 111); break;
                case 256:  blockColor = new QColor(238, 203, 100); break;
                case 512:  blockColor = new QColor(240, 198,  83); break;
                case 1024: blockColor = new QColor(235, 198,  69); break;
                case 2048: blockColor = new QColor(237, 195,  44); break;
                default:   blockColor = new QColor( 62,  56,  53); break;
            }

            //draw small rectangle
            painter.fillRect(blockRect, QBrush(*blockColor, Qt::SolidPattern));

            if (currBlock != 0)
            {
                //draw a text
                painter.setPen(*textColor);
                char textCurrBlock[6];
                sprintf(textCurrBlock, "%d", currBlock);
                painter.drawText(blockRect, Qt::AlignCenter, textCurrBlock);
            }

            //delete create colors
            delete blockColor;
            delete textColor;

            //sets position to the next block (i+1)
            blockRect.moveTo(blockRect.x() + this->sizeBlockPaint + this->spaceBetweenBlocksPaint, blockRect.y());
        }

        //sets position to the first block of the next line
        blockRect.moveTo(xBlockFirstColumn, blockRect.y() + this->sizeBlockPaint + this->spaceBetweenBlocksPaint);
    }
}

void Game2048::drawScore(QPainter& painter) const
{
    //draw rectangle
    QRect scoreRect = QRect(this->xScorePaint, this->yScorePaint, this->scoreWidth, this->scoreHeight);
    painter.fillRect(scoreRect, QBrush(Qt::black, Qt::SolidPattern));

    //draw text
    painter.setPen(Qt::white);
    char scoreText[9];
    sprintf(scoreText, "%d", this->score);
    scoreRect.adjust(0,3,-10,0); //so it has some space between the left side of the rectangle and the text
    painter.drawText(scoreRect, Qt::AlignRight, scoreText);
}
