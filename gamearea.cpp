#include "gamearea.h"
#include "ui_gamearea.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>

#define SUDOKU_BOX_SIZE (60)
#define BG_STYLE_MAGIC(i, j) (((i)/3 + (j)/3) % 2 ? normalBoxStyle1 : normalBoxStyle2)
#define TEXT_STYLE_MAGIC(b) ((b)->isEditable() ? editableTextStyle : uneditableTextStyle)

const QDate startDate(2112, 9, 3);  //Birthday of Doraemon

GameArea::GameArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameArea),
    gameTime(new QDateTime(startDate, QTime(0, 0, 0))),
    hintUsed(0)
{
    ui->setupUi(this);

    //set numberButtons.
    numberButtonGroup = new QButtonGroup(this);
    numberButtonGroup->addButton(ui->numberButton1, 1);
    numberButtonGroup->addButton(ui->numberButton2, 2);
    numberButtonGroup->addButton(ui->numberButton3, 3);
    numberButtonGroup->addButton(ui->numberButton4, 4);
    numberButtonGroup->addButton(ui->numberButton5, 5);
    numberButtonGroup->addButton(ui->numberButton6, 6);
    numberButtonGroup->addButton(ui->numberButton7, 7);
    numberButtonGroup->addButton(ui->numberButton8, 8);
    numberButtonGroup->addButton(ui->numberButton9, 9);
    numberButtonGroup->setExclusive(false);

    for(int i = 1; i <= 9; ++i){
        connect(numberButtonGroup->button(i),
                static_cast<void (QAbstractButton:: *)(bool checked)>(&QAbstractButton::clicked),
                this, [=](bool checked){on_numberButton_clicked(i, checked);});
    }

    //set table.
    ui->sudokuTable->setModel(sudokuModel = new QStandardItemModel(9, 9));
    connect(ui->sudokuTable->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(on_currentBox_changed(QModelIndex, QModelIndex)));

    for(int i = 0; i < 9; ++i) for(int j = 0; j < 9; ++j) {
        QModelIndex id = sudokuModel->index(i, j);
        ui->sudokuTable->setIndexWidget(id, new SudokuBox((i+j)%9));
    }

    //set timer.
    timer = new QTimer(this);
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGameTime()));

    ui->PausePicture->setVisible(false);
    pauseImg = new QImage(":/pic/pausePic");
    ui->PausePicture->setPixmap(QPixmap::fromImage(*pauseImg));

    //set undoStack.
    undoStack = new QUndoStack(this);
    undoAction = undoStack->createUndoAction(this);
    redoAction = undoStack->createRedoAction(this);

    //setStyle....
    ui->sudokuTable->setShowGrid(false);
    readQSS(&normalBoxStyle1, ":/stylesheet/normalBox1");
    readQSS(&normalBoxStyle2, ":/stylesheet/normalBox2");
    readQSS(&highlightBoxStyle, ":/stylesheet/highlightBox");
    readQSS(&selectedBoxStyle, ":/stylesheet/selectedBox");
    readQSS(&sameNumBoxStyle, ":/stylesheet/sameNumBox");
    readQSS(&editableTextStyle, ":/stylesheet/editableText");
    readQSS(&uneditableTextStyle, ":/stylesheet/uneditableText");

    //read q
    loadProblem(new QString("123456780234567801345678012456780123567801234678012345780123456801234567012345678"),
                new QString("123456789234567891345678912456789123567891234678912345789123456891234567912345678"));

    for(int i = 0; i < 9; ++i) for(int j = 0; j < 9; ++j) {
        SudokuBox *box = getBox(sudokuModel->index(i, j));
        box->setStyleSheet(TEXT_STYLE_MAGIC(box) + BG_STYLE_MAGIC(i, j));
    }

}

GameArea::~GameArea()
{
    delete ui;
}

void GameArea::loadProblem(const QString *p, const QString *a) {
    answer = *a;
    for(int i = 0; i < 9; ++i) for(int j = 0; j < 9; ++j) {
        QModelIndex id = sudokuModel->index(i, j);
        int num = (*p)[9 * i + j].toLatin1() - '0';
        ui->sudokuTable->setIndexWidget(id, new SudokuBox(num));
    }

    //fresh style
    for(int i = 0; i < 9; ++i) for(int j = 0; j < 9; ++j) {
        SudokuBox *box = getBox(sudokuModel->index(i, j));
        box->setStyleSheet(TEXT_STYLE_MAGIC(box) + BG_STYLE_MAGIC(i, j));
    }

}

