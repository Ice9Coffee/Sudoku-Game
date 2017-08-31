#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "gamearea.h"

GameWindow::GameWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameWindow)
{
    ui->setupUi(this);
}

GameWindow::~GameWindow()
{
    delete ui;
}
