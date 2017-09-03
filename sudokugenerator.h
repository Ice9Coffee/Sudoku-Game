#ifndef SUDOKUGENERATOR_H
#define SUDOKUGENERATOR_H

#include <QString>
#include <QTime>

class SudokuGenerator
{
public:
    SudokuGenerator();
    void gen(int magic, int blanks);
    QString puzzle();
    QString answer();

private:
    int sd[9][9];

    char p[81], a[81];

    void floor();
    void tower();
    void row();
    void column();
    void turn();

};

#endif // SUDOKUGENERATOR_H
