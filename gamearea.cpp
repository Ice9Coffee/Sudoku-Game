﻿#include "gamearea.h"
#include "ui_gamearea.h"

#define SUDOKU_BOX_SIZE (60)

GameArea::GameArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameArea),
    numberButtonGroup(new QButtonGroup(this))
{
    ui->setupUi(this);

    //set numberButtons...
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


    //set table...
    ui->sudokuTable->setModel(sudokuModel = new QStandardItemModel(9, 9));
    //read question...
    for(int i = 0; i < 9; ++i) for(int j = 0; j < 9; ++j) {
        QModelIndex id = sudokuModel->index(i, j);
        ui->sudokuTable->setIndexWidget(id, new SudokuBox((i + j) % 9));
    }

    connect(ui->sudokuTable->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(on_currentBox_changed(QModelIndex)));

    //ui->sudokuTable->selectionModel()->currentChanged();



}

GameArea::~GameArea()
{
    delete ui;
}

void GameArea::on_numberButton_clicked(int num, bool checked) {
    QModelIndexList idlist = ui->sudokuTable->selectionModel()->selectedIndexes();
    if(!idlist.empty()) {
        makeMarkOn(idlist.at(0), num, checked);
    }
    else {
        //特殊情况：开局直接点数字时...
    }
}

void GameArea::on_currentBox_changed(QModelIndex id) {
    SudokuBox *box = getBoxByIndex(id);
    freshNumberButtons(box->getMarks());
    if(!box->isEditable()){
        int num = box->getNumber();
        for(int i=1; i<=9; ++i) numberButtonGroup->button(i)->setEnabled(false);
        numberButtonGroup->button(num)->setEnabled(true);
    }
    else for(int i=1; i<=9; ++i) numberButtonGroup->button(i)->setEnabled(true);

}

void GameArea::freshNumberButtons(markFlag f) {
    for(int i = 1; i <= 9; ++i) numberButtonGroup->button(i)->setChecked( f[i] );
}

SudokuBox *GameArea::getBoxByIndex(QModelIndex id) {
    return dynamic_cast<SudokuBox*>(ui->sudokuTable->indexWidget(id));
}

//......................
void GameArea::makeMarkOn(QModelIndex id, int number, bool marked) {
    getBoxByIndex(id)->setMark(number, marked);
}