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
    hintUsed(0),
    numberSE(this), hintSE(this), winSE(this)
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

    //set Style....
    ui->sudokuTable->setShowGrid(false);
    readQSS(&normalBoxStyle1, ":/stylesheet/normalBox1");
    readQSS(&normalBoxStyle2, ":/stylesheet/normalBox2");
    readQSS(&highlightBoxStyle, ":/stylesheet/highlightBox");
    readQSS(&selectedBoxStyle, ":/stylesheet/selectedBox");
    readQSS(&sameNumBoxStyle, ":/stylesheet/sameNumBox");
    readQSS(&editableTextStyle, ":/stylesheet/editableText");
    readQSS(&uneditableTextStyle, ":/stylesheet/uneditableText");

    //set SE
    numberSE.setSource(QUrl::fromLocalFile(":/SE/number"));
    hintSE.setSource(QUrl::fromLocalFile(":/SE/hint"));
    winSE.setSource(QUrl::fromLocalFile(":/SE/win"));

    auto *th = new BgmThread(":/bgm/bgm1", this);
    th->start();

    //read q
    loadProblem(new QString("000000000000000000000000000000000000000000000000000000000000000000000000000000000"),
                new QString("000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
    setGameAreaActive(false);
}

GameArea::~GameArea()
{
    delete ui;
}

void GameArea::loadProblem(const QString *p, const QString *a) {
    problem = *p;
    answer = *a;
    for(int i = 0; i < 9; ++i) for(int j = 0; j < 9; ++j) {
        QModelIndex id = sudokuModel->index(i, j);
        int num = (*p)[9 * i + j].toLatin1() - '0';
        ui->sudokuTable->setIndexWidget(id, new SudokuBox(num));
    }

    gameTime = new QDateTime(startDate, QTime(0, 0, 0));
    undoStack->clear();
    setGameAreaActive(true);
    ui->pauseButton->setEnabled(true);

    //fresh style
    for(int i = 0; i < 9; ++i) for(int j = 0; j < 9; ++j) {
        SudokuBox *box = getBox(sudokuModel->index(i, j));
        box->setStyleSheet(TEXT_STYLE_MAGIC(box) + BG_STYLE_MAGIC(i, j));
    }

}

void GameArea::keyPressEvent(QKeyEvent *e) {
    int k = e->key();
    QModelIndex curId, newId;
    switch (k) {
    case Qt::Key_0: case Qt::Key_C:
        ui->clearButton->click();
        break;
    case Qt::Key_1:
        ui->numberButton1->click();
        break;
    case Qt::Key_2:
        ui->numberButton2->click();
        break;
    case Qt::Key_3:
        ui->numberButton3->click();
        break;
    case Qt::Key_4:
        ui->numberButton4->click();
        break;
    case Qt::Key_5:
        ui->numberButton5->click();
        break;
    case Qt::Key_6:
        ui->numberButton6->click();
        break;
    case Qt::Key_7:
        ui->numberButton7->click();
        break;
    case Qt::Key_8:
        ui->numberButton8->click();
        break;
    case Qt::Key_9:
        ui->numberButton9->click();
        break;
    case Qt::Key_H:
        ui->hintButton->click();
        break;
    case Qt::Key_P:
        ui->pauseButton->click();
        break;
    case Qt::Key_F2:
        ui->restartButton->click();
        break;
    case Qt::Key_Up:
        curId = ui->sudokuTable->selectionModel()->currentIndex();
        if(curId.row() - 1 < 0) break;
        newId = sudokuModel->index(curId.row() - 1, curId.column());
        ui->sudokuTable->selectionModel()->setCurrentIndex(newId, QItemSelectionModel::Current);
        break;
    case Qt::Key_Down:
        curId = ui->sudokuTable->selectionModel()->currentIndex();
        if(curId.row() + 1 >= 9) break;
        newId = sudokuModel->index(curId.row() + 1, curId.column());
        ui->sudokuTable->selectionModel()->setCurrentIndex(newId, QItemSelectionModel::Current);
        break;
    case Qt::Key_Left:
        curId = ui->sudokuTable->selectionModel()->currentIndex();
        if(curId.column() - 1 < 0) break;
        newId = sudokuModel->index(curId.row(), curId.column() - 1);
        ui->sudokuTable->selectionModel()->setCurrentIndex(newId, QItemSelectionModel::Current);
        break;
    case Qt::Key_Right:
        curId = ui->sudokuTable->selectionModel()->currentIndex();
        if(curId.column() + 1 >= 9) break;
        newId = sudokuModel->index(curId.row(), curId.column() + 1);
        ui->sudokuTable->selectionModel()->setCurrentIndex(newId, QItemSelectionModel::Current);
        break;
    case Qt::Key_Z:
        ui->undoButton->click();
        break;
    case Qt::Key_X:
        ui->redoButton->click();
        break;
    default:
        QWidget::keyPressEvent(e);
        break;
    }
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
    ui->hintButton->setEnabled(active);
    ui->commitButton->setEnabled(active);
    ui->restartButton->setEnabled(active);
    ui->pauseButton->setEnabled(active);
    if(active) {
        timer->start();
        freshUndoRedoButtons();
        freshClearButton(getCurrentBox());
        freshNumberButtons(getCurrentBox());
    }
    else {
        timer->stop();
        ui->undoButton->setEnabled(false);
        ui->redoButton->setEnabled(false);
        ui->clearButton->setEnabled(false);
        for(int i = 1; i <= 9; ++i) numberButtonGroup->button(i)->setEnabled(false);
    }
}

void GameArea::readQSS(QString *dest, QString srcFile) {
    QFile *qssFile;
    qssFile= new QFile(srcFile);
    if(qssFile->open(QFile::ReadOnly)) *dest = QString(qssFile->readAll());
    qssFile->close();
    delete qssFile;
}

void GameArea::freshStyle(QModelIndex current, int curN, QModelIndex previous, int preN) {
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

        if(preN) for(int i = 0; i < 9; ++i) for(int j = 0; j < 9; ++j) {
            b = getBox(sudokuModel->index(i, j));
            if(b->getNumber() == preN) b->setStyleSheet(TEXT_STYLE_MAGIC(b) + BG_STYLE_MAGIC(i, j));
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

        if(curN) for(int i = 0; i < 9; ++i) for(int j = 0; j < 9; ++j) {
            b = getBox(sudokuModel->index(i, j));
            if(b->getNumber() == curN) b->setStyleSheet(TEXT_STYLE_MAGIC(b) + sameNumBoxStyle);
        }
    }
    getBox(current)->setStyleSheet(TEXT_STYLE_MAGIC(getBox(current)) + selectedBoxStyle);

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

SudokuBox *GameArea::getCurrentBox() {
    QModelIndex id = ui->sudokuTable->selectionModel()->currentIndex();
    if(!id.isValid()) return nullptr;
    else return getBox(id);
}

bool GameArea::makeMarkOn(SudokuBox *box, int number, bool marked) {
    if(box == nullptr) return false;
    markFlag oldF = box->getMarks();
    bool result = box->setMark(number, marked);
    markFlag newF = box->getMarks();

    //add in UndoStack.
    if(result) undoStack->push(new SudokuCommand(box, oldF, newF));

    return result;
}

bool GameArea::setMarkOn(SudokuBox *box, markFlag f) {
    markFlag oldF = box->getMarks();
    bool result = box->setMarkFlag(f);

    //add in UndoStack.
    if(result) undoStack->push(new SudokuCommand(box, oldF, f));

    return result;
}

bool GameArea::clearMark(SudokuBox *box) {
    return setMarkOn(box, markFlag());
}

void GameArea::on_numberButton_clicked(int num, bool checked) {
    SudokuBox* box = getCurrentBox();
    if(box == nullptr) {
        numberButtonGroup->button(num)->setChecked(!checked);
        return;
    }
    int preN = box->getNumber();
    if(!makeMarkOn(box, num, checked)) { //设置mark失败时，不改变numberButton.checked状态
        numberButtonGroup->button(num)->setChecked(!checked);
        return;
    }
    numberSE.play();
    freshUndoRedoButtons();
    freshClearButton(box);
    QModelIndex id = ui->sudokuTable->selectionModel()->currentIndex();
    freshStyle(id, box->getNumber(), id, preN);
}

void GameArea::on_currentBox_changed(QModelIndex current, QModelIndex previous) {
    SudokuBox *curBox = getBox(current);
    SudokuBox *preBox = getBox(previous);
    int curN = curBox == nullptr ? 0 : curBox->getNumber();
    int preN = preBox == nullptr ? 0 : preBox->getNumber();

    freshNumberButtons(curBox);
    freshClearButton(curBox);
    freshStyle(current, curN, previous, preN);
}

void GameArea::on_pauseButton_clicked(bool checked) {
    if(checked){
        setGameAreaActive(false);
        ui->pauseButton->setEnabled(true);

        //遮挡棋盘.
        ui->sudokuTable->setVisible(false);
        ui->PausePicture->setVisible(true);

    } else {
        setGameAreaActive(true);
        ui->pauseButton->setEnabled(true);

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

    QModelIndex id = ui->sudokuTable->currentIndex();
    SudokuBox* box = getCurrentBox();
    int preN = box->getNumber();
    freshUndoRedoButtons();
    freshNumberButtons(box);
    freshStyle(id, box->getNumber(), id, preN);
}

void GameArea::on_redoButton_clicked() {
    redoAction->trigger();

    QModelIndex id = ui->sudokuTable->currentIndex();
    SudokuBox* box = getCurrentBox();
    int preN = box->getNumber();
    freshUndoRedoButtons();
    freshNumberButtons(getCurrentBox());
    freshStyle(id, box->getNumber(), id, preN);
}

void GameArea::on_clearButton_clicked() {
    SudokuBox* box = getCurrentBox();
    if(box == nullptr) return;
    int preN = box->getNumber();
    clearMark(box);

    freshUndoRedoButtons();
    freshNumberButtons(getCurrentBox());

    QModelIndex id = ui->sudokuTable->selectionModel()->selectedIndexes().at(0);
    freshStyle(id, box->getNumber(), id, preN);

}

void GameArea::on_hintButton_clicked() {
    QModelIndex id = ui->sudokuTable->selectionModel()->currentIndex();
    SudokuBox* box = getCurrentBox();
    if(box == nullptr || !box->isEditable()) {
        QMessageBox::warning(this, "Sudoku-Game", "Select a box and click again to get the answer.", QMessageBox::Ok);
        return;
    }
    markFlag f;
    f.setflag(answer[9 * id.row() + id.column()].toLatin1() - '0', true);
    int preN = box->getNumber();
    if( setMarkOn(box, f) ) {
        ++hintUsed;
        freshStyle(id, box->getNumber(), id, preN);
        freshUndoRedoButtons();
        freshClearButton(box);
        freshNumberButtons(box);
        hintSE.play();
    } else QMessageBox::warning(this, "Sudoku-Game", "You are right in this box!", QMessageBox::Ok);
}

void GameArea::on_restartButton_clicked() {
    int re = QMessageBox::warning(this, "Sudoku-Game", "Are you sure to RESTART?", QMessageBox::Yes | QMessageBox::No);
    if(re == QMessageBox::Yes) {
        loadProblem(&problem, &answer);
    }
}

void GameArea::on_commitButton_clicked() {
    SudokuBox *box;
    for(int i = 0; i < 9; ++i) for(int j = 0; j < 9; ++j) {
        box = getBox(sudokuModel->index(i, j));
        int num = box->getNumber();
        if(num == 0) {
            ui->sudokuTable->selectionModel()->select(sudokuModel->index(i, j), QItemSelectionModel::Select);
            ui->sudokuTable->selectionModel()->setCurrentIndex(sudokuModel->index(i, j), QItemSelectionModel::Current);
            QMessageBox::warning(this, "Sudoku-Game", QString("You didn't finish the puzzle!"), QMessageBox::Yes);
            return;
        }
        if(num != answer[9 * i + j].toLatin1() - '0') {
            ui->sudokuTable->selectionModel()->select(sudokuModel->index(i, j), QItemSelectionModel::Select);
            ui->sudokuTable->selectionModel()->setCurrentIndex(sudokuModel->index(i, j), QItemSelectionModel::Current);
            QMessageBox::warning(this, "Sudoku-Game", QString("You made an mistake, check again!"), QMessageBox::Ok);
            return;
        }
    }
    winSE.play();
    setGameAreaActive(false);
    QMessageBox::information(this, "Sudoku-Game", QString("You Win!\nHint used: %0").arg(hintUsed), QMessageBox::Ok);
}
