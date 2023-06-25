#pragma once
#include "../Helpers/HelpersHeader.h"
#include "Order.h"

// ������������ �������
static void SerializeBalance(double balance) {
	ofstream file("balance.txt");
	if (file.is_open()) {
		file << balance;
		file.close();
	}
	else {
		cout << "�� ������� ������� ���� ��� ������������ �������." << endl;
	}
}

// �������������� �������
static double DeserializeBalance() {
	double balance = 0.0;
	ifstream file("balance.txt");
	if (file.is_open()) {
		file >> balance;
		file.close();
	}
	else {
		cout << "�� ������� ������� ���� ��� �������������� �������." << endl;
	}
	return balance;
}

// ������������ ������� � ������������ ������
template <class T>
class TextSerializer {
public:
	bool Serialize(const vector<T>& objects, const string& filename) {
		ofstream file(filename);
		if (!file.is_open()) {
			cerr << "�� ������� ������� ����: " << filename << endl;
			return false;
		}

		for (const auto& obj : objects) {
			file << obj << '\n';
		}

		file.close();
		return true;
	}
};

// �������������� ������� � ������������ ������
template <class T>
class TextDeserializer {
public:
	bool Deserialize(vector<T>& objects, const string& filename) {
		ifstream file(filename);
		if (!file.is_open()) {
			cerr << "�� ������� ������� ����: " << filename << endl;
			return false;
		}

		T obj;
		while (file >> obj) {
			objects.push_back(obj);
		}

		file.close();
		return true;
	}
};

// ����� �����
class Guest {
	vector<Product> products;
	vector<MenuItem> menu;
	vector<MenuItem> cart;
	vector<Order> orders;
	Order currentOrder;
	int lastOrderId;
private:

public:
	Guest() {
		showMenu();
	}

	// ����� ��� ����������� ���� ������ ��������
	void showMenu() {
		TextDeserializer<Order> deserializer3;
		deserializer3.Deserialize(orders, "orders.txt");
		TextDeserializer<Product> deserializer;
		deserializer.Deserialize(products, "products.txt");
		TextDeserializer<MenuItem> deserializer2;
		deserializer2.Deserialize(menu, "menu.txt");
		lastOrderId = findLastOrderId();

		int choice;
		do {
			cout << "1. ����������� ����" << endl;
			cout << "2. �������� ����� ���� � �������" << endl;
			cout << "3. ����������� �����" << endl;
			cout << "4. ����������� ������ ������" << endl;
			cout << "5. ����������� ���� �����" << endl;
			cout << "6. ���������� �����" << endl;
			cout << "7. �����" << endl;
			cout << "�������� ��������: ";
			choice = getInputInt();

			switch (choice) {
			case 1:
				showAllMenuItems();
				break;
			case 2:
				addMenuItemToCart();
				break;
			case 3:
				confirmOrder();
				break;
			case 4:
				trackOrderStatus();
				break;
			case 5:
				viewOrder();
				break;
			case 6:
				continueOrder();
				break;
			case 7:
				break;
			default:
				cout << "�������� ����� ��������." << endl;
				break;
			}
		} while (choice != 7);

		TextSerializer<Order> serializer3;
		serializer3.Serialize(orders, "orders.txt");
		TextSerializer<Product> serializer;
		serializer.Serialize(products, "products.txt");
		TextSerializer<MenuItem> serializer2;
		serializer2.Serialize(menu, "menu.txt");
	}

	// �������� ����� ����
	void showAllMenuItems() {
		cout << "������ ���� ���� � ����:" << endl;
		for (const auto& menuItemPair : map<string, MenuItem>()) {
			const MenuItem& menuItem = menuItemPair.second;
			menuItem.PrintMenuItemInfo();
		}
	}

	// ���������� ������ ���� � �������
	void addMenuItemToCart() {
		cout << "������� �������� �����, ������� ������ �������� � �������: ";
		string menuItemName = getInputString();

		auto it = find_if(menu.begin(), menu.end(), [menuItemName](const MenuItem& menuItem) {
			return menuItem.getName() == menuItemName;
			});

		if (it == menu.end()) {
			cout << "����� � ����� ��������� �� �������." << endl;
			return;
		}

		bool isAvailable = true;
		for (const auto& product : it->getComposition()) {
			auto prodIt = find_if(products.begin(), products.end(), [product](const Product& p) {
				return p.getName() == product.getName();
				});

			if (prodIt == products.end() || prodIt->getQuantity() <= 0) {
				isAvailable = false;
				break;
			}
		}

		if (!isAvailable) {
			cout << "������������ ��������� �� ������ ��� ���������� ����� � �������." << endl;
			return;
		}

		cart.push_back(*it);

		for (const auto& product : it->getComposition()) {
			auto prodIt = find_if(products.begin(), products.end(), [product](const Product& p) {
				return p.getName() == product.getName();
				});

			if (prodIt != products.end()) {
				prodIt->setQuantity(prodIt->getQuantity() - 1);
			}
		}

		cout << "����� ������� ��������� � �������!" << endl;
	}

