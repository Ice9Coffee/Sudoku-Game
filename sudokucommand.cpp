#include "sudokucommand.h"


SudokuCommand::SudokuCommand(SudokuBox *box, markFlag oldF, markFlag newF) :
    m_box(box), m_old(oldF), m_new(newF)
{}

void SudokuCommand::undo() {
    m_box->setMarkFlag(m_old);
}

void SudokuCommand::redo() {
    m_box->setMarkFlag(m_new);
}
