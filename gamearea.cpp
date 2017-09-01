﻿#include "gamearea.h"
#include "ui_gamearea.h"

#include <QDebug>

#define SUDOKU_BOX_SIZE (60)

const QDate startDate(2112, 9, 3);  //Birthday of Doraemon

GameArea::GameArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameArea),
    gameTime(new QDateTime(startDate, QTime(0, 0, 0)))
{
    ui->setupUi(this);

    //set numberButtons.
    numberButtonGroup = new QButtonGroup(this);
    numberButtonGroup->addButton(ui->numberButton1, 1);
    numberButtonGroup->addButton(ui->numberButton2, 2);
    numberButtonGroup->addButton(ui->numberButton3, 3);
    numberButtonGroup->addButton(ui->numberButton4, 4);
    numberButtonGroup->addButton(ui->numberButton5, 5);
    numberButtonGroup->addButton(ui->numberButton6, 6);
    numberButtonGroup->addButton(ui->numberButton7, 7);
    numberButtonGroup->addButton(ui->numberButton8, 8);
    numberButtonGroup->addButton(ui->numberButton9, 9);
    numberButtonGroup->setExclusive(false);

    for(int i = 1; i <= 9; ++i){
        connect(numberButtonGroup->button(i),
                static_cast<void (QAbstractButton:: *)(bool checked)>(&QAbstractButton::clicked),
                this, [=](bool checked){on_numberButton_clicked(i, checked);});
    }


    //set table.
    ui->sudokuTable->setModel(sudokuModel = new QStandardItemModel(9, 9));
    connect(ui->sudokuTable->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(on_currentBox_changed(QModelIndex)));

    //read question.....................
    for(int i = 0; i < 9; ++i) for(int j = 0; j < 9; ++j) {
        QModelIndex id = sudokuModel->index(i, j);
        ui->sudokuTable->setIndexWidget(id, new SudokuBox((i + j) % 9));
    }

    //set timer.
    timer = new QTimer(this);
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGameTime()));

    ui->PausePicture->setVisible(false);
    pauseImg = new QImage(":/pic/pausePic");
    ui->PausePicture->setPixmap(QPixmap::fromImage(*pauseImg));

    //set undoStack.
    undoStack = new QUndoStack(this);
    undoAction = undoStack->createUndoAction(this);
    redoAction = undoStack->createRedoAction(this);
}

GameArea::~GameArea()
{
    delete ui;
}

void GameArea::on_numberButton_clicked(int num, bool checked) {
    SudokuBox* box = getSelectedBox();
    if(!makeMarkOn(box, num, checked)) //设置mark失败时，不改变numberButton.checked状态
        numberButtonGroup->button(num)->setChecked(!checked);
    freshUndoRedoButtons();
}

void GameArea::on_currentBox_changed(QModelIndex id) {
    freshNumberButtons(getBoxByIndex(id));
}

void GameArea::freshNumberButtons(SudokuBox *box) {
    if(box == nullptr) return;
    markFlag f = box->getMarks();

    for(int i = 1; i <= 9; ++i) numberButtonGroup->button(i)->setChecked( f[i] );

    if(!box->isEditable()){
        int num = box->getNumber();
        for(int i=1; i<=9; ++i) numberButtonGroup->button(i)->setEnabled(false);
        numberButtonGroup->button(num)->setEnabled(true);
    }
    else for(int i=1; i<=9; ++i) numberButtonGroup->button(i)->setEnabled(true);

}

void GameArea::freshUndoRedoButtons() {
    ui->undoButton->setEnabled(undoStack->canUndo());
    ui->redoButton->setEnabled(undoStack->canRedo());
}

void GameArea::setGameAreaActive(bool active) {
    ui->sudokuTable->setEnabled(active);
    for(int i = 1; i <= 9; ++i) numberButtonGroup->button(i)->setEnabled(active);
    if(active) freshUndoRedoButtons();
    else {
        ui->undoButton->setEnabled(false);
        ui->redoButton->setEnabled(false);
    }
}

void GameArea::updateGameTime() {
    //Note: addSecs()为const函数，更新时间时注意！！！
    *gameTime = gameTime->addSecs(1);

    int gameHours = startDate.daysTo(gameTime->date()) * 24 + gameTime->time().hour();
    if(gameTime->time().hour() >= 1) {
        int b = 1, h = gameHours;
        while(h /= 10) ++b;
        ui->timeLCD->setDigitCount(b + 6);
    }

    QString lcdString = QString::number(gameHours).append(":")
            .append(gameTime->toString("mm:ss"));
    ui->timeLCD->display(lcdString);
    qDebug() << QString("Game Time: %0").arg(lcdString);
}

SudokuBox *GameArea::getBoxByIndex(QModelIndex id) {
    return dynamic_cast<SudokuBox*>(ui->sudokuTable->indexWidget(id));
}

SudokuBox *GameArea::getSelectedBox() {
    QModelIndexList idlist = ui->sudokuTable->selectionModel()->selectedIndexes();
    if(idlist.empty()) return nullptr;
    else return getBoxByIndex(idlist.at(0));
}

bool GameArea::makeMarkOn(SudokuBox *box, int number, bool marked) {
    if(box == nullptr) return false;
    markFlag oldF = box->getMarks();
    bool result = box->setMark(number, marked);
    markFlag newF = box->getMarks();

    //add in UndoStack.
    if(result) undoStack->push(new sudokuCommand(box, oldF, newF));

    return result;
}

bool GameArea::setMarkOn(SudokuBox *box, markFlag f) {
    markFlag oldF = box->getMarks();
    bool result = box->setMarkFlag(f);

    //add in UndoStack.
    if(result) undoStack->push(new sudokuCommand(box, oldF, f));

    return result;
}

void GameArea::on_pauseButton_clicked(bool checked) {
    if(checked){
        timer->stop();
        setGameAreaActive(false);

        //遮挡棋盘.
        ui->sudokuTable->setVisible(false);
        ui->PausePicture->setVisible(true);


    } else {
        timer->start();
        setGameAreaActive(true);

        //防止pause连打.
        ui->pauseButton->setEnabled(false);
        QTimer::singleShot(5000, this, [=]{ui->pauseButton->setEnabled(true);});

        //取消遮挡.
        ui->sudokuTable->setVisible(true);
        ui->PausePicture->setVisible(false);
    }
}

void GameArea::on_undoButton_clicked() {
    undoAction->trigger();

    freshUndoRedoButtons();
    freshNumberButtons(getSelectedBox());
}

void GameArea::on_redoButton_clicked() {
    redoAction->trigger();

    freshUndoRedoButtons();
    freshNumberButtons(getSelectedBox());

}

void GameArea::on_clearButton_clicked() {
    SudokuBox* box = getSelectedBox();
    box->setMarkFlag(markFlag());
}