void GameArea::on_numberButton_clicked(int num, bool checked) {
    SudokuBox* box = getSelectedBox();
    if(!makeMarkOn(box, num, checked)) { //设置mark失败时，不改变numberButton.checked状态
        numberButtonGroup->button(num)->setChecked(!checked);
        return;
    }
    freshUndoRedoButtons();
    freshClearButton(box);
    QModelIndex id = ui->sudokuTable->selectionModel()->selectedIndexes().at(0);
    setStyle(id, id);
}

void GameArea::on_currentBox_changed(QModelIndex current, QModelIndex previous) {
    freshNumberButtons(getBox(current));
    freshClearButton(getBox(current));
    setStyle(current, previous);
}

void GameArea::freshNumberButtons(SudokuBox *box) {
    if(box == nullptr) return;
    markFlag f = box->getMarks();

    for(int i = 1; i <= 9; ++i) numberButtonGroup->button(i)->setChecked( f[i] );

    if(!box->isEditable()){
        int num = box->getNumber();
        for(int i=1; i<=9; ++i) numberButtonGroup->button(i)->setEnabled(false);
        numberButtonGroup->button(num)->setEnabled(true);
    }
    else for(int i=1; i<=9; ++i) numberButtonGroup->button(i)->setEnabled(true);

}

void GameArea::freshUndoRedoButtons() {
    ui->undoButton->setEnabled(undoStack->canUndo());
    ui->redoButton->setEnabled(undoStack->canRedo());
}

void GameArea::freshClearButton(SudokuBox* box) {
    if(box == nullptr) return;
    ui->clearButton->setEnabled(box->isEditable() && box->getMarks().count());
}

void GameArea::setGameAreaActive(bool active) {
    ui->sudokuTable->setEnabled(active);
    for(int i = 1; i <= 9; ++i) numberButtonGroup->button(i)->setEnabled(active);
    if(active) {
        freshUndoRedoButtons();
        freshClearButton(getSelectedBox());
    }
    else {
        ui->undoButton->setEnabled(false);
        ui->redoButton->setEnabled(false);
        ui->clearButton->setEnabled(false);
    }
}

void GameArea::readQSS(QString *dest, QString srcFile) {
    QFile *qssFile;
    qssFile= new QFile(srcFile);
    if(qssFile->open(QFile::ReadOnly)) *dest = QString(qssFile->readAll());
    qssFile->close();
    delete qssFile;
}

void GameArea::setStyle(QModelIndex current, QModelIndex previous) {
    SudokuBox *box, *b;
    if(previous.isValid()) {
        box = getBox(previous);
        if(box->isEditable()) {
            for(int j = 0; j < 9; ++j) {
                b = getBox(sudokuModel->index(previous.row(), j));
                b->setStyleSheet(TEXT_STYLE_MAGIC(b) + BG_STYLE_MAGIC(previous.row(), j));
            }
            for(int i = 0; i < 9; ++i) {
                b = getBox(sudokuModel->index(i, previous.column()));
                b->setStyleSheet(TEXT_STYLE_MAGIC(b) + BG_STYLE_MAGIC(i, previous.column()));
            }
        }

        int num = box->getNumber();
        if(num) for(int i = 0; i < 9; ++i) for(int j = 0; j < 9; ++j) {
            b = getBox(sudokuModel->index(i, j));
            if(b->getNumber() == num) b->setStyleSheet(TEXT_STYLE_MAGIC(b) + BG_STYLE_MAGIC(i, j));
        }
    }

    if(current.isValid()) {
        box = getBox(current);
        if(box->isEditable()) {
            for(int j = 0; j < 9; ++j) {
                b = getBox(sudokuModel->index(current.row(), j));
                b->setStyleSheet(TEXT_STYLE_MAGIC(b) + highlightBoxStyle);
            }
            for(int i = 0; i < 9; ++i) {
                b = getBox(sudokuModel->index(i, current.column()));
                b->setStyleSheet(TEXT_STYLE_MAGIC(b) + highlightBoxStyle);
            }
        }

        int num = box->getNumber();
        if(num) for(int i = 0; i < 9; ++i) for(int j = 0; j < 9; ++j) {
            b = getBox(sudokuModel->index(i, j));
            if(b->getNumber() == num) b->setStyleSheet(TEXT_STYLE_MAGIC(b) + sameNumBoxStyle);
        }
    }
    getBox(current)->setStyleSheet(TEXT_STYLE_MAGIC(b) + selectedBoxStyle);

    qDebug() << "setStyle at (" << current.row()+1 << "," << current.column() << ")";
}

