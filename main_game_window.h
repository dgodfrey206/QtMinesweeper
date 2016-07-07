#ifndef MAIN_GAME_WINDOW_H
#define MAIN_GAME_WINDOW_H
#include <QMainWindow>
#include <QLabel>

namespace Ui {
class MainGameWindow;
}

class GameModel;

class MainGameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainGameWindow(QWidget *parent = 0);
    ~MainGameWindow();
protected:
    virtual void paintEvent(QPaintEvent *event);     
    virtual void mousePressEvent(QMouseEvent *event);
private:
    Ui::MainGameWindow *ui;

    GameModel *game; 
    QTimer *timer;     
    QLabel *timeLabel; 

    void handleGameState(GameModel *game); 
private slots:
    void onStartGameClicked();    
    void onLevelChooseClicked();  
    void onQuitClicked();        
};

#endif // MAIN_GAME_WINDOW_H
