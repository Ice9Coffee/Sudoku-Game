#ifndef SUDOKUCOMMAND_H
#define SUDOKUCOMMAND_H

#include <QUndoCommand>

#include "sudokubox.h"

class SudokuCommand : public QUndoCommand
{

public:
    SudokuCommand(SudokuBox *box, markFlag oldF, markFlag newF);
    void undo();
    void redo();

private:
    SudokuBox *m_box;
    markFlag m_old, m_new;
};

#endif // SUDOKUCOMMANDS_H
