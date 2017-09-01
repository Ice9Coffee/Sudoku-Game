#ifndef GAMEAREA_H
#define GAMEAREA_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QButtonGroup>
#include <QTextStream>
#include <QTime>
#include <QTimer>
#include <QImage>
#include <QUndoStack>
#include <QAction>

#include "sudokubox.h"
#include "sudokucommand.h"



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
    void on_currentBox_changed(QModelIndex current, QModelIndex previous);
    void updateGameTime();
    void setStyle(QModelIndex current, QModelIndex previous);

    void on_pauseButton_clicked(bool checked);
    void on_undoButton_clicked();
    void on_redoButton_clicked();
    void on_clearButton_clicked();

private:
    Ui::GameArea *ui;
    QButtonGroup *numberButtonGroup;
    QStandardItemModel *sudokuModel;

    QDateTime *gameTime;
    QTimer *timer;
    QImage *pauseImg;

    QUndoStack *undoStack;
    QAction *undoAction, *redoAction;

    QString normalBoxStyle, highlightBoxStyle, selectedBoxStyle;

    SudokuBox* getBox(QModelIndex id);
    SudokuBox* getSelectedBox();


    bool makeMarkOn(SudokuBox* box, int number, bool marked);
    bool setMarkOn(SudokuBox* box, markFlag f);
    bool clearMark(SudokuBox* box);

    void freshNumberButtons(SudokuBox* box);
    void freshUndoRedoButtons();
    void freshClearButton(SudokuBox *box);
    void setGameAreaActive(bool active);

};

#endif // GAMEAREA_H
