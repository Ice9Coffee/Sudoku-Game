#ifndef SUDOKUTABLE_H
#define SUDOKUTABLE_H

#include <QObject>
#include <QTableView>
#include <QKeyEvent>

#include "gamearea.h"

class SudokuTable : public QTableView
{
    Q_OBJECT

protected:
    void keyPressEvent(QKeyEvent* e);

public:
    SudokuTable(QWidget* parent = nullptr);
};

#endif // SUDOKUTABLE_H
