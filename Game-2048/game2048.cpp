#include "game2048.h"

#include <stdlib.h> //rand()
using namespace std;

//start and restart game
Game2048::Game2048() //for default matrix order
{
    //it has to do this because it's not possible to pass a CONST as reference parameter (even though the parameter is also const)
    int defaultMatrixOrder = Game2048::DEFAULT_MATRIX_ORDER;
    this->initialize(defaultMatrixOrder, true);
}

Game2048::Game2048(const int &matrixOrder)
{
    this->initialize(matrixOrder, true);
}

void Game2048::initialize(const int &matrixOrder, bool createNewMatrix)
{
    //set others variables
    this->score = 0;
    this->lost = false;
    this->hasMade2048Block = false;

    //set matrixOrder
    this->matrixOrder = matrixOrder;

    //set other variables
    this->numberOfFreeBlocks = this->matrixOrder * this->matrixOrder;

    //allocate blocksMatrix and set the positions to 0
    if (createNewMatrix)
        this->blocksMatrix = new int*[this->matrixOrder];
    int ** const endMatrixPointer = this->blocksMatrix + this->matrixOrder;
    for (int **matrixPointer = this->blocksMatrix; matrixPointer!=endMatrixPointer; matrixPointer++)
    {
        if (createNewMatrix)
            *matrixPointer = new int[this->matrixOrder];

        int * const endArrayPointer = *matrixPointer + this->matrixOrder;
        for (int *arrayPointer = *matrixPointer; arrayPointer != endArrayPointer; arrayPointer++)
            *arrayPointer = 0;
    }
}

void Game2048::restartGame() //it doesn't change the matrix order
{
    //resets all variables and blocks (does not need to create a new matrix)
    this->initialize(this->matrixOrder, false);

    this->start();
}

void Game2048::restartGame(const int &matrixOrder)
{
    if (matrixOrder == this->matrixOrder)
    //resets everything but doesn't create a new matrix (it just sets zero in each position of the matrix)
        this->restartGame();
    else
    //resets everything
    {
        this->initialize(matrixOrder, true);
        this->start();
    }
}

void Game2048::start()
{
    this->addNewBlock();
}


//destructor
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
    return this->movement(direction, true, firstTimeMade2048Block);
}

bool Game2048::movement(const Direction &direction, bool actuallyMove, bool &firstTimeMade2048Block) //returns if it would move or actually moved anything
//method used by move() and checksLost()
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
                    //movement
                    if (!actuallyMove) //if it's just to check if it would move or not
                        return true;
                    moved = true; //sum of two blocks

                    //input sum in last block with value
                    const int newValueSumedBlock = curr * 2;
                    this->blocksMatrix[iLastNotFreeBlock][jLastNotFreeBlock] = newValueSumedBlock;
                    lastElemNotFreeSummed = true;

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
                        //movement
                        if (!actuallyMove) //if it's just to check if it would move or not
                            return true;
                        moved = true; //curr block will move to the {direction}

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

    //if there's no more free blocks, the game checks if the user lost or not (the user didn't lose yet if the user can move and free some blocks)
    if (this->numberOfFreeBlocks == 0)
    {
        this->lost = this->checkLost();
        return this->lost;
    }else
    //if it came until here, the user hasn't lost, because the block was successfully added
        return false;
}

bool Game2048::checkLost() //returns if the user lost (does not set this->lost)
//assumes that this->lost is false and this->numberOfFreeBlocks is zero
{
    //Explanation: to know if the user can change any block by moving to any direction, it's only necessary to verify one horizontal direction and one vertical direction.
    //That's because if moving the matrix to the right changes a block, moving it to the left also changes it. The same happens with up and down directions.
    //ps: That's only true because there's no free blocks in the matrix.
    //ps2: Moving to the right or left might change which block would be modified, but not if any block would be modified.

    //checks if the user can change anything if he moves to any direction
    bool a;//won't use, but it's necessary
    return !this->movement(Direction::Right, false, a) && !this->movement(Direction::Up, false, a);
}


//getters
long int Game2048::getScore() const
{
    return this->score;
}

bool Game2048::userLost() const
{
    return this->lost;
}


//draw
void Game2048::configurePaint(const int &xMargin, const int &yMargin, const int &windowWidth, const int &windowHeight)
{
    //score
    this->scoreWidth  = (float)Game2048::percScoreWidth/100.0f * windowWidth;
    this->scoreHeight = (float)Game2048::percScoreHeight/100.0f * windowHeight;
    this->yScore = yMargin + (float)Game2048::percScoreTopMargin/100.0f * windowHeight;
    //font score
    this->scoreFontSize = this->scoreHeight * Game2048::percScoreFontSize/100;

    //each block
    const int pseudoMatrixSize = (float)(100 - 2*Game2048::percMatrixHorizMargins)/100.0f * windowWidth; //it's not the final matrix size, because if it were, there would be some space left in the end of the matrix (so after that, the matrix size will be calculated using the block size and space between blocks)
    const float proportionBetweenBlocks = (float)Game2048::percBeteweenBlocks/100.0f;
    this->sizeBlock = pseudoMatrixSize / (proportionBetweenBlocks * (this->matrixOrder + 1) + this->matrixOrder); //matrixSize / this->matrixOrder;
    this->spaceBetweenBlocks = (float)this->sizeBlock * proportionBetweenBlocks;
    //font block
    this->blockFontSize = this->sizeBlock * Game2048::percBlockFontSize/100;

    //matrix
    this->matrixSize = this->sizeBlock * this->matrixOrder + this->spaceBetweenBlocks * (this->matrixOrder + 1);
    this->xMatrix = xMargin + (windowWidth-this->matrixSize)/2;
    this->yMatrix = yMargin + (float)Game2048::percMatrixTopMargin/100.0f * windowWidth;

    //title
    this->xTitle = this->xMatrix + (float)Game2048::percTitleLeftMarginMatrix/100.0f * windowWidth;
    this->yTitle = yMargin + (float)Game2048::percTitleTopMargin/100.0f * windowHeight;
    this->titleFontSize = (float)Game2048::percTitleFontSize/100.0f * windowHeight;

    //others
    this->xScore = this->xMatrix + this->matrixSize - this->scoreWidth - (float)(Game2048::percScoreRightMargin)/100.0f * windowWidth;
}

void Game2048::drawMatrix(QPainter& painter) const
{
    // draw big rectangle (behind)
    painter.fillRect(QRect(this->xMatrix, this->yMatrix, this->matrixSize, this->matrixSize),
                     QBrush(QColor(188, 175, 166), Qt::SolidPattern));

    //set font size
    QFont font = painter.font();
    font.setPointSize(this->blockFontSize);
    painter.setFont(font);

    // draw each block
    const int xBlockFirstColumn = this->xMatrix + this->spaceBetweenBlocks;
    QRect blockRect (xBlockFirstColumn, this->yMatrix + this->spaceBetweenBlocks,
                     this->sizeBlock, this->sizeBlock); //position first block
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
                case 4:    blockColor = new QColor(227, 212, 190); break;
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
                sprintf(textCurrBlock, "%ld", currBlock);
                painter.drawText(blockRect, Qt::AlignCenter, textCurrBlock);
            }

            //delete create colors
            delete blockColor;
            delete textColor;

            //sets position to the next block (i+1)
            blockRect.moveTo(blockRect.x() + this->sizeBlock + this->spaceBetweenBlocks, blockRect.y());
        }

        //sets position to the first block of the next line
        blockRect.moveTo(xBlockFirstColumn, blockRect.y() + this->sizeBlock + this->spaceBetweenBlocks);
    }
}

void Game2048::drawHeader(QPainter& painter) const
{
    //DRAW 2048 LABEL
        //set font size
    QFont font = painter.font();
    font.setPointSize(this->titleFontSize);
    painter.setFont(font);
        //draw text
    painter.setPen(Qt::black);
    painter.drawText(this->xTitle, this->yTitle, "2048");

    //DRAW SCORE
        //draw rectangle
    QRect scoreRect = QRect(this->xScore, this->yScore, this->scoreWidth, this->scoreHeight);
    painter.fillRect(scoreRect, QBrush(Qt::black, Qt::SolidPattern));
        //set font size
    font.setPointSize(this->scoreFontSize);
    painter.setFont(font);
        //draw score
    painter.setPen(Qt::white);
    const int horizontalMargin = 0.06f * this->scoreWidth;
    const int verticalMargin = 0.12f * this->scoreHeight;
    scoreRect.adjust(horizontalMargin, verticalMargin, -horizontalMargin, 0); //so it has some space between the text and the rectangle
        // - label
    painter.drawText(scoreRect, Qt::AlignLeft, "Score:");
        // - number
    char scoreText[20];
    sprintf(scoreText, "%lld", this->score);
    painter.drawText(scoreRect, Qt::AlignRight, scoreText);
}