	// ������������� ������
	void confirmOrder() {
		if (cart.empty()) {
			cout << "������� �����. �������� ������ ���� ����� �������������� ������." << endl;
			return;
		}

		currentOrder.setItems(cart);

		double orderTotal = calculateOrderTotal();
		currentOrder.setTotalCost(orderTotal);

		double balance = DeserializeBalance();
		balance += orderTotal;
		SerializeBalance(balance);

		currentOrder.setStatus("������� � ������� �� �����");
		currentOrder.setId(generateId());

		orders.push_back(currentOrder);

		cout << "����� ������� �����������! ��� ����� ������: " << currentOrder.getId() << endl;

		cart.clear();
		currentOrder = Order();
	}

	// �������� ������ ������
	void viewOrder() {
		cout << "��� �����:" << endl;
		currentOrder.PrintOrderInfo();
	}

	// ������������ ������� ������
	void trackOrderStatus() {
		if (currentOrder.getId() == 0) {
			cout << "� ��� ��� ��������� ������." << endl;
		}
		else {
			cout << "������ ������ ������: " << currentOrder.getStatus() << endl;
		}
	}

	// ����������� ������
	void continueOrder() {
		if (currentOrder.getId() != 0) {
			cout << "� ��� ��� ���� �������� �����. ��������� ��� ����� ������������." << endl;
			return;
		}

		if (orders.empty()) {
			cout << "������ ������� ����. ��� ��������� ������� ��� �����������." << endl;
			return;
		}

		currentOrder = orders.back();

		cout << "����� ������� ��������! �� ������ ���������� ��� ��� �������� ��� ����������." << endl;
	}

	// ����� ��� ��������� ����������� ��������� �������������� ������ �� ������ ������� � ���� ������
	int generateId() {
		time_t currentTime = time(0);
		tm localTime;
		localtime_s(&localTime, &currentTime);

		int year = localTime.tm_year + 1900;
		int month = localTime.tm_mon + 1;
		int day = localTime.tm_mday;
		int hour = localTime.tm_hour;
		int minute = localTime.tm_min;
		int second = localTime.tm_sec;

		int id = (year * 10000000000) + (month * 100000000) + (day * 1000000) + (hour * 10000) + (minute * 100) + second;

		return id;
	}

	// ����� ��� �������� ��������� ���� ������� ���� � ������
	double calculateOrderTotal() {
		double total = 0.0;
		for (const auto& item : cart) {
			total += item.getPrice();
			writeFinancialEvent("�����", "����� ����� " + item.getName() + ": " + to_string(item.getPrice()));
		}
		return total;
	}

	// ����� ��� ������ ���������� ��������������� �������������� ������
	int findLastOrderId() {
		int lastId = 0;
		for (const auto& order : orders) {
			if (order.getId() > lastId) {
				lastId = order.getId();
			}
		}
		return lastId;
	}
};

// ����� ���������
class Employee {
protected:
	string username;
	string password;
	string role;
	string firstName;
	string middleName;
	string lastName;

	// ��������������� ���������� ��� ������������ � ��������������
	friend ostream& operator<<(ostream& os, const Employee& employee) {
		os << employee.username << ' ' << employee.password << ' ' << employee.role << ' ' << employee.firstName << ' ' << employee.middleName << ' ' << employee.lastName;
		return os;
	}

	friend istream& operator>>(istream& is, Employee& employee) {
		is >> employee.username >> employee.password >> employee.role >> employee.firstName >> employee.middleName >> employee.lastName;
		return is;
	}

public:
	// ����������� � ������ ������
	Employee() {}
	Employee(const string& username) : username(username) {}
	Employee(const string& username, const string& password, const string& role, const string& firstName, const string& middleName, const string& lastName)
		: username(username), password(password), role(role), firstName(firstName), middleName(middleName), lastName(lastName) {}

