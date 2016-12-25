#ifndef TAB1_H
#define TAB1_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QIntValidator>
#include <QGridLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QtGlobal>
#include <vector>
#include <QObject>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include "drawnn.h"

class Tab1 : public QWidget
{
    Q_OBJECT
public:
    Tab1();
    ~Tab1();
public slots:
    void resetInputFormNeurons();
    void loadData();
    void selectData();
private:
    QGridLayout *grid;
    QPushButton *load_data_button;
    QPushButton *add_layer_button;
    QLineEdit * inputFormNeurons;
    QLabel *labelForDebug;
    QString data;
};

#endif // TAB1_H
