#include "sudokucommand.h"


sudokuCommand::sudokuCommand(SudokuBox *box, markFlag oldF, markFlag newF) :
    m_box(box), m_old(oldF), m_new(newF)
{}

void sudokuCommand::undo() {
    m_box->setMarkFlag(m_old);
}

void sudokuCommand::redo() {
    m_box->setMarkFlag(m_new);
}
