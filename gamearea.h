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
#include <QKeyEvent>
#include <QSoundEffect>
#include <QMediaPlayer>

#include "sudokubox.h"
#include "sudokucommand.h"
#include "bgmthread.h"

namespace Ui {
class GameArea;
}

class GameArea : public QWidget
{
    Q_OBJECT

public:
    explicit GameArea(QWidget *parent = 0);
    ~GameArea();

    void loadProblem(const QString* p, const QString* a);

    void keyPressEvent(QKeyEvent *e);

private slots:
    void on_numberButton_clicked(int num, bool checked);
    void on_currentBox_changed(QModelIndex current, QModelIndex previous);
    void updateGameTime();

    void on_pauseButton_clicked(bool checked);
    void on_undoButton_clicked();
    void on_redoButton_clicked();
    void on_clearButton_clicked();
    void on_hintButton_clicked();
    void on_restartButton_clicked();
    void on_commitButton_clicked();

private:
    Ui::GameArea *ui;
    QButtonGroup *numberButtonGroup;
    QStandardItemModel *sudokuModel;

    QString problem, answer;
    int hintUsed;

    QDateTime *gameTime;
    QTimer *timer;
    QImage *pauseImg;

    QUndoStack *undoStack;
    QAction *undoAction, *redoAction;

    QString normalBoxStyle1, normalBoxStyle2,
            highlightBoxStyle, selectedBoxStyle, sameNumBoxStyle,
            editableTextStyle, uneditableTextStyle;

    QSoundEffect numberSE, hintSE, winSE;

    SudokuBox* getBox(QModelIndex id);
    SudokuBox* getCurrentBox();

    bool makeMarkOn(SudokuBox* box, int number, bool marked);
    bool setMarkOn(SudokuBox* box, markFlag f);
    bool clearMark(SudokuBox* box);

    void freshNumberButtons(SudokuBox* box);
    void freshUndoRedoButtons();
    void freshClearButton(SudokuBox *box);
    void freshStyle(QModelIndex current, int curN, QModelIndex previous, int preN);
    void setGameAreaActive(bool active);

    void readQSS(QString* dest, QString srcFile);
};

#endif // GAMEAREA_H
