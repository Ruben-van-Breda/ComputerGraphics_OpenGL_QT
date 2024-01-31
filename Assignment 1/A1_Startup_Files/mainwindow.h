#pragma once

#include "glwidget.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    GLWidget *opengl;
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
