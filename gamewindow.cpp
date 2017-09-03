#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "gamearea.h"

#include <QFile>
#include <QMessageBox>

#include <QDebug>

#define MAGIC (233)

GameWindow::GameWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    auto *puzzleFile = new QFile(":/puzzles/default");
    if(puzzleFile->open(QFile::ReadOnly)) {
        for(int i = 0; i < 11; ++i) {
            p[i] = new QString(puzzleFile->readLine());
            a[i] = new QString(puzzleFile->readLine());
            qDebug() << "Read file:\n" << *(p[i]) << *(a[i]);
        }
        puzzleFile->close();
    }
    else qDebug() << "Failed to open defaul puzzle file...";


    connect(ui->actionLv_0,  &QAction::triggered, this, [=]{ loadPuzzle(0); });
    connect(ui->actionLv_1,  &QAction::triggered, this, [=]{ loadPuzzle(1); });
    connect(ui->actionLv_2,  &QAction::triggered, this, [=]{ loadPuzzle(2); });
    connect(ui->actionLv_3,  &QAction::triggered, this, [=]{ loadPuzzle(3); });
    connect(ui->actionLv_4,  &QAction::triggered, this, [=]{ loadPuzzle(4); });
    connect(ui->actionLv_5,  &QAction::triggered, this, [=]{ loadPuzzle(5); });
    connect(ui->actionLv_6,  &QAction::triggered, this, [=]{ loadPuzzle(6); });
    connect(ui->actionLv_7,  &QAction::triggered, this, [=]{ loadPuzzle(7); });
    connect(ui->actionLv_8,  &QAction::triggered, this, [=]{ loadPuzzle(8); });
    connect(ui->actionLv_9,  &QAction::triggered, this, [=]{ loadPuzzle(9); });
    connect(ui->actionLv_10, &QAction::triggered, this, [=]{ loadPuzzle(10);});
    connect(ui->actionRandom, &QAction::triggered, this, &GameWindow::loadRandomPuzzle);
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::loadPuzzle(int lv) {
    int re = QMessageBox::warning(this, "Sudoku-Game", "Are you sure to load a new game?", QMessageBox::Yes | QMessageBox::No);
    if(re == QMessageBox::Yes) ui->gameArea->loadProblem(p[lv], a[lv]);
}

void GameWindow::loadRandomPuzzle() {
    int re = QMessageBox::warning(this, "Sudoku-Game", "Are you sure to load a random game?", QMessageBox::Yes | QMessageBox::No);
    if(re != QMessageBox::Yes) return;
    int blanks = 30 + (60 - 30) * 1.0f * qrand() / RAND_MAX;
    sdkGen.gen(MAGIC, blanks);
    auto *rp = new QString(sdkGen.puzzle());
    auto *ra = new QString(sdkGen.answer());
    ui->gameArea->loadProblem(rp, ra);
}
