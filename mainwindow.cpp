#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton_BUFER, &QPushButton::clicked, this, &MainWindow::DOBAVLENIE);
    connect(ui->pushButton_DELITI, &QPushButton::clicked, this, &MainWindow::DELENIE);
    connect(ui->pushButton_DELOST, &QPushButton::clicked, this, &MainWindow::DELENIE);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::PreCompilator() {
    QString s = "";
    s += ui->lineEdit->text();
    s += ";";
    // признак конца строки
    int i;
    bool DALEE = true;
    try {
//начало try
        //первый проход - пробелы
        i = 0;
        while ( s[i] != ";" ) {
            if ( s[i] == " " ) {
                s.remove(i, 1);
                continue;
            }
            ++i;
        }
        //если строка пуста - вылетаем
        if ( s == ";;" ) {
            throw 100;
        }
        i = 0;
        if ( s[i] == "+" || s[i] == "-" ) {
            ++i;
        }  //если вначале + или -, то это норм
        while ( s[i] != ";" )
        {
            if ( s[i] >= '1' && s[i] <= '9' ) {
                while ( s[i] >= '0' && s[i] <= '9' ) {
                    ++i;
                }
            } else {
                if ( s[i] == "0" ) {
                    throw 800;
                }
                throw 200;
            } //если нет цифры - плохо

            if ( s[i] == "x" ) {
                ++i;
            } else {
                throw 300;
            }

            if ( s[i] == "^" ) {
                ++i;
            } else {
                throw 400;
            }

            if ( s[i] == "-" || s[i] == "+" ) {
                ++i;
            }

            if ( s[i] >= '0' && s[i] <= '9' ) {
                while ( s[i] >= '0' && s[i] <= '9' ) {
                    ++i;
                }
            } else {
                throw 500;
            }

            if ( s[i] == "+" || s[i] == "-" ) {
                ++i;
                if ( s[i] == "+" || s[i] == "-" ) {
                    throw 600;
                }
                if ( s[i] == ";" ) {
                    throw 700;
                }
            }
        }
//конец try
    } catch ( int j ) {
        //какие могут быть ошибки:
        if ( j == -1 ) {
            QMessageBox::information(this, "неправильная запись", "неизвестный символ");
        }
        if ( j == 100 ) {
            QMessageBox::information(this, "неправильная запись", "строка пуста");
        }
        if ( j == 200 ) {
            QMessageBox::information(this, "неправильная запись", "перед одним из иксов не стоит коэффициент");
        }
        if ( j == 300 ) {
            QMessageBox::information(this, "неправильная запись", "после коэффициента должен идти X");
        }
        if ( j == 400 ) {
            QMessageBox::information(this, "неправильная запись", "после X должен идти ^");
        }
        if ( j == 500 ) {
            QMessageBox::information(this, "неправильная запись", "после X^ должен идти коэффициент");
        }
        if ( j == 600 ) {
            QMessageBox::information(this, "неправильная запись", "подрят стоят 2 знака арифметических действия");
        }
        if ( j == 700 ) {
            QMessageBox::information(this, "неправильная запись", "+- в кнцце строки");
        }
        if ( j == 800 ) {
            QMessageBox::information(this, "неправильная запись", "коэффициент не может начинаться с 0");
        }
        DALEE = false;
    }

    if ( DALEE ) {
        s.remove(s.size()-1, 1);
        ui->lineEdit->setText(s);
        return true;
    } else {
        return false;
    }
    //если вернёт false, то перевод строки в список не происходит
}