void GameArea::updateGameTime() {
    //Note: addSecs()为const函数，更新时间时注意！！！
    *gameTime = gameTime->addSecs(1);

    int gameHours = startDate.daysTo(gameTime->date()) * 24 + gameTime->time().hour();
    if(gameTime->time().hour() >= 1) {
        int b = 1, h = gameHours;
        while(h /= 10) ++b;
        ui->timeLCD->setDigitCount(b + 6);
    }

    QString lcdString = QString::number(gameHours).append(":")
            .append(gameTime->toString("mm:ss"));
    ui->timeLCD->display(lcdString);
    //qDebug() << QString("Game Time: %0").arg(lcdString);
}

SudokuBox *GameArea::getBox(QModelIndex id) {
    return dynamic_cast<SudokuBox*>(ui->sudokuTable->indexWidget(id));
}

SudokuBox *GameArea::getSelectedBox() {
    QModelIndexList idlist = ui->sudokuTable->selectionModel()->selectedIndexes();
    if(idlist.empty()) return nullptr;
    else return getBox(idlist.at(0));
}

bool GameArea::makeMarkOn(SudokuBox *box, int number, bool marked) {
    if(box == nullptr) return false;
    markFlag oldF = box->getMarks();
    bool result = box->setMark(number, marked);
    markFlag newF = box->getMarks();

    //add in UndoStack.
    if(result) undoStack->push(new sudokuCommand(box, oldF, newF));

    return result;
}

bool GameArea::setMarkOn(SudokuBox *box, markFlag f) {
    markFlag oldF = box->getMarks();
    bool result = box->setMarkFlag(f);

    //add in UndoStack.
    if(result) undoStack->push(new sudokuCommand(box, oldF, f));

    return result;
}

bool GameArea::clearMark(SudokuBox *box) {
    return setMarkOn(box, markFlag());
}

void GameArea::on_pauseButton_clicked(bool checked) {
    if(checked){
        timer->stop();
        setGameAreaActive(false);

        //遮挡棋盘.
        ui->sudokuTable->setVisible(false);
        ui->PausePicture->setVisible(true);


    } else {
        timer->start();
        setGameAreaActive(true);

        //防止pause连打.
        ui->pauseButton->setEnabled(false);
        QTimer::singleShot(5000, this, [=]{ui->pauseButton->setEnabled(true);});

        //取消遮挡.
        ui->sudokuTable->setVisible(true);
        ui->PausePicture->setVisible(false);
    }
}

void GameArea::on_undoButton_clicked() {
    undoAction->trigger();

    freshUndoRedoButtons();
    freshNumberButtons(getSelectedBox());
}

void GameArea::on_redoButton_clicked() {
    redoAction->trigger();

    freshUndoRedoButtons();
    freshNumberButtons(getSelectedBox());

}

void GameArea::on_clearButton_clicked() {
    SudokuBox* box = getSelectedBox();
    if(box == nullptr) return;
    clearMark(box);

    freshUndoRedoButtons();
    freshNumberButtons(getSelectedBox());
}

void GameArea::on_hintButton_clicked() {
    SudokuBox* box = getSelectedBox();
    if(box == nullptr || !box->isEditable()) {
        QMessageBox::warning(this, "Sudoku-Game", "Select a box and click <bold>Hint</bold> to get the answer.", QMessageBox::Ok);
        return;
    }
    markFlag f;
    int i = ui->sudokuTable->selectionModel()->selectedIndexes().at(0).row();
    int j = ui->sudokuTable->selectionModel()->selectedIndexes().at(0).column();
    f.setflag(answer[9 * i + j].toLatin1() - '0', true);
    if( setMarkOn(box, f) ) {
        ++hintUsed;
    }
}
