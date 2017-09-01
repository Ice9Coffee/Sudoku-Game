#ifndef SUDOKUBOX_H
#define SUDOKUBOX_H

#include <QWidget>
#include <QLabel>

struct markFlag {
    bool operator[](int n) const { return (flags >> n) & 1; }
    bool operator==(const markFlag& other) const {return flags == other.rawHash();}
    int count() const {
        int sum = 0, f = flags;
        for(sum = f & 1; f >>= 1; sum += f & 1);
        return sum;
    }

    //for debug.
    int rawHash() const {return flags;}

    void setflag(int n, bool checked) { checked ? flags |= 1 << n : flags &= ~(1 << n); }
    void clean() { flags = 0; }

private:
    int flags = 0;
};

namespace Ui {
class SudokuBox;
}

class SudokuBox : public QWidget
{
    Q_OBJECT

public:
    explicit SudokuBox(int num, QWidget *parent = 0);
    ~SudokuBox();

    //Read Only Interfaces
    int getNumber() const;
    markFlag getMarks() const;
    bool isEditable() const;
    int countMarks() const;

public slots:
    bool setMark(int n, bool marked);
    bool setMarkFlag(markFlag f);
    bool clearMarks();

signals:
    void numberChanged();
    void markChanged();

private slots:
    void on_numberChanged();
    void on_markChanged();

private:
    Ui::SudokuBox *ui;
    int number;
    markFlag mark;
    bool editable;

    QLabel** sLabel;

    void fillNumber(int n);
    void clearNumber();

};

#endif // SUDOKUBOX_H