void MainWindow::on_pushButton_READ_clicked()
{
        if ( !PreCompilator() ) {
            return;
        }

    //удалим из буфера то, что там есть
    bufer_deleter();

    node *N;

    //переведём строку в многочлен
    QString s = ui->lineEdit->text();
    s += ";";
    QString support;
    int pos = 0;
    int plus;
    while ( s[pos] != ";" ) {
        N = new node;

        plus = 1;
        if ( s[pos] == "+" ) {
            ++pos;
        }
        if ( s[pos] == "-" ) {
            plus = -1;
            ++pos;
        }

        support.clear();
        while ( s[pos] >= '0' && s[pos] <= '9' ) {
            support += s[pos];
            ++pos;
        }
        N->koeff = support.toInt()*plus;

        if ( s[pos] == "x" ) {
            pos += 2;
        }
        support.clear();
        plus = 1;
        if ( s[pos] == "-" ) {
            plus = -1;
            ++pos;
        }
        if ( s[pos] == "+" ) {
            ++pos;
        }
        while ( s[pos] >= '0' && s[pos] <= '9' ) {
            support += s[pos];
            ++pos;
        }
        N->step = support.toInt()*plus;
        N->next = L2;
        L2 = N;
    }

    //отсортируем
    sorti( L2 );

    //приведём одинаковые степени
    N = L2;
    node *N2;
    if ( L2 == nullptr ) {
        QMessageBox::critical(this, "ошибка записи", "введите хоть что-нибудь");
        return;
    }
    //приводим равные
    while ( N->next != nullptr ) {
        if ( N->next->koeff == 0 ) {
            N2 = N->next;
            N->next = N->next->next;
            delete N2;
            continue;
        }
        if ( N->step == N->next->step ) {
            N2 = N->next;
            N->next = N->next->next;
            N->koeff = N->koeff + N2->koeff;
            delete N2;
            continue;
        }
        N=N->next;
    }
    if ( L2 != nullptr ) {
        if ( L2->koeff == 0 ) {
            N2 = L2;
            L2 = L2->next;
            delete L2;
        }
    }

    //занесём в буфер имеющееся, но при чтении с файла выводить не будем
    bufer_creator();

    return;
}

void MainWindow::bufer_creator()
{
    ui->listWidget_BUFER->clear();
    if ( L2 == nullptr ) {
        ui->listWidget_BUFER->addItem("0");
        return;
    }
    QString support = "";
    node *N = L2;;
    while ( N != nullptr ) {
        if ( N->koeff > 0 ) {
            if ( N != L2 ) {
                support += "+";
            }
        }
        if ( N->koeff != 1 || ( N->koeff == 1 && N->step == 0 ) ) {
            support += QString::number(N->koeff);
        }
        if ( N->step == 1 ) {
            support += "x";
        }
        if ( N->step != 1 && N->step != 0 ) {
            support += "x^";
            support += QString::number(N->step);
        }
        N = N->next;
    }
    ui->listWidget_BUFER->addItem(support);
    return;
}

void MainWindow::bufer_deleter()
{
    node *N;
    while ( L2 != nullptr ) {
        N = L2;
        L2 = L2->next;
        delete N;
    }
    return;
}

void MainWindow::sorti ( node *&Left )
{
    if ( Left == nullptr ) {
        return;
    }
    if ( Left->next == nullptr ) {
        return;
    }
    int i = 0;
    node *runner = Left, *stator = Left, *runner2;
    while ( runner->next != nullptr ) {
        runner = runner->next;
        if ( i & 1 ) {
            stator = stator->next;
        }
        ++i;
    }

    sorti ( stator->next );
    runner2 = stator->next;
    stator->next = nullptr;
    sorti ( Left );
    runner = Left;

        if ( runner->step > runner2->step ) {
            Left = runner2;
            runner2 = runner2->next;
        } else {
            runner = runner->next;
        }
        stator = Left;

        while ( runner != nullptr || runner2 != nullptr ) {
            if ( runner == nullptr ) {
                stator->next = runner2;
                stator = runner2;
                runner2 = runner2->next;
                continue;
            }
            if ( runner2 == nullptr ) {
                stator->next = runner;
                stator = runner;
                runner = runner->next;
                continue;
            }
            if ( runner->step < runner2->step ) {
                stator->next = runner;
                stator = runner;
                runner = runner->next;
            } else {
                stator->next = runner2;
                stator = runner2;
                runner2 = runner2->next;
            }
        }
        stator->next = nullptr;
    return;
}

void MainWindow::DOBAVLENIE()
{
    if ( L2 == nullptr ) {
        QMessageBox::information(this, "ошибка", "буфер пуст или не содержит ненулевых коэффициентов");
        return;
    }
    node_of_nodes *NoN;
    node *N;
    //перенумеровка, вставка в голову нового элемента
    NoN = L;
    for ( int i = 2; NoN != nullptr; ++i ) {
        NoN->numb = i;
        NoN = NoN->next;
    }
    NoN = new node_of_nodes;
    NoN->numb = 1;
    NoN->next = L;
    L = NoN;
    L->Nnode = L2;
    N = L->Nnode;

    //отщепим L2
    L2 = new node;
    node *tail = L2, *helper;
    while ( N != nullptr ) {
        helper = new node;
        helper->step = N->step;
        helper->koeff = N->koeff;
        tail->next = helper;
        tail = tail->next;
        N = N->next;
    }
    tail->next = nullptr;
    helper = L2;
    L2 = L2->next;
    delete helper;

    //вывод на листвиджет
    list_refresh();
    return;
}