	// ����� ��� ������ ���������� � ����������
	void PrintEmployeeInfo() const {
		cout << "\n�����: " << username << endl;
		cout << "����: " << role << endl;
		cout << "���: " << firstName << endl;
		cout << "�������: " << middleName << endl;
		cout << "��������: " << lastName << endl;
		cout << "=============================" << endl;
	}

	// ������� � �������
	string getUsername() const {
		return username;
	}

	string getPasswordHash() const {
		return password;
	}

	string getRole() const {
		return role;
	}

	void setUsername(const string& newUsername) {
		username = newUsername;
	}

	void setPassword(const string& newPassword) {
		password = newPassword;
	}

	void setRole(const string& newRole) {
		role = newRole;
	}

	void setFirstName(const string& newFirstName) {
		firstName = newFirstName;
	}

	void setMiddleName(const string& newMiddleName) {
		middleName = newMiddleName;
	}

	void setLastName(const string& newLastName) {
		lastName = newLastName;
	}
};

// ����� ��������� �������������
class Administrator : public Employee {
private:
	vector<Employee> employees;
	vector<Product> products;
	vector<MenuItem> menu;
public:
	// ����������� ������
	Administrator(const string& username) : Employee(username) {
		showMenu();
	}

	// ����� ��� ����������� ���� ������ ��������
	void showMenu() {
		int choice;
		do
		{
			cout << "1. ������������� ����" << endl;
			cout << "2. ������������� ������ ���������" << endl;
			cout << "3. ������������� ������� ������ �����������" << endl;
			cout << "4. �������� ������� ������" << endl;
			cout << "5. �����" << endl;
			cout << "�������� ��������: " << endl;
			choice = getInputInt();

			switch (choice) {
			case 1:
				editMenu();
				break;
			case 2:
				editProductList();
				break;
			case 3:
				editEmployeeAccounts();
				break;
			case 4:
				showAuditLog();
				break;
			case 5:
				break;
			default:
				cout << "�������� ����� ��������." << endl;
				break;
			}
		} while (choice != 5);
	}

	// �������������� ����
	void editMenu() {
		TextDeserializer<MenuItem> deserializer;
		deserializer.Deserialize(menu, "menu.txt");

		int choice;
		do {
			cout << "1. �������� ����� � ����" << endl;
			cout << "2. ������� ����� �� ����" << endl;
			cout << "3. ����������� �� ����" << endl;
			cout << "4. �����" << endl;
			cout << "�������� ��������: " << endl;
			choice = getInputInt();

			switch (choice) {
			case 1:
				addMenuItem();
				break;
			case 2:
				deleteMenuItem();
				break;
			case 3:
				showAllMenuItems();
				break;
			case 4:
				break;
			default:
				cout << "�������� ����� ��������." << endl;
				break;
			}
		} while (choice != 4);

		TextSerializer<MenuItem> serializer;
		serializer.Serialize(menu, "menu.txt");
	}

	// ���������� ������ ������ ����
	void addMenuItem() {
		TextDeserializer<Product> deserializer;
		deserializer.Deserialize(products, "products.txt");

		cout << "������� �������� �����: ";
		string name = getInputString();
		cout << "������� ��� �����: ";
		int weight = getInputInt();
		cout << "������� �������� �����: ";
		string description = getInputString();
		cout << "������� ����� ������������� �����: ";
		int preparationTime = getInputInt();
		cout << "������� ��������� �����: ";
		double cost = getInputDouble();

		MenuItem newMenuItem(name, weight, {}, description, preparationTime, cost);

		int choice;
		do {
			cout << "�������� ������� ��� ���������� � ������ �����:" << endl;

			showAllProducts();

			int productId = getInputInt();
			auto it = find_if(products.begin(), products.end(), [productId](const Product& product) {
				return product.getId() == productId;
				});

			if (it == products.end()) {
				cout << "������� � ����� ID �� ������." << endl;
			}
			else {
				newMenuItem.addProduct(*it);
				cout << "������� ������� �������� � ������ �����!" << endl;
			}

			cout << "������ �������� ��� �������? (1 - ��, 0 - ���): ";
			choice = getInputInt();
		} while (choice != 0);

		menu.push_back(newMenuItem);

		writeAuditLog(username, "������� ����� � ����: " + name);
		cout << "����� ���� ������� ��������!" << endl;
	}

