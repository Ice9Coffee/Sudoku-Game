#include "sudokugenerator.h"

void m_swap(int& c1, int& c2){
    int temp = c1;
    c1 = c2;
    c2 = temp;
}

SudokuGenerator::SudokuGenerator() :
    sd{
{1,2,3,4,5,6,7,8,9},
{4,5,6,7,8,9,1,2,3},
{7,8,9,1,2,3,4,5,6},
{2,3,4,5,6,7,8,9,1},
{5,6,7,8,9,1,2,3,4},
{8,9,1,2,3,4,5,6,7},
{3,4,5,6,7,8,9,1,2},
{6,7,8,9,1,2,3,4,5},
{9,1,2,3,4,5,6,7,8}
      }
{
    qsrand(QTime(0, 0).secsTo(QTime::currentTime()));
}

void SudokuGenerator::gen(int magic, int blanks) {
    int r;
    for(int i = 0; i < magic; ++i) {
        r = 4.2f * qrand() / RAND_MAX;
        switch(r){
        case 1:
            floor();
            break;
        case 2:
            tower();
            break;
        case 3:
            row();
            break;
        case 4:
            column();
            break;
        case 5:
            turn();
            break;
        }
    }
    for(int i = 0; i < 81; ++i) {
        a[i] = '0' + sd[i/9][i%9];
        p[i] = a[i];
    }
    for(int i = 0; i < blanks; ++i) {
        r = 80.999f * qrand() / RAND_MAX;
        p[r] = '0';
    }
}

QString SudokuGenerator::puzzle() {
    return QString(p);
}

QString SudokuGenerator::answer() {
    return QString(a);
}

void SudokuGenerator::floor() {
    int r1, r2;
    r1= 3.0f * qrand() / RAND_MAX;
    r1 = r1 > 2 ? 2 : r1;
    r2= 3.0f * qrand() / RAND_MAX;
    r2 = r2 > 2 ? 2 : r2;
    for(int k = 0; k < 3; ++k) {
        for(int i = 0; i < 9; ++i) {
            m_swap(sd[3 * r1 + k][i], sd[3 * r2 + k][i]);
        }
    }
}

void SudokuGenerator::tower() {
    int r1, r2;
    r1= 3.0f * qrand() / RAND_MAX;
    r1 = r1 > 2 ? 2 : r1;
    r2= 3.0f * qrand() / RAND_MAX;
    r2 = r2 > 2 ? 2 : r2;
    for(int k = 0; k < 3; ++k) {
        for(int i = 0; i < 9; ++i) {
            m_swap(sd[i][3 * r1 + k], sd[i][3 * r2 + k]);
        }
    }
}

void SudokuGenerator::row(){
    int r1, r2, r3;
    r1= 3.0f * qrand() / RAND_MAX;
    r1 = r1 > 2 ? 2 : r1;
    r2= 3.0f * qrand() / RAND_MAX;
    r2 = r2 > 2 ? 2 : r2;
    r3= 3.0f * qrand() / RAND_MAX;
    r3 = r3 > 2 ? 2 : r3;
    for(int i = 0; i < 9; ++i) {
        m_swap(sd[3 * r1 + r2][i], sd[3 * r1 + r3][i]);
    }
}

void SudokuGenerator::column() {
    int r1, r2, r3;
    r1= 3.0f * qrand() / RAND_MAX;
    r1 = r1 > 2 ? 2 : r1;
    r2= 3.0f * qrand() / RAND_MAX;
    r2 = r2 > 2 ? 2 : r2;
    r3= 3.0f * qrand() / RAND_MAX;
    r3 = r3 > 2 ? 2 : r3;
    for(int i = 0; i < 9; ++i) {
        m_swap(sd[i][3 * r1 + r2], sd[i][3 * r1 + r3]);
    }
}

void SudokuGenerator::turn() {
    for(int i = 0; i < 9; ++i)
        for(int j = 0; j < i; ++j) {
            m_swap(sd[i][j], sd[j][i]);
        }
}

