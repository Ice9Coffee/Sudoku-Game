#include "sudokubox.h"
#include "ui_sudokubox.h"

#include <QDebug>

SudokuBox::SudokuBox(int num, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SudokuBox),
    sLabel(new QLabel* [10])
{
    ui->setupUi(this);

    connect(this, SIGNAL(numberChanged()), SLOT(on_numberChanged()));
    connect(this, SIGNAL(markChanged(int)), SLOT(on_markChanged(int)));

    sLabel[0] = nullptr;
    sLabel[1] = ui->sLabel1; sLabel[2] = ui->sLabel2; sLabel[3] = ui->sLabel3;
    sLabel[4] = ui->sLabel4; sLabel[5] = ui->sLabel5; sLabel[6] = ui->sLabel6;
    sLabel[7] = ui->sLabel7; sLabel[8] = ui->sLabel8; sLabel[9] = ui->sLabel9;
    for(int i = 1; i <= 9; ++i) sLabel[i]->hide();
    ui->mLabel->hide();
    number = 0;


    //set the box...
    mark.clean();
    editable = true;
    if(1 <= num && num <= 9) {
        setMark(num, true);
        editable = false;
    } else if(num == 0) clearNumber();


}

SudokuBox::~SudokuBox()
{
    delete ui;
}

int SudokuBox::getNumber() const {return number;}
markFlag SudokuBox::getMarks() const {return mark;}
bool SudokuBox::isEditable() const {return editable;}
int SudokuBox::countMarks() const {return mark.count();}

//return isSuccessed
bool SudokuBox::setMark(int n, bool marked) {
    if(!editable || mark[n] == marked) return false;

    mark.setflag(n, marked);
    qDebug() << this
             << QString(": mark %0 changed to %1. Now markflag=%2")
                .arg(n).arg(marked?"true":"false").arg(mark.rawHash());
    emit markChanged(n);
    return true;
}

void SudokuBox::fillNumber(int n) {
    if(number == n) return;

    number = n;
    qDebug() << this << ": fill " << n;
    emit numberChanged();
}

void SudokuBox::clearNumber() {
    if(number == 0) return;
    number = 0;
    qDebug() << this << ": number cleared.";
    emit numberChanged();
}

void SudokuBox::clearMarks() {
    for(int i = 1; i <= 9; ++i) setMark(i, false);
}


void SudokuBox::on_numberChanged() {
    if(number == 0) {
        ui->mLabel->hide();
        return;
    }
    ui->mLabel->setText(QString::number(number));
    ui->mLabel->setVisible(true);
}

void SudokuBox::on_markChanged(int n) {
    int amount = countMarks();
    bool isMore = mark[n];

    if(amount > 1) {
        if (amount == 2 && isMore) {
            clearNumber();
            for(int i=1; i<=9; ++i) sLabel[i]->setVisible(mark[i]);
        }
        else sLabel[n]->setVisible(mark[n]);
    }
    else if (amount == 1) {
        int num=0;
        while(!mark[++num]);
        fillNumber(num);
        if(!isMore) for(int i=1; i<=9; ++i) sLabel[i]->setVisible(false);
    }
    else { //markAmount == 0
        clearNumber();
    }
}