	// �������� ������ ����
	void deleteMenuItem() {
		cout << "������� �������� ����� � ���� ��� ��������: ";
		string menuItemName = getInputString();

		auto it = find_if(menu.begin(), menu.end(), [menuItemName](const MenuItem& menuItem) {
			return menuItem.getName() == menuItemName;
			});

		if (it == menu.end()) {
			cout << "����� � ���� � ����� ��������� �� ������." << endl;
			return;
		}

		menu.erase(it);

		writeAuditLog(username, "������ �� ����: " + it->getName());
		cout << "����� ���� ������� ������!" << endl;
	}

	// �������� ����� ����
	void showAllMenuItems() {
		cout << "������ ���� ���� � ����:" << endl;
		for (const auto& menuItem : menu) {
			menuItem.PrintMenuItemInfo();
		}
	}

	// �������������� ������ ���������
	void editProductList() {
		TextDeserializer<Product> deserializer;
		deserializer.Deserialize(products, "products.txt");

		int choice;
		do
		{
			cout << "1. �������� �������" << endl;
			cout << "2. ������� �������" << endl;
			cout << "3. ����������� ��� ��������" << endl;
			cout << "4. �����" << endl;
			cout << "�������� ��������: " << endl;
			choice = getInputInt();

			switch (choice) {
			case 1:
				addProduct();
				break;
			case 2:
				deleteProduct();
				break;
			case 3:
				showAllProducts();
				break;
			case 4:
				break;
			default:
				cout << "�������� ����� ��������." << endl;
				break;
			}
		} while (choice != 4);

		TextSerializer<Product> serializer;
		serializer.Serialize(products, "products.txt");
	}

	// ���������� ������ ��������
	void addProduct() {
		cout << "������� ������������ ��������: ";
		string name = getInputString();
		cout << "������� ���� ��������: ";
		double price = getInputDouble();
		cout << "������� ���������� ��������: ";
		int quantity = getInputInt();

		int newId = getNextProductId();
		Product newProduct(newId, name, price, quantity);
		products.push_back(newProduct);

		writeAuditLog(username, "������� � ������ ���������: " + name);
		cout << "������� ������� ��������!" << endl;
	}

	// ��������� ���������� ���������� ID ��� ��������
	int getNextProductId() {
		if (products.empty()) {
			return 1;
		}
		else {
			int maxId = 0;
			for (const auto& product : products) {
				if (product.getId() > maxId) {
					maxId = product.getId();
				}
			}
			return maxId + 1;
		}
	}

	// �������� ��������
	void deleteProduct() {
		cout << "������� ID �������� ��� ��������: ";
		int productId = getInputInt();

		auto it = find_if(products.begin(), products.end(), [productId](const Product& product) {
			return product.getId() == productId;
			});

		if (it == products.end()) {
			cout << "������� � ����� ID �� ������." << endl;
			return;
		}

		products.erase(it);

		writeAuditLog(username, "������ �� ������ ���������: " + it->getName());
		cout << "������� ������� ������!" << endl;
	}

	// �������� ���� ���������
	void showAllProducts() {
		cout << "������ ���� ���������:" << endl;
		for (const auto& product : products) {
			product.PrintProductInfo();
		}
	}

	// �������������� ������� ������� �����������
	void editEmployeeAccounts() {
		TextDeserializer<Employee> deserializer;
		deserializer.Deserialize(employees, "employees.txt");

		int choice;
		do
		{
			cout << "1. ������� ������� ������" << endl;
			cout << "2. �������� ������� ������" << endl;
			cout << "3. ������� ������� ������" << endl;
			cout << "4. ����������� ������� ������" << endl;
			cout << "5. ����������� ������� ������" << endl;
			cout << "6. �����" << endl;
			cout << "�������� ��������: " << endl;
			choice = getInputInt();

			switch (choice) {
			case 1:
				createEmployeeAccount();
				break;
			case 2:
				editEmployeeAccount();
				break;
			case 3:
				deleteEmployeeAccount();
				break;
			case 4:
				showEmployeeInfo();
				break;
			case 5:
				showAllEmployees();
				break;
			case 6:
				break;
			default:
				cout << "�������� ����� ��������." << endl;
				break;
			}
		} while (choice != 6);

		TextSerializer<Employee> serializer;
		serializer.Serialize(employees, "employees.txt");
	}

