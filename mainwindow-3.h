#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    struct node { int step; int koeff; node *next; };
    struct node_of_nodes { int numb; node *Nnode; node_of_nodes *next; };
    // L - указатель на сверхсписок, L2 - указатель на буфер
    node_of_nodes *L = nullptr;
    node *L2 = nullptr;

private slots:
    //одномногочленные
    void on_deleter_clicked();
    void on_pushButton_ZNACHENIA_clicked();
    void on_pushButton_PROIZVOD_clicked();
    void on_pushButton_KORNI_clicked();
    void on_pushButton_READ_clicked();  //считывает с экрана

    //димногочленные
    void DELENIE();
    void on_pushButton_multiply_clicked();

    //служебные математические
    void sorti( node *&Left );
    void list_refresh();  //к deleter
    bool PreCompilator();
    void bufer_creator();  //выводит значения буфера
    int HitrayaStepen( int koef, int step );
    double HitrayaStepenDouble( double koef, int step );
    bool checkKorni( int X );  //к поиску корней
    void slozenie_slianie( node *L1, node *L2 ); // к функции деления с остатком

    //служебные для изменения списков
    void DOBAVLENIE();  //перестраивает сверхсписок
    void bufer_deleter();  //чистит буфер
    void on_pushButton_SLOZHENIE_clicked();
};

#endif // MAINWINDOW_H
