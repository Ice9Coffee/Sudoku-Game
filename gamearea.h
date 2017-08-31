#ifndef GAMEAREA_H
#define GAMEAREA_H

#include <QWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>
#include <QButtonGroup>
#include <QTextStream>
#include <QSignalMapper>

#include "sudokubox.h"

namespace Ui {
class GameArea;
}

class GameArea : public QWidget
{
    Q_OBJECT

public:
    explicit GameArea(QWidget *parent = 0);
    ~GameArea();

private slots:
    void on_numberButton_clicked(int num, bool checked);
    void on_currentBox_changed(QModelIndex id);

private:
    Ui::GameArea *ui;
    QButtonGroup *numberButtonGroup;
    QStandardItemModel *sudokuModel;

    SudokuBox* getBoxByIndex(QModelIndex id);

    bool makeMarkOn(QModelIndex id, int number, bool marked);
    void freshNumberButtons(QModelIndex boxid);


};

#endif // GAMEAREA_H