	// �������� ����� ������� ������ ����������
	void createEmployeeAccount() {
		cout << "������� ��� ������������: ";
		string username = getInputString();

		auto it = find_if(employees.begin(), employees.end(), [username](const Employee& employee) {
			return employee.getUsername() == username;
			});

		if (it != employees.end()) {
			cout << "������������ � ����� ������ ��� ����������." << endl;
			return;
		}

		cout << "������� ������: ";
		string password = getInputString();
		cout << "������� ����: ";
		string role = getInputString();
		cout << "������� �������: ";
		string middleName = getInputString();
		cout << "������� ���: ";
		string firstName = getInputString();
		cout << "������� ��������: ";
		string lastName = getInputString();

		Employee newEmployee(username, password, role, firstName, middleName, lastName);
		employees.push_back(newEmployee);

		writeAuditLog(username, "������ ����������: " + username);
		cout << "������� ������ ���������� ������� �������!" << endl;
	}

	// ��������� ������� ������ ����������
	void editEmployeeAccount() {
		cout << "������� ��� ������������: ";
		string username = getInputString();

		auto it = find_if(employees.begin(), employees.end(), [username](const Employee& employee) {
			return employee.getUsername() == username;
			});

		if (it == employees.end()) {
			cout << "��������� � ����� ������ ������������ �� ������." << endl;
			return;
		}

		Employee& employee = *it;
		int choice;
		do {

			cout << "�������� ���� ��� ���������:" << endl;
			cout << "1. ��� ������������" << endl;
			cout << "2. ������" << endl;
			cout << "3. ����" << endl;
			cout << "4. �������" << endl;
			cout << "5. ���" << endl;
			cout << "6. ��������" << endl;
			cout << "7. �����" << endl;
			cout << "�������� ��������: ";
			choice = getInputInt();

			switch (choice) {
			case 1:
				cout << "������� ����� ��� ������������: ";
				employee.setUsername(getInputString());
				break;
			case 2:
				cout << "������� ����� ������: ";
				employee.setPassword(hashPassword(getInputString()));
				break;
			case 3:
				cout << "������� ����� ����: ";
				employee.setRole(getInputString());
				break;
			case 4:
				cout << "������� ����� �������: ";
				employee.setLastName(getInputString());
				break;
			case 5:
				cout << "������� ����� ���: ";
				employee.setFirstName(getInputString());
				break;
			case 6:
				cout << "������� ����� ��������: ";
				employee.setMiddleName(getInputString());
				break;
			case 7:
				break;
			default:
				cout << "�������� ����� ��������." << endl;
				break;
			}
		} while (choice != 7);

		writeAuditLog(username, "������� ����������: " + username);
		cout << "������� ������ ���������� ������� ��������!" << endl;
	}

	// �������� ���������� � ����������
	void showEmployeeInfo() {
		cout << "������� ��� ������������: ";
		string username = getInputString();

		auto it = find_if(employees.begin(), employees.end(), [username](const Employee& employee) {
			return employee.getUsername() == username;
			});

		if (it == employees.end()) {
			cout << "��������� � ����� ������ ������������ �� ������." << endl;
			return;
		}

		Employee& employee = *it;
		employee.PrintEmployeeInfo();
	}

	// �������� ������� ������ ����������
	void deleteEmployeeAccount() {
		cout << "������� ��� ������������: ";
		string username = getInputString();

		auto it = find_if(employees.begin(), employees.end(), [username](const Employee& employee) {
			return employee.getUsername() == username;
			});

		if (it == employees.end()) {
			cout << "��������� � ����� ������ ������������ �� ������." << endl;
			return;
		}

		employees.erase(it);

		writeAuditLog(username, "������ ����������: " + it->getUsername());
		cout << "������� ������ ���������� ������� �������!" << endl;
	}

	// �������� ���� �����������
	void showAllEmployees() {
		cout << "������ ���� �����������:" << endl;
		for (const auto& employee : employees) {
			employee.PrintEmployeeInfo();
		}
	}

	// �������� ������� ������
	void showAuditLog() {
		viewAuditLog();
	}
};

// ����� ���������
class Stockkeeper : public Employee {
private:
	vector<Product> products;
	vector<ProductRequest> requests;

public:
	Stockkeeper(const string& username) : Employee(username) {
		showMenu();
	}

	// ����� ��� ����������� ���� ������ ��������
	void showMenu() {
		TextDeserializer<Product> deserializer;
		deserializer.Deserialize(products, "products.txt");
		TextDeserializer<ProductRequest> deserializer2;
		deserializer2.Deserialize(requests, "productRequests.txt");

		int choice;
		do
		{
			cout << "1. �������� ������" << endl;
			cout << "2. �������� ������ �� �������" << endl;
			cout << "3. �����" << endl;
			cout << "�������� ��������: " << endl;
			choice = getInputInt();

			switch (choice) {
			case 1:
				showAllProducts();
				break;
			case 2:
				createProductRequest();
				break;
			case 3:
				break;
			default:
				cout << "�������� ����� ��������." << endl;
				break;
			}
		} while (choice != 3);

		TextSerializer<ProductRequest> serializer2;
		serializer2.Serialize(requests, "productRequests.txt");
	}

	// �������� ���� ���������
	void showAllProducts() {
		cout << "������ ���� ���������:" << endl;
		for (const auto& product : products) {
			product.PrintProductInfo();
		}
	}

	// �������� ������ �� �������
	void createProductRequest() {
		cout << "�������� ������ �� �������" << endl;
		cout << "������� ID ��������: ";
		int productId = getInputInt();

		auto it = find_if(products.begin(), products.end(), [productId](const Product& product) {
			return product.getId() == productId;
			});

		if (it == products.end()) {
			cout << "������� � ����� ID �� ������." << endl;
			return;
		}

		Product product = *it;

		cout << "������� ������������� ����������: ";
		int requestedQuantity = getInputInt();

		double totalCost = product.getPrice() * requestedQuantity;
		if (totalCost > DeserializeBalance()) {
			cout << "����� ������ ��������� ������ ��������. ������ �� ����� ���� �������." << endl;
			return;
		}

		int newId = getNextRequestId();
		ProductRequest request(newId, product, requestedQuantity);
		request.setStatus("�������");
		requests.push_back(request);

		writeAuditLog(username, "������ ������ �� �������: " + product.getName());
		cout << "������ �� ������� ������� �������!" << endl;
	}

	// ��������� ���������� ���������� ID ��� ������ �� �������
	int getNextRequestId() {
		if (requests.empty()) {
			return 1;
		}
		else {
			int maxId = 0;
			for (const auto& request : requests) {
				if (request.getId() > maxId) {
					maxId = request.getId();
				}
			}
			return maxId + 1;
		}
	}
};

// ����� ���������
class Supplier : public Employee {
	vector<Product> products;
	vector<ProductRequest> requests;
public:
	Supplier(const string& username) : Employee(username) {
		showMenu();
	}

	// ����� ��� ����������� ���� ������ ��������
	void showMenu() {
		TextDeserializer<Product> deserializer;
		deserializer.Deserialize(products, "products.txt");
		TextDeserializer<ProductRequest> deserializer2;
		deserializer2.Deserialize(requests, "productRequests.txt");

		int choice;
		do {
			cout << "������ ���������: " << DeserializeBalance() << endl;
			cout << "1. �������� ������ �� ��������" << endl;
			cout << "2. ��������� �������� �� ������" << endl;
			cout << "3. �����" << endl;
			cout << "�������� ��������: " << endl;
			choice = getInputInt();

			switch (choice) {
			case 1:
				showAllRequests();
				break;
			case 2:
				sendProducts();
				break;
			case 3:
				break;
			default:
				cout << "�������� ����� ��������." << endl;
				break;
			}
		} while (choice != 3);

		TextSerializer<Product> serializer;
		serializer.Serialize(products, "products.txt");
		TextSerializer<ProductRequest> serializer2;
		serializer2.Serialize(requests, "productRequests.txt");
	}

	// �������� ���� ������ �� ��������
	void showAllRequests() {
		cout << "������ ���� ������ �� ��������:" << endl;
		for (const auto& request : requests) {
			if (request.getStatus() == "�������") {
				request.PrintRequestInfo();
			}
		}
	}

	void sendProducts() {
		cout << "�������� ��������� �� ������" << endl;
		cout << "������� ID ������: ";
		int requestId = getInputInt();

		auto it = find_if(requests.begin(), requests.end(), [requestId](const ProductRequest& request) {
			return request.getId() == requestId && request.getStatus() == "�������";
			});

		if (it == requests.end()) {
			cout << "������ � ����� ID �� ������� ��� ��� ���������." << endl;
			return;
		}

		ProductRequest& request = *it;

		auto it2 = find_if(products.begin(), products.end(), [&request](const Product& product) {
			return product == request.getProduct();
			});

		if (it2 == products.end()) {
			cout << "������� � ����� ID �� ������." << endl;
			return;
		}

		Product& product = *it2;

		double balance = DeserializeBalance();
		double totalCost = product.getPrice() * request.getRequestedQuantity();
		if (totalCost > balance) {
			cout << "����� ������ ��������� ������ ��������. ������ �� ����� ���� ���������." << endl;
			return;
		}

		product.setQuantity(product.getQuantity() + request.getRequestedQuantity());
		request.setStatus("���������");

		SerializeBalance(balance - totalCost);
		writeAuditLog(username, "�������� ������ �� �������: " + product.getName());
		writeFinancialEvent(username, "�������� �� ������: " + to_string(totalCost));
		cout << "������ �� ������� ������� ����������!" << endl;
	}
};

// ����� ���������
class Accountant : public Employee {
private:
	vector<ProductRequest> productRequests;

public:
	Accountant(const string& username) : Employee(username) {
		showMenu();
	}

	// ����� ��� ����������� ���� ������ ��������
	void showMenu() {
		TextDeserializer<ProductRequest> deserializer;
		deserializer.Deserialize(productRequests, "productRequests.txt");

		int choice;
		do {
			cout << "1. �������� ������ �� ��������" << endl;
			cout << "2. �������� ���������� ����������" << endl;
			cout << "3. �����" << endl;
			cout << "�������� ��������: " << endl;
			choice = getInputInt();

			switch (choice) {
			case 1:
				showAllProductRequests();
				break;
			case 2:
				viewFinancialReport();
				break;
			case 3:
				break;
			default:
				cout << "�������� ����� ��������." << endl;
				break;
			}
		} while (choice != 3);
	}

	// �������� ���� ������ �� ��������
	void showAllProductRequests() {
		cout << "������ ���� ������ �� ��������:" << endl;
		for (const auto& request : productRequests) {
			request.PrintRequestInfo();
		}
	}

	// ����� ��� ��������� ���������� ����������
	void viewFinancialReport() {
		double balance = DeserializeBalance();
		double totalSales = calculateTotalSales();
		double totalExpenses = calculateTotalExpenses();

		cout << "���������� ����������:" << endl;
		cout << "������ �������� �������: " << balance << endl;
		cout << "����� ����� ������: " << totalSales << endl;
		cout << "����� �������� �� ���������� ������: " << totalExpenses << endl;
	}

	// ������� ��� ������� ����� ����� ������
	double calculateTotalSales() {
		double totalSales = 0.0;
		ifstream file("financial_report.txt");
		if (file.is_open()) {
			string line;
			while (getline(file, line)) {
				size_t pos = line.find_last_of(':');
				string amountStr = line.substr(pos + 2);
				double amount = stod(amountStr);
				if (line.find("����� �����") != string::npos) {
					totalSales += amount;
				}
			}
			file.close();
		}
		else {
			cerr << "������ �������� ����� ����������� ������." << endl;
		}
		return totalSales;
	}

	// ������� ��� ������� �������� �� ���������� ������
	double calculateTotalExpenses() {
		double totalExpenses = 0.0;
		ifstream file("financial_report.txt");
		if (file.is_open()) {
			string line;
			while (getline(file, line)) {
				size_t pos = line.find_last_of(':');
				string amountStr = line.substr(pos + 2);
				double amount = stod(amountStr);
				if (line.find("�������� �� ������") != string::npos) {
					totalExpenses += amount;
				}
			}
			file.close();
		}
		else {
			cerr << "������ �������� ����� ����������� ������." << endl;
		}
		return totalExpenses;
	}
};

// ����� �����
class Chef : public Employee {
private:
	vector<Order> orders;

public:
	Chef(const string& username) : Employee(username) {
		showMenu();
	}

	// ����� ��� ����������� ���� ������ ��������
	void showMenu() {
		TextDeserializer<Order> deserializer;
		deserializer.Deserialize(orders, "orders.txt");

		int choice;
		do {
			cout << "1. ����������� ������" << endl;
			cout << "2. ������ ������������� ������" << endl;
			cout << "3. �����" << endl;
			cout << "�������� ��������: ";
			choice = getInputInt();

			switch (choice) {
			case 1:
				viewOrders();
				break;
			case 2:
				startOrderPreparation();
				break;
			case 3:
				break;
			default:
				cout << "�������� ����� ��������." << endl;
				break;
			}
		} while (choice != 3);

		TextSerializer<Order> serializer;
		serializer.Serialize(orders, "orders.txt");
	}

	// ����� ��� ��������� ���� �������
	void viewOrders() {
		if (orders.empty()) {
			cout << "������ ������� ����." << endl;
			return;
		}

		cout << "������ �������:" << endl;
		for (const auto& order : orders) {
			if (order.getStatus() == "������� � ������� �� �����") {
				order.PrintOrderInfo();
			}
		}
	}

	// ����� ��� ������ ������������� ������
	void startOrderPreparation() {
		if (orders.empty()) {
			cout << "������ ������� ����." << endl;
			return;
		}

		cout << "������� ����� ������, ������� �� ������ �����������: ";
		int orderId = getInputInt();

		auto it = find_if(orders.begin(), orders.end(), [orderId](const Order& order) {
			return order.getId() == orderId;
			});

		if (it == orders.end()) {
			cout << "����� � ����� ������� �� ������." << endl;
			return;
		}

		if (it->getStatus() != "������� � ������� �� �����") {
			cout << "����� �� ����� ���� �����������, ��� ��� ��� ������ �� �������� '������� � ������� �� �����'." << endl;
			return;
		}

		it->setStatus("� �������� �������������");
		cout << "����� " << orderId << " ����� �������������." << endl;

		thread preparationThread(&Chef::prepareOrder, this, ref(*it));
		preparationThread.join();
	}

	// ����� ��� ������������� ������
	void prepareOrder(Order& order) {
		vector<int> productNumbers = order.getProductNumbers();
		bool allProductsPrepared = true;

		cout << "������������� ��������� ��� ������ " << order.getId() << ":" << endl;

		for (int productNumber : productNumbers) {
			cout << "������� ����� �������� ��� �������������: ";
			int product = getInputInt();

			auto it = find(productNumbers.begin(), productNumbers.end(), product);
			if (it != productNumbers.end()) {
				cout << "������� " << product << " �����������." << endl;
			}
			else {
				cout << "������� � ������� " << product << " �� ������ � ������." << endl;
				allProductsPrepared = false;
			}
		}

		if (allProductsPrepared) {
			order.setStatus("������� ��������� �� ������");
			writeAuditLog(username, "���������� �����: " + to_string(order.getId()));
			cout << "����� " << order.getId() << " ����� � ������." << endl;
		}
		else {
			cout << "����� " << order.getId() << " �� ����� � ������." << endl;
		}
	}
};

// ����� ��������
class Waiter : public Employee {
private:
	vector<Order> orders;

public:
	Waiter(const string& username) : Employee(username) {
		showMenu();
	}

	// ����� ��� ����������� ���� ������ ��������
	void showMenu() {
		TextDeserializer<Order> deserializer;
		deserializer.Deserialize(orders, "orders.txt");

		int choice;
		do {
			cout << "1. ����������� ������" << endl;
			cout << "2. ������ �����" << endl;
			cout << "3. �����" << endl;
			cout << "�������� ��������: ";
			choice = getInputInt();

			switch (choice) {
			case 1:
				viewOrders();
				break;
			case 2:
				serveOrder();
				break;
			case 3:
				break;
			default:
				cout << "�������� ����� ��������." << endl;
				break;
			}
		} while (choice != 3);

		TextSerializer<Order> serializer;
		serializer.Serialize(orders, "orders.txt");
	}

	// ����� ��� ��������� ���� �������
	void viewOrders() {
		if (orders.empty()) {
			cout << "������ ������� ����." << endl;
			return;
		}

		cout << "������ �������, ��������� ��� ������:" << endl;
		for (const auto& order : orders) {
			if (order.getStatus() == "������� ��������� �� ������") {
				order.PrintOrderInfo();
			}
		}
	}

	// ����� ��� ������ ������
	void serveOrder() {
		if (orders.empty()) {
			cout << "������ ������� ����." << endl;
			return;
		}

		cout << "������� ����� ������, ������� �� ������ ������: ";
		int orderId = getInputInt();

		auto it = find_if(orders.begin(), orders.end(), [orderId](const Order& order) {
			return order.getId() == orderId && order.getStatus() == "������� ��������� �� ������";
			});

		if (it == orders.end()) {
			cout << "����� � ����� ������� �� ������ ��� �� �������� ��� ������." << endl;
			return;
		}

		it->setStatus("��������");

		writeAuditLog(username, "����� �����: " + to_string(it->getId()));
		cout << "����� " << orderId << " �����." << endl;
	}
};