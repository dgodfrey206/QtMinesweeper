#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QMessageBox>
#include "main_game_window.h"
#include "ui_maingamewindow.h"
#include "game_model.h"

const int blockSize = 20;
const int offsetX = 5; 
const int offsetY = 5; 
const int spaceY = 70; 

MainGameWindow::MainGameWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainGameWindow)
{
    ui->setupUi(this);
   
    timeLabel = new QLabel(this);
    
    connect(ui->actionStart, SIGNAL(triggered(bool)), this, SLOT(onStartGameClicked()));
    connect(ui->actionBasic, SIGNAL(triggered(bool)), this, SLOT(onLevelChooseClicked()));
    connect(ui->actionMedium, SIGNAL(triggered(bool)), this, SLOT(onLevelChooseClicked()));
    connect(ui->actionHard, SIGNAL(triggered(bool)), this, SLOT(onLevelChooseClicked()));
    connect(ui->actionQuit, SIGNAL(triggered(bool)), this, SLOT(onQuitClicked()));


    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));

    game = new GameModel;
    game->createGame(15, 20, 100);

    setFixedSize(game->mCol * blockSize + offsetX * 2, game->mRow * blockSize + offsetY * 2 + spaceY);

    timeLabel->setGeometry(game->mCol * blockSize + offsetX * 2 - 80, spaceY / 2, 80, 20);
    timeLabel->setText("Time: " + QString::number(game->timerSeconds) + " s");
    timer->start(1000);
}

MainGameWindow::~MainGameWindow()
{
    delete game;
    game = NULL;
    delete ui;
}

void MainGameWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap bmpBlocks(":/res/blocks.bmp");
    QPixmap bmpFaces(":/res/faces.bmp");
    QPixmap bmpFrame(":/res/frame.bmp");
    QPixmap bmpNumber(":/res/timenumber.bmp");

    switch(game->gameState)
    {
    case OVER:
        painter.drawPixmap((game->mCol * blockSize + offsetX * 2) / 2 - 12, spaceY / 2, bmpFaces, 0 * 24, 0, 24, 24); // 24是笑脸的边长,锚点在左上，因为工具栏占了些，所以看起来不再中间
        break;
    case PLAYING:
        painter.drawPixmap((game->mCol * blockSize + offsetX * 2) / 2 - 12, spaceY / 2, bmpFaces, 1 * 24, 0, 24, 24);
        break;
    case WIN:
        painter.drawPixmap((game->mCol * blockSize + offsetX * 2) / 2 - 12, spaceY / 2, bmpFaces, 2 * 24, 0, 24, 24);
        break;
    default:
        painter.drawPixmap((game->mCol * blockSize + offsetX * 2) / 2 - 12, spaceY / 2, bmpFaces, 1 * 24, 0, 24, 24);
        break;
    }

    int n = game->curMineNumber;
    int posX = (game->mCol * blockSize + offsetX * 2) / 2 - 50; 
    if(n <= 0)
    {
        painter.drawPixmap(posX, spaceY / 2, bmpNumber, n * 20, 0, 20, 28);
    }
    while(n > 0) // 如果是多位数
    {
        painter.drawPixmap(posX - 20, spaceY / 2, bmpNumber, n % 10 * 20, 0, 20, 28); // 每次从后面绘制一位
        n /= 10;
        posX -= 20;
    }

    for(int i = 0; i < game->mRow; i++)
    {
        for(int j = 0; j < game->mCol; j++)
        {
            switch(game->gameMap[i][j].curState)
            {
            case UN_DIG:
                painter.drawPixmap(j * blockSize + offsetX, i * blockSize + offsetY + spaceY , bmpBlocks, blockSize * 10, 0, blockSize, blockSize);
                break;
            case DIGGED:
                painter.drawPixmap(j * blockSize + offsetX, i * blockSize + offsetY + spaceY, bmpBlocks, blockSize * game->gameMap[i][j].valueFlag, 0, blockSize, blockSize);
                break;
            case MARKED:
                painter.drawPixmap(j * blockSize + offsetX, i * blockSize + offsetY + spaceY, bmpBlocks, blockSize * 11, 0, blockSize, blockSize);
                break;
            case BOMB:
                painter.drawPixmap(j * blockSize + offsetX, i * blockSize + offsetY + spaceY, bmpBlocks, blockSize * 9, 0, blockSize, blockSize);
                break;
            case WRONG_BOMB:
                if(game->gameState == PLAYING || game->gameState == FAULT)
                {
                    painter.drawPixmap(j * blockSize + offsetX, i * blockSize + offsetY + spaceY, bmpBlocks, blockSize * 11, 0, blockSize, blockSize);
                }
                else if(game->gameState == OVER)
                {
                    painter.drawPixmap(j * blockSize + offsetX, i * blockSize + offsetY + spaceY, bmpBlocks, blockSize * 12, 0, blockSize, blockSize);
                }
                break;
            default:
                break;
            }
        }
    }
    handleGameState(game);
}

void MainGameWindow::handleGameState(GameModel *game)
{
    if(game->gameState == OVER)
    {
        timer->stop();
        qDebug() << "you lose!";
    }
    else if(game->gameState == WIN)
    {
        timer->stop();
        qDebug() << "you win!";
//        QMessageBox::information(this, "win!", "your time: " + QString::number(game->timerSeconds) + " s");
//        game->gameState = PLAYING; 
    }
}

void MainGameWindow::mousePressEvent(QMouseEvent *event)
{
    int px = event->x() - offsetX;
    int py = event->y() - offsetY - spaceY;
    int row = py / blockSize;
    int col = px / blockSize;
   
    switch(event->button())
    {
    case Qt::LeftButton:
        game->digMine(row, col);
        update(); 
        break;
    case Qt::RightButton:
        game->markMine(row, col);
        update();
        break;
    default:
        break;

        int x = event->x();
        int y = event->y();

        if(x >= (game->mCol * blockSize + offsetX * 2) / 2 - 12
        && x <= (game->mCol * blockSize + offsetX * 2) / 2 + 12
        && y >= spaceY / 2
        && y <= spaceY / 2 + 24)
        {
            game->restartGame(); 
            timer->start(1000);
            timeLabel->setText("Time: " + QString::number(game->timerSeconds) + " s"); 
            update();
        }
    }
    else if(game->gameState != OVER && game->gameState != WIN)
    {
        int px = event->x() - offsetX;
        int py = event->y() - offsetY - spaceY;
        int row = py / blockSize;
        int col = px / blockSize;
        
        switch(event->button())
        {
        case Qt::LeftButton:
            game->digMine(row, col);
            update();
            break;
        case Qt::RightButton:
            game->markMine(row, col);
            update();
            break;
        default:
            break;
        }
    }
}

void MainGameWindow::onStartGameClicked()
{
    qDebug()<<"game started";
    game->restartGame();
    timeLabel->setText("Time: " + QString::number(game->timerSeconds) + " s");
    timer->start(1000);
    update();
}

void MainGameWindow::onLevelChooseClicked()
{
    QAction *actionSender = (QAction *)dynamic_cast<QAction *>(sender());
    if(actionSender == ui->actionBasic)
    {
        qDebug() << "basic";

        game->createGame(8, 10, 15, BASIC);
    }
    else if(actionSender == ui->actionMedium)
    {
        qDebug() << "medium";

        game->createGame(15, 20, 50, MEDIUM);

    }
    else if(actionSender == ui->actionHard)
    {
        qDebug() << "hard";
        game->createGame(20, 30, 100, HARD);
    }
    
    timer->start(1000);
    timeLabel->setText("Time: " + QString::number(game->timerSeconds) + " s");
    timeLabel->setGeometry(game->mCol * blockSize + offsetX * 2 - 80, spaceY / 2, 80, 20);
    setFixedSize(game->mCol * blockSize + offsetX * 2, game->mRow * blockSize + offsetY * 2 + spaceY);
}


void MainGameWindow::updateTimer()
{

    game->timerSeconds++;
    timeLabel->setText("Time: " + QString::number(game->timerSeconds) + " s");
    qDebug() << game->timerSeconds;
}


void MainGameWindow::onQuitClicked()
{
    QCoreApplication::quit();
}
