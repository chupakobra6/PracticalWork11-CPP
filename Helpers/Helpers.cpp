// Helpers.cpp : Определяет функции для статической библиотеки.
//

#include "pch.h"
#include "framework.h"

#include "HelpersHeader.h"

// Хэширование пароля
string hashPassword(const string& password) {
    string hashedPassword = to_string(hash<string>{}(password));
    return hashedPassword;
}

// Метод обертки для ввода строки с валидацией
string getInputString() {
    string input;
    getline(cin, input);

    while (input.empty()) {
        cout << "Ошибка: пустой ввод. Пожалуйста, повторите ввод: ";
        getline(cin, input);
    }

    replace(input.begin(), input.end(), ' ', '_');

    return input;
}

// Метод обертки для ввода целого числа с валидацией
int getInputInt() {
    while (true) {
        string input;
        getline(cin, input);

        try {
            size_t pos;
            int value = stoi(input, &pos);

            if (pos != input.size()) {
                throw invalid_argument("Недопустимый ввод");
            }

            return value;
        }
        catch (const exception& e) {
            cout << "Недопустимый ввод. Пожалуйста, введите целое число: ";
        }
    }
}

// Метод обертки для ввода вещественного числа с валидацией
double getInputDouble() {
    while (true) {
        string input;
        getline(cin, input);

        try {
            size_t pos;
            double value = stod(input, &pos);

            if (pos != input.size()) {
                throw invalid_argument("Недопустимый ввод");
            }

            return value;
        }
        catch (const exception& e) {
            cout << "Недопустимый ввод. Пожалуйста, введите вещественное число: ";
        }
    }
}

// Запись действия в журнал аудита
void writeAuditLog(const string& username, const string& action) {
    time_t currentTime;
    time(&currentTime);
    char timestamp[26];

    ctime_s(timestamp, sizeof(timestamp), &currentTime);

    timestamp[strlen(timestamp) - 1] = '\0';

    ofstream file("audit_log.txt", ios::app);
    if (file.is_open()) {
        file << timestamp << " - " << username << ": " << action << endl;
        file.close();
    }
    else {
        cerr << "Ошибка открытия файла журнала аудита." << endl;
    }
}

// Просмотр журнала аудита системным администратором
void viewAuditLog() {
    ifstream file("audit_log.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }
    else {
        cerr << "Ошибка открытия файла журнала аудита." << endl;
    }
}

// Запись финансового события в журнал
void writeFinancialEvent(const string& username, const string& event) {
    time_t currentTime;
    time(&currentTime);
    char timestamp[26];

    ctime_s(timestamp, sizeof(timestamp), &currentTime);

    timestamp[strlen(timestamp) - 1] = '\0';

    ofstream file("financial_report.txt", ios::app);
    if (file.is_open()) {
        file << timestamp << " - " << username << ": " << event << endl;
        file.close();
    }
    else {
        cerr << "Ошибка открытия файла финансового отчёта." << endl;
    }
}

// Просмотр финансового отчёта
void viewFinancialReport() {
    ifstream file("financial_report.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }
    else {
        cerr << "Ошибка открытия файла финансового отчёта." << endl;
    }
}