#pragma once
#include "Entity.h"

// ����� �����������
class Authorization {
private:
	vector<Employee> employees;

public:
	Authorization() {
		TextDeserializer<Employee> deserializer;
		deserializer.Deserialize(employees, "employees.txt");
	}

	// ����� ��� ����� �����������
	void login() {
		string username, password;
		cout << "������� ��� ������������: ";
		username = getInputString();
		cout << "������� ������: ";
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

				writeAuditLog(it->getUsername(), "����� � ������� ��� " + role);

				cout << "�� ������� ����� ��� " << role << endl;
				if (role == "�����") {
					auto admin = make_unique<Administrator>(username);
				}
				else if (role == "���������") {
					auto stockkeeper = make_unique<Stockkeeper>(username);
				}
				else if (role == "���������") {
					auto supplier = make_unique<Supplier>(username);
				}
				else if (role == "���������") {
					auto accountant = make_unique<Accountant>(username);
				}
				else if (role == "�����") {
					auto chef = make_unique<Chef>(username);
				}
				else if (role == "��������") {
					auto waiter = make_unique<Waiter>(username);
				}
				else {
					cout << "������: ����������� ����." << endl;
				}
				return;
			}
		}

		cout << "������ �����. �������� ��� ������������ ��� ������." << endl;
	}

	// ����� ��� ����� �����
	void loginAsGuest() {
		cout << "����� ���������� � ��������!" << endl;
		writeAuditLog("�����", "����� � �������");
		Guest guest;
	}

	// ������ ��� ����������� ����������
	void registerEmployee() {
		string username, password, firstname, middlename, lastname;
		string role = "-";

		cout << "������� ��� ������������: ";
		username = getInputString();

		for (const Employee& emp : employees) {
			if (emp.getUsername() == username) {
				cout << "������������ � ����� ������ ��� ����������." << endl;
				return;
			}
		}

		cout << "������� ������: ";
		password = hashPassword(getInputString());
		cout << "������� ���: ";
		firstname = getInputString();
		cout << "������� �������: ";
		middlename = getInputString();
		cout << "������� ��������: ";
		lastname = getInputString();


		Employee employee(username, password, role, firstname, middlename, lastname);
		employees.push_back(employee);

		TextSerializer<Employee> serializer;
		serializer.Serialize(employees, "employees.txt");

		writeAuditLog(username, "��������������� � �������");
		cout << "������� ������� ���������������!" << endl;
	}
};