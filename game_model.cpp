#include <time.h>
#include <stdlib.h>
#include "game_model.h"

GameModel::GameModel()
    : mRow(kRow),
      mCol(kCol),
      mineNumber(kMineCount),
      gameState(PLAYING)
{

}

GameModel::~GameModel()
{

}

void GameModel::createGame(int row, int col, int mineCount)
{
    mRow = row;
    mCol = col;
    mineNumber = mineCount;
    gameState = PLAYING;

    for(int i = 0; i < mRow; i++)
    {
        std::vector<MineBlock> lineBlocks;
        for(int j = 0; j < mCol; j++)
        {
            MineBlock mineBlock;
            mineBlock.curState = UN_DIG;
            mineBlock.valueFlag = 0;
            lineBlocks.push_back(mineBlock);
        }
        gameMap.push_back(lineBlocks);
    }

    srand((unsigned int)time(0));
    for(int k = mineNumber; k > 0; k--)
    {
        int pRow = rand() % mRow;
        int pCol = rand() % mCol;
        gameMap[pRow][pCol].valueFlag = -1;
    }

    for(int i = 0; i < mRow; i++)
    {
        for(int j = 0; j < mCol; j++)
        {
            for(int y = -1; y <= 1; y++)
            {
                for(int x = -1; x <= 1; x++)
                {
                    if(i + y >= 0
                    && i + y < mRow
                    && j + x >= 0
                    && j + x < mCol
                    && gameMap[i + y][j + x].valueFlag == -1
                    && !(x == 0 && y == 0))
                    {
                        gameMap[i][j].valueFlag++;
                    }
                }
            }
        }
    }
}

void GameModel::digMine(int m, int n)
{
    if(gameMap[m][n].valueFlag > 0
     && gameMap[m][n].curState == UN_DIG)
    {
        gameMap[m][n].curState = DIGGED;
    }

    if(gameMap[m][n].valueFlag == 0
     && gameMap[m][n].curState == UN_DIG)
    {
        gameMap[m][n].curState = DIGGED;
        for(int y = -1; y <= 1; y++)
        {
            for(int x = -1; x <= 1; x++)
            {
                if(m + y >= 0
                && m + y < mRow
                && n + x >= 0
                && n + x < mCol
                && !(x == 0 && y == 0))
                {
                    digMine(m + y, n + x);
                }
            }
        }
    }

    if(gameMap[m][n].valueFlag == -1)
    {
        gameState = OVER;
        gameMap[m][n].curState = BOMB;
    }

    checkGame();
}

void GameModel::markMine(int m, int n)
{
    if(gameMap[m][n].curState == UN_DIG)
    {
        gameMap[m][n].curState = MARKED;
        mineNumber--;
        if(gameMap[m][n].valueFlag == -1)
        {
            gameMap[m][n].curState = MARKED;
        }
        else
        {
            gameState = FAULT;
            gameMap[m][n].curState = WRONG_BOMB;
        }
        curMineNumber--;
>>>>>>> d96786b... Use updated Qt versions and build versions
    }
    else if(gameMap[m][n].curState == MARKED || gameMap[m][n].curState == WRONG_BOMB)
    {
        gameMap[m][n].curState = UN_DIG;
        gameState = PLAYING;
        curMineNumber++;
    }

    checkGame();
}

void GameModel::checkGame()
{
    if(gameState == OVER)
    {
        for(int i = 0; i < mRow; i++)
        {
            for(int j = 0; j < mCol; j++)
            {
                if(gameMap[i][j].valueFlag == -1)
                {
                    gameMap[i][j].curState = BOMB;
                }
            }
        }
    }

    if(mineNumber == 0)
    {
        gameState = WIN;
    }

    return gameState;
    }

    if(gameState != FAULT)
    {
        for(int i = 0; i < mRow; i++)
        {
            for(int j = 0; j < mCol; j++)
            {
                if(gameMap[i][j].curState == UN_DIG)
                {
                    gameState = PLAYING;
                    return;
                }
            }
        }
        gameState = WIN;
    }
}
