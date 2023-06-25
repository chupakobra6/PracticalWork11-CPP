#pragma once
#include "Entity.h"

// Класс авторизации
class Authorization {
private:
	vector<Employee> employees;

public:
	Authorization() {
		TextDeserializer<Employee> deserializer;
		deserializer.Deserialize(employees, "employees.txt");
	}

	// Метод для входа сотрудников
	void login() {
		string username, password;
		cout << "Введите имя пользователя: ";
		username = getInputString();
		cout << "Введите пароль: ";
		password = getInputString();

		if (find_if(employees.begin(), employees.end(), [username, password](const Employee& employee) {
			string storedPasswordHash = hashPassword(password);
			return (employee.getUsername() == username) && (employee.getPasswordHash() == storedPasswordHash);
			}) != employees.end()) {
			auto it = find_if(employees.begin(), employees.end(), [username](const Employee& employee) {
				return employee.getUsername() == username;
				});

			if (it != employees.end()) {
				string role = it->getRole();

				writeAuditLog(it->getUsername(), "вошёл в систему как " + role);

				cout << "Вы успешно вошли как " << role << endl;
				if (role == "админ") {
					auto admin = make_unique<Administrator>(username);
				}
				else if (role == "кладовщик") {
					auto stockkeeper = make_unique<Stockkeeper>(username);
				}
				else if (role == "поставщик") {
					auto supplier = make_unique<Supplier>(username);
				}
				else if (role == "бухгалтер") {
					auto accountant = make_unique<Accountant>(username);
				}
				else if (role == "повар") {
					auto chef = make_unique<Chef>(username);
				}
				else if (role == "официант") {
					auto waiter = make_unique<Waiter>(username);
				}
				else {
					cout << "Ошибка: неизвестная роль." << endl;
				}
				return;
			}
		}

		cout << "Ошибка входа. Неверное имя пользователя или пароль." << endl;
	}

	// Метод для входа гостя
	void loginAsGuest() {
		cout << "Добро пожаловать в ресторан!" << endl;
		writeAuditLog("гость", "вошёл в систему");
		Guest guest;
	}

	// Методя для регистрации сотрудника
	void registerEmployee() {
		string username, password, firstname, middlename, lastname;
		string role = "-";

		cout << "Введите имя пользователя: ";
		username = getInputString();

		for (const Employee& emp : employees) {
			if (emp.getUsername() == username) {
				cout << "Пользователь с таким именем уже существует." << endl;
				return;
			}
		}

		cout << "Введите пароль: ";
		password = hashPassword(getInputString());
		cout << "Введите имя: ";
		firstname = getInputString();
		cout << "Введите фамилию: ";
		middlename = getInputString();
		cout << "Введите отчество: ";
		lastname = getInputString();


		Employee employee(username, password, role, firstname, middlename, lastname);
		employees.push_back(employee);

		TextSerializer<Employee> serializer;
		serializer.Serialize(employees, "employees.txt");

		writeAuditLog(username, "зарегистрирован в системе");
		cout << "Аккаунт успешно зарегистрирован!" << endl;
	}
};