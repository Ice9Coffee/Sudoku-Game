#ifndef SUDOKUBOX_H
#define SUDOKUBOX_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class SudokuBox;
}

class SudokuBox : public QWidget
{
    Q_OBJECT

public:
    explicit SudokuBox(int num, QWidget *parent = 0);
    ~SudokuBox();

    int getNumber() const;
    bool isEditable() const;
    int countMarks() const;

public slots:
    bool setMark(int n, bool marked);

signals:
    void numberChanged(int n);
    void markChanged(int n);

private slots:
    void on_numberChanged(int n);
    void on_markChanged(int n);

private:
    Ui::SudokuBox *ui;
    int number;
    bool editable;
    bool mark[10];

    QLabel** sLabel;


    void fillNumber(int n);
    void clearNumber();


    void clearMarks();

};

#endif // SUDOKUBOX_H