void MainWindow::list_refresh()
{
    node *N;
    node_of_nodes *NoN;
    QString support;

    ui->listWidget->clear();
    NoN = L;
    while ( NoN != nullptr ) {
        //заполняем строку и выводим её в листВиджет
        //и так n раз
        support.clear();
        support += QString::number(NoN->numb);
        support += ") ";
        N = NoN->Nnode;
        while ( N != nullptr ) {
            if ( N->koeff > 0 ) {
                if ( N != L2 ) {
                    support += "+";
                }
            }
            if ( N->koeff != 1 || ( N->koeff == 1 && N->step == 0 ) ) {
                support += QString::number(N->koeff);
            }
            if ( N->step == 1 ) {
                support += "x";
            }
            if ( N->step != 1 && N->step != 0 ) {
                support += "x^";
                support += QString::number(N->step);
            }
            N = N->next;
        }
        ui->listWidget->addItem(support);
        NoN = NoN->next;
    }
}

void MainWindow::on_deleter_clicked()
{
    if ( L == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    int number;
    node_of_nodes *NoN = L, *NoN_helper;
    node *N, *N_helper;
    number = ui->spinBox_DELETER->value();
    if ( number == 1  ) {
        NoN = L;
        L = L->next;
        N = NoN->Nnode;
        delete NoN;
        NoN = L;
    } else {
        //если элемент не первый, то идёт операция "удаление следущего"
        for ( int i = 2; i < number && NoN->next != nullptr; ++i ) {
            NoN = NoN->next;
        }
        if ( NoN->next == nullptr || number <= 0 ) {
            QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
            return;
        }
        NoN_helper = NoN->next;
        N = NoN->next->Nnode;
        NoN->next = NoN->next->next;
        delete NoN_helper;
        NoN = NoN->next;
    }

    //удаляем минисписок
    while ( N != nullptr ) {
        N_helper = N;
        N = N->next;
        delete N_helper;
    }

    //перенумеровка
    while ( NoN != nullptr ) {
        NoN->numb -= 1;
        NoN = NoN->next;
    }

    list_refresh();
    return;
}

void MainWindow::on_pushButton_ZNACHENIA_clicked()
{
    if ( L == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    node *N;
    node_of_nodes *NoN;
    QString support = ui->lineEdit_ZNACH->text();
    if ( support.isEmpty() ) {
        support = "0";
        ui->lineEdit_ZNACH->setText("0");
    }

    //проверка на double
    support += ";";
    int i = 0;
    for ( ; support[i] != ";"; ++i ) {
        if ( i == 0 && ( support[i] == "-" || support[i] == "+" ) ) {
            ++i;
        }
        if ( support[i] < '0' || support[i] > '9' ) {
            break;
        }
    }
    if ( support[i] == "." || support[i] == "," ) {
        if ( support[i] == "," ) {
            support.remove(i, 1);
            support.insert(i, ".");
        }
        ++i;
    }
    for ( ; support[i] != ";"; ++i ) {
        if ( support[i] < '0' || support[i] > '9' ) {
            break;
        }
    }
    if ( support[i] != ";" ) {
        QMessageBox::critical(this, "рукокрюкий", "вы ввели не вещественное число");
    }
    support.remove(i, 1);
    double X = support.toDouble();

    NoN = L;
    int funcNumb = ui->spinBox_DELETER->value();
    for ( int i = 1; i < funcNumb && NoN != nullptr; ++i ) {
        NoN = NoN->next;
    }
    if ( funcNumb == 0 || NoN == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }

    //считаем многочлен
    double H;
    N = NoN->Nnode;
    double ZNACH = 0;
    while ( N != nullptr ) {
        if ( N->step == 0 ) {
            ZNACH += N->koeff;
        } else {
            H = N->koeff;
            ZNACH += H * 1.0 * HitrayaStepenDouble( X, N->step );
        }
        N = N->next;
    }
    support = QString::number(ZNACH);
    ui->label_ZNACHENIA->setText(support);
    return;
}

int MainWindow::HitrayaStepen( int koef, int step )
{
    if ( koef == 0 ) {
        return 0;
    }
    if ( step == 1 ) {
        return koef;
    }
    if ( step == 0 ) {
        return 1;
    }
    int i = step / 2;
    return( HitrayaStepen(koef, i) * HitrayaStepen(koef, step-i) );
}

double MainWindow::HitrayaStepenDouble( double koef, int step )
{
    if ( step == -1 ) {
        return 1/koef;
    }
    if ( step == 1 ) {
        return koef;
    }
    if ( step == 0 ) {
        return 1;
    }
    int i = step / 2;
    return( HitrayaStepenDouble(koef, i) * HitrayaStepenDouble(koef, step-i) );
}

void MainWindow::on_pushButton_SLOZHENIE_clicked()
{
    if ( L == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    int number;
    node *N1, *N2;
    number = ui->spinBox_1slag->value();
    node_of_nodes *NoN = L;
    for ( int i = 1; i < number && NoN != nullptr; ++i ) {
        NoN = NoN->next;
    }
    if ( number == 0 || NoN == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    N1 = NoN->Nnode;
    number = ui->spinBox_2slag->value();
    NoN = L;
    for ( int i = 1; i < number && NoN != nullptr; ++i ) {
        NoN = NoN->next;
    }
    if ( number == 0 || NoN == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    N2 = NoN->Nnode;

    //слияние
    bufer_deleter();
    node *helper, *tail;
    tail = new node;
    L2 = tail;
    while ( N1 != nullptr || N2 != nullptr ) {
        helper = new node;
        if ( N1 == nullptr ) {
            helper->koeff = N2->koeff;
            helper->step = N2->step;
            N2 = N2->next;
            tail->next = helper;
            tail = tail->next;
            continue;
        }
        if ( N2 == nullptr ) {
            helper->koeff = N1->koeff;
            helper->step = N1->step;
            N1 = N1->next;
            tail->next = helper;
            tail = tail->next;
            continue;
        }
        if ( N1->step == N2->step ) {
            helper->koeff = N1->koeff + N2->koeff;
            helper->step = N1->step;
            if ( helper->koeff == 0 ) {
                delete helper;
            } else {
                tail->next = helper;
                tail = tail->next;
            }
            N1 = N1->next;
            N2 = N2->next;
            continue;
        }
        if ( N1->step < N2->step ) {
            helper->koeff = N1->koeff;
            helper->step = N1->step;
            N1 = N1->next;
            tail->next = helper;
            tail = tail->next;
        } else {
            helper->koeff = N2->koeff;
            helper->step = N2->step;
            N2 = N2->next;
            tail->next = helper;
            tail = tail->next;
        }
    }
    tail->next = nullptr;
    helper = L2;
    L2 = L2->next;
    delete helper;
    if ( L2 == nullptr ) {
        QMessageBox::information(this, "операция прошла успешно", "но результат равен нулю, поэтому в буфер мы его не занесём");
        L2 = new node;
        L2->next = nullptr;
        L2->step = 1;
        L2->koeff = 1;
        bufer_creator();
        return;
    }
    bufer_creator();
    return;
}

void MainWindow::on_pushButton_PROIZVOD_clicked()
{
    if ( L == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    int PROIZVOD = ui->spinBox_2slag->value();
    int i = 0;
    int number = ui->spinBox_1slag->value();
    node_of_nodes *NoN = L;
    node *N;
    for ( i = 1; i < number && NoN != nullptr; ++i ) {
        NoN = NoN->next;
    }
    if ( NoN == nullptr || number == 0 ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    N = NoN->Nnode;
    bufer_deleter();
    L2 = new node;
    node *helper, *tail = L2;
    while ( N != nullptr ) {
        if ( N->step < PROIZVOD && N->step >= 0 ) {
            N = N->next;
            continue;
        }
        helper = new node;
        helper->step = N->step;
        helper->koeff = N->koeff;
        for ( int i = 0; i < PROIZVOD; ++i ) {
            helper->koeff *= helper->step;
            helper->step -= 1;
        }
        tail->next = helper;
        tail = tail->next;
        N = N->next;
    }
    tail->next = nullptr;
    helper = L2;
    L2 = L2->next;
    delete helper;
    if ( L2 == nullptr ) {
        QMessageBox::information(this, "гуманитарий", "результат равен нулю, и в буфер мы его не занесём");
        L2 = new node;
        L2->koeff = 1;
        L2->step = 0;
        L2->next = nullptr;
    }
    bufer_creator();
    return;
}

void MainWindow::on_pushButton_multiply_clicked()
{
    if ( L == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    int number;
    node *N1, *N2, *helper, *newer;
    number = ui->spinBox_1slag->value();
    node_of_nodes *NoN = L;
    for ( int i = 1; i < number && NoN != nullptr; ++i ) {
        NoN = NoN->next;
    }
    if ( number == 0 || NoN == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    N1 = NoN->Nnode;
    number = ui->spinBox_2slag->value();
    NoN = L;
    for ( int i = 1; i < number && NoN != nullptr; ++i ) {
        NoN = NoN->next;
    }
    if ( number == 0 || NoN == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    N2 = NoN->Nnode;
    helper = N2;

    bufer_deleter();
    L2 = nullptr;
    //перемножаем всё вперемешку
    while ( N1 != nullptr ) {
        N2 = helper;
        while ( N2 != nullptr ) {
            newer = new node;
            newer->koeff = N1->koeff * N2->koeff;
            newer->step = N1->step + N2->step;
            newer->next = L2;
            L2 = newer;
            N2 = N2->next;
        }
        N1 = N1->next;
    }

    sorti(L2);

    //сложим степени
    N1 = L2;
    N2 = L2->next;
    while ( N2 != nullptr ) {
        if ( N1->step == N2->step ) {
            N1->koeff += N2->koeff;
            N1->next = N2->next;
            delete N2;
            N2 = N1->next;
        } else {
            N1 = N1->next;
            N2 = N2->next;
        }
    }

    //уберём koeff == 0
    N1 = L2;
    N2 = L2->next;
    while ( N2 != nullptr ) {
        if ( N2->koeff == 0 ) {
            N1->next = N2->next;
            delete N2;
            N2 = N1->next;
        } else {
            N1 = N1->next;
            N2 = N2->next;
        }
    }
    if ( L2->koeff == 0 ) {
        helper = L2;
        L2 = L2->next;
        delete helper;
    }

    bufer_creator();

    return;
}

void MainWindow::on_pushButton_KORNI_clicked()
{
    if ( L == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    node *N;
    node_of_nodes *NoN;

    NoN = L;
    int funcNumb = ui->spinBox_DELETER->value();
    for ( int i = 1; i < funcNumb && NoN != nullptr; ++i ) {
        NoN = NoN->next;
    }
    if ( funcNumb == 0 || NoN == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }

    //дело
    int SVOBOD = NoN->Nnode->koeff;
    if ( SVOBOD < 0 ) {
        SVOBOD = -SVOBOD;
    }

    N = NoN->Nnode;
    int minstep = N->step;
    while ( N != nullptr ) {
        N->step -= minstep;
        N = N->next;
    }

    //подержи информацию, плиз
    N = L2;
    //L2 есть в checkKorni
    L2 = NoN->Nnode;

    QString support = "";
    if ( minstep > 0 ) {
        support += "0 ";
    }

    for ( int i = 0; i <= SVOBOD; ++i ) {
        if (checkKorni(i)) {
            support = support + QString::number(i) + " ";
        }
        if (checkKorni(-i)) {
            support = support + QString::number(-i) + " ";
        }
    }

    L2 = N;
    N = NoN->Nnode;
    while ( N != nullptr ) {
        N->step += minstep;
        N = N->next;
    }

    if ( support.isEmpty() ) {
        support = "корней нет";
    }
    ui->label_KORNI->setText(support);

    return;
}

bool MainWindow::checkKorni(int X)
{
    node *runner = L2;

    if ( X == 0 ) {
        while ( runner != nullptr ) {
            if ( runner->step <= 0 ) {
                return false;
            }
            runner = runner->next;
        }
        return true;
    }

    int sum = 0;
    while ( runner != nullptr ) {
        sum += runner->koeff * HitrayaStepen(X, runner->step);
        runner = runner->next;
    }
    if ( sum == 0 ) {
        return true;
    } else {
        return false;
    }
}

void MainWindow::slozenie_slianie( node *N1, node *N2 )
{
    node *helper, *tail;
    tail = new node;
    L2 = tail;
    while ( N1 != nullptr || N2 != nullptr ) {
        helper = new node;
        if ( N1 == nullptr ) {
            helper->koeff = N2->koeff;
            helper->step = N2->step;
            N2 = N2->next;
            tail->next = helper;
            tail = tail->next;
            continue;
        }
        if ( N2 == nullptr ) {
            helper->koeff = N1->koeff;
            helper->step = N1->step;
            N1 = N1->next;
            tail->next = helper;
            tail = tail->next;
            continue;
        }
        if ( N1->step == N2->step ) {
            helper->koeff = N1->koeff + N2->koeff;
            helper->step = N1->step;
            if ( helper->koeff == 0 ) {
                delete helper;
            } else {
                tail->next = helper;
                tail = tail->next;
            }
            N1 = N1->next;
            N2 = N2->next;
            continue;
        }
        if ( N1->step > N2->step ) {
            helper->koeff = N1->koeff;
            helper->step = N1->step;
            N1 = N1->next;
            tail->next = helper;
            tail = tail->next;
        } else {
            helper->koeff = N2->koeff;
            helper->step = N2->step;
            N2 = N2->next;
            tail->next = helper;
            tail = tail->next;
        }
    }
    tail->next = nullptr;
    helper = L2;
    L2 = L2->next;
    delete helper;
    return;
}

void MainWindow::DELENIE()
{
    if ( L == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    int number;
    node *N1, *N2, *helper;
    number = ui->spinBox_1slag->value();
    node_of_nodes *NoN = L;
    for ( int i = 1; i < number && NoN != nullptr; ++i ) {
        NoN = NoN->next;
    }
    if ( number == 0 || NoN == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    N1 = NoN->Nnode;
    number = ui->spinBox_2slag->value();
    NoN = L;
    for ( int i = 1; i < number && NoN != nullptr; ++i ) {
        NoN = NoN->next;
    }
    if ( number == 0 || NoN == nullptr ) {
        QMessageBox::warning(this, "ошибка", "несуществующий номер многочлена");
        return;
    }
    N2 = NoN->Nnode;

    //инвертируем порядок, чтобы вначале шёл максимальный коэффициент
    node *delimoe = nullptr, *delitel = nullptr;
    while ( N1 != nullptr ) {
        helper = new node;
        helper->koeff = N1->koeff;
        helper->step = N1->step;
        helper->next = delimoe;
        delimoe = helper;
        N1 = N1->next;
    }
    while ( N2 != nullptr ) {
        helper = new node;
        helper->koeff = N2->koeff;
        helper->step = N2->step;
        helper->next = delitel;
        delitel = helper;
        N2 = N2->next;
    }

    //само деление
    node *chastnoe = nullptr;
    try {
        bufer_deleter();
        int koef_del, step_del;
        while ( delimoe->step >= delitel->step ) {
            if ( delimoe->koeff % delitel->koeff != 0 ) {
                throw 1;
            }
            koef_del = -1 * delimoe->koeff / delitel->koeff;
            step_del = delimoe->step - delitel->step;
            //домножим делитель так, чтобы при сложении с делимым он аннигилировал максимальную степень
            //потом вернём как было

            helper = delitel;
            while ( helper != nullptr ) {
                helper->koeff = helper->koeff * koef_del;
                helper->step = helper->step + step_del;
                helper = helper->next;
            }

            slozenie_slianie(delimoe, delitel);

            while ( delimoe != nullptr ) {
                helper = delimoe;
                delimoe = delimoe->next;
                delete helper;
            }
            delimoe = L2;
            L2 = nullptr;

            helper = new node;
            helper->koeff = -koef_del;
            helper->step = step_del;
            helper->next = chastnoe;
            chastnoe = helper;

            helper = delitel;
            while ( helper != nullptr ) {
                helper->koeff = helper->koeff / koef_del;
                helper->step = helper->step - step_del;
                helper = helper->next;
            }
            if ( delimoe == nullptr ) {
                throw 2;
            }
        }
    } catch ( int i ) {
        if ( i == 1 ) {
            QMessageBox::information(this, "невозможно произвести действие",
             "появляется дробный коэффициент. попробуйте домножить делимое на коэффициент максимального члена делителя");
            bufer_deleter();
            L2 = new node;
            L2->next = nullptr;
            L2->koeff = 1;
            L2->step = 1;
            bufer_creator();
            return;
        }

    }

    QPushButton* button = qobject_cast< QPushButton* >( sender() );
    if ( button->text() == "/" ) {
        bufer_deleter();
        while ( chastnoe != nullptr ) {
            helper = chastnoe;
            chastnoe = chastnoe->next;
            helper->next = L2;
            L2 = helper;
        }
        while ( delimoe != nullptr ) {
            helper = delimoe;
            delimoe = delimoe->next;
            delete helper;
        }
    } else {
        bufer_deleter();
        while ( delimoe != nullptr ) {
            helper = delimoe;
            delimoe = delimoe->next;
            helper->next = L2;
            L2 = helper;
        }
        while ( chastnoe != nullptr ) {
            helper = chastnoe;
            chastnoe = chastnoe->next;
            delete helper;
        }
    }

    bufer_creator();

    return;
}
