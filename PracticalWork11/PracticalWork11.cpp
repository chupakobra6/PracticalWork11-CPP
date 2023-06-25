// PracticalWork11.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "Authorization.h"


// Точка входа в программу, запускаемый файл, авторизация
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    shared_ptr<Authorization> authorization = make_shared<Authorization>();
    int choice;

    do {
        cout << "1. Войти" << endl;
        cout << "2. Войти как гость" << endl;
        cout << "3. Зарегистрироваться" << endl;
        cout << "4. Выйти" << endl;
        cout << "Выберите действие: " << endl;
        choice = getInputInt();

        switch (choice) {
        case 1:
            authorization->login();
            break;
        case 2:
            authorization->loginAsGuest();
            break;
        case 3:
            authorization->registerEmployee();
            break;
        case 4:
            break;
        default:
            cout << "Неверный выбор." << endl;
            break;
        }
    } while (choice != 4);

    return 0;
}
