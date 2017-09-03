#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = 0);
    ~GameWindow();

private slots:
    void loadPuzzle(int lv);

private:
    Ui::GameWindow *ui;
    QString *p[11], *a[11];
};

#endif // GAMEWINDOW_H
