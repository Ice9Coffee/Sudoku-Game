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
   void on_numberButton_toggled(int num, bool checked);


private:
    Ui::GameArea *ui;
    QButtonGroup *numberButtonGroup;
    QStandardItemModel *sudokuModel;

    void makeMarkOn(QModelIndex id, int number, bool marked);

};

#endif // GAMEAREA_H
