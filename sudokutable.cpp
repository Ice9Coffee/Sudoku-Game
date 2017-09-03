#include "sudokutable.h"

void SudokuTable::keyPressEvent(QKeyEvent *e) {
    static_cast<GameArea*>(parent())->keyPressEvent(e);
}

SudokuTable::SudokuTable(QWidget *parent) : QTableView(parent)
{}
