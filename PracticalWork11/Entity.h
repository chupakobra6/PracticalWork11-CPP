#pragma once
#include "../Helpers/HelpersHeader.h"
#include "Order.h"

// Сериализация баланса
static void SerializeBalance(double balance) {
	ofstream file("balance.txt");
	if (file.is_open()) {
		file << balance;
		file.close();
	}
	else {
		cout << "Не удалось открыть файл для сериализации баланса." << endl;
	}
}

// Десериализация баланса
static double DeserializeBalance() {
	double balance = 0.0;
	ifstream file("balance.txt");
	if (file.is_open()) {
		file >> balance;
		file.close();
	}
	else {
		cout << "Не удалось открыть файл для десериализации баланса." << endl;
	}
	return balance;
}

// Сериализация вектора с экземплярами класса
template <class T>
class TextSerializer {
public:
	bool Serialize(const vector<T>& objects, const string& filename) {
		ofstream file(filename);
		if (!file.is_open()) {
			cerr << "Не удалось открыть файл: " << filename << endl;
			return false;
		}

		for (const auto& obj : objects) {
			file << obj << '\n';
		}

		file.close();
		return true;
	}
};

// Десериализация вектора с экземплярами класса
template <class T>
class TextDeserializer {
public:
	bool Deserialize(vector<T>& objects, const string& filename) {
		ifstream file(filename);
		if (!file.is_open()) {
			cerr << "Не удалось открыть файл: " << filename << endl;
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

// Класс Гость
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

	// Метод для отображения меню выбора действия
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
			cout << "1. Просмотреть меню" << endl;
			cout << "2. Добавить пункт меню в корзину" << endl;
			cout << "3. Подтвердить заказ" << endl;
			cout << "4. Отслеживать статус заказа" << endl;
			cout << "5. Просмотреть свой заказ" << endl;
			cout << "6. Продолжить заказ" << endl;
			cout << "7. Выйти" << endl;
			cout << "Выберите действие: ";
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
				cout << "Неверный выбор действия." << endl;
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

	// Просмотр всего меню
	void showAllMenuItems() {
		cout << "Список всех блюд в меню:" << endl;
		for (const auto& menuItemPair : map<string, MenuItem>()) {
			const MenuItem& menuItem = menuItemPair.second;
			menuItem.PrintMenuItemInfo();
		}
	}

	// Добавление пункта меню в корзину
	void addMenuItemToCart() {
		cout << "Введите название блюда, которое хотите добавить в корзину: ";
		string menuItemName = getInputString();

		auto it = find_if(menu.begin(), menu.end(), [menuItemName](const MenuItem& menuItem) {
			return menuItem.getName() == menuItemName;
			});

		if (it == menu.end()) {
			cout << "Блюдо с таким названием не найдено." << endl;
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
			cout << "Недостаточно продуктов на складе для добавления блюда в корзину." << endl;
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

		cout << "Блюдо успешно добавлено в корзину!" << endl;
	}

	// Подтверждение заказа
	void confirmOrder() {
		if (cart.empty()) {
			cout << "Корзина пуста. Добавьте пункты меню перед подтверждением заказа." << endl;
			return;
		}

		currentOrder.setItems(cart);

		double orderTotal = calculateOrderTotal();
		currentOrder.setTotalCost(orderTotal);

		double balance = DeserializeBalance();
		balance += orderTotal;
		SerializeBalance(balance);

		currentOrder.setStatus("Оплачен и передан на кухню");
		currentOrder.setId(generateId());

		orders.push_back(currentOrder);

		cout << "Заказ успешно подтвержден! Ваш номер заказа: " << currentOrder.getId() << endl;

		cart.clear();
		currentOrder = Order();
	}

	// Просмотр своего заказа
	void viewOrder() {
		cout << "Ваш заказ:" << endl;
		currentOrder.PrintOrderInfo();
	}

	// Отслеживание статуса заказа
	void trackOrderStatus() {
		if (currentOrder.getId() == 0) {
			cout << "У вас нет активного заказа." << endl;
		}
		else {
			cout << "Статус вашего заказа: " << currentOrder.getStatus() << endl;
		}
	}

	// Продолжение заказа
	void continueOrder() {
		if (currentOrder.getId() != 0) {
			cout << "У вас уже есть активный заказ. Завершите его перед продолжением." << endl;
			return;
		}

		if (orders.empty()) {
			cout << "Список заказов пуст. Нет доступных заказов для продолжения." << endl;
			return;
		}

		currentOrder = orders.back();

		cout << "Заказ успешно загружен! Вы можете продолжить его или изменить его содержимое." << endl;
	}

	// Метод для генерации уникального числового идентификатора заказа на основе времени и даты заказа
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

	// Метод для подсчета стоимости всех пунктов меню в заказе
	double calculateOrderTotal() {
		double total = 0.0;
		for (const auto& item : cart) {
			total += item.getPrice();
			writeFinancialEvent("гость", "купил блюдо " + item.getName() + ": " + to_string(item.getPrice()));
		}
		return total;
	}

	// Метод для поиска последнего использованного идентификатора заказа
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

// Класс Сотрудник
class Employee {
protected:
	string username;
	string password;
	string role;
	string firstName;
	string middleName;
	string lastName;

	// Переопределение операторов для сериализации и десериализации
	friend ostream& operator<<(ostream& os, const Employee& employee) {
		os << employee.username << ' ' << employee.password << ' ' << employee.role << ' ' << employee.firstName << ' ' << employee.middleName << ' ' << employee.lastName;
		return os;
	}

	friend istream& operator>>(istream& is, Employee& employee) {
		is >> employee.username >> employee.password >> employee.role >> employee.firstName >> employee.middleName >> employee.lastName;
		return is;
	}

public:
	// Конструктор и другие методы
	Employee() {}
	Employee(const string& username) : username(username) {}
	Employee(const string& username, const string& password, const string& role, const string& firstName, const string& middleName, const string& lastName)
		: username(username), password(password), role(role), firstName(firstName), middleName(middleName), lastName(lastName) {}

	// Метод для вывода информации о сотруднике
	void PrintEmployeeInfo() const {
		cout << "\nЛогин: " << username << endl;
		cout << "Роль: " << role << endl;
		cout << "Имя: " << firstName << endl;
		cout << "Фамилия: " << middleName << endl;
		cout << "Отчество: " << lastName << endl;
		cout << "=============================" << endl;
	}

	// Геттеры и сеттеры
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

// Класс Системный администратор
class Administrator : public Employee {
private:
	vector<Employee> employees;
	vector<Product> products;
	vector<MenuItem> menu;
public:
	// Конструктор класса
	Administrator(const string& username) : Employee(username) {
		showMenu();
	}

	// Метод для отображения меню выбора действия
	void showMenu() {
		int choice;
		do
		{
			cout << "1. Редактировать меню" << endl;
			cout << "2. Редактировать список продуктов" << endl;
			cout << "3. Редактировать учетные записи сотрудников" << endl;
			cout << "4. Просмотр журнала аудита" << endl;
			cout << "5. Выйти" << endl;
			cout << "Выберите действие: " << endl;
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
				cout << "Неверный выбор действия." << endl;
				break;
			}
		} while (choice != 5);
	}

	// Редактирование меню
	void editMenu() {
		TextDeserializer<MenuItem> deserializer;
		deserializer.Deserialize(menu, "menu.txt");

		int choice;
		do {
			cout << "1. Добавить пункт в меню" << endl;
			cout << "2. Удалить пункт из меню" << endl;
			cout << "3. Просмотреть всё меню" << endl;
			cout << "4. Выйти" << endl;
			cout << "Выберите действие: " << endl;
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
				cout << "Неверный выбор действия." << endl;
				break;
			}
		} while (choice != 4);

		TextSerializer<MenuItem> serializer;
		serializer.Serialize(menu, "menu.txt");
	}

	// Добавление нового пункта меню
	void addMenuItem() {
		TextDeserializer<Product> deserializer;
		deserializer.Deserialize(products, "products.txt");

		cout << "Введите название блюда: ";
		string name = getInputString();
		cout << "Введите вес блюда: ";
		int weight = getInputInt();
		cout << "Введите описание блюда: ";
		string description = getInputString();
		cout << "Введите время приготовления блюда: ";
		int preparationTime = getInputInt();
		cout << "Введите стоимость блюда: ";
		double cost = getInputDouble();

		MenuItem newMenuItem(name, weight, {}, description, preparationTime, cost);

		int choice;
		do {
			cout << "Выберите продукт для добавления в состав блюда:" << endl;

			showAllProducts();

			int productId = getInputInt();
			auto it = find_if(products.begin(), products.end(), [productId](const Product& product) {
				return product.getId() == productId;
				});

			if (it == products.end()) {
				cout << "Продукт с таким ID не найден." << endl;
			}
			else {
				newMenuItem.addProduct(*it);
				cout << "Продукт успешно добавлен в состав блюда!" << endl;
			}

			cout << "Хотите добавить еще продукт? (1 - Да, 0 - Нет): ";
			choice = getInputInt();
		} while (choice != 0);

		menu.push_back(newMenuItem);

		writeAuditLog(username, "добавил пункт в меню: " + name);
		cout << "Пункт меню успешно добавлен!" << endl;
	}

	// Удаление пункта меню
	void deleteMenuItem() {
		cout << "Введите название блюда в меню для удаления: ";
		string menuItemName = getInputString();

		auto it = find_if(menu.begin(), menu.end(), [menuItemName](const MenuItem& menuItem) {
			return menuItem.getName() == menuItemName;
			});

		if (it == menu.end()) {
			cout << "Блюдо в меню с таким названием не найден." << endl;
			return;
		}

		menu.erase(it);

		writeAuditLog(username, "удалил из меню: " + it->getName());
		cout << "Пункт меню успешно удален!" << endl;
	}

	// Просмотр всего меню
	void showAllMenuItems() {
		cout << "Список всех блюд в меню:" << endl;
		for (const auto& menuItem : menu) {
			menuItem.PrintMenuItemInfo();
		}
	}

	// Редактирование списка продуктов
	void editProductList() {
		TextDeserializer<Product> deserializer;
		deserializer.Deserialize(products, "products.txt");

		int choice;
		do
		{
			cout << "1. Добавить продукт" << endl;
			cout << "2. Удалить продукт" << endl;
			cout << "3. Просмотреть все продукты" << endl;
			cout << "4. Выйти" << endl;
			cout << "Выберите действие: " << endl;
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
				cout << "Неверный выбор действия." << endl;
				break;
			}
		} while (choice != 4);

		TextSerializer<Product> serializer;
		serializer.Serialize(products, "products.txt");
	}

	// Добавление нового продукта
	void addProduct() {
		cout << "Введите наименование продукта: ";
		string name = getInputString();
		cout << "Введите цену продукта: ";
		double price = getInputDouble();
		cout << "Введите количество продукта: ";
		int quantity = getInputInt();

		int newId = getNextProductId();
		Product newProduct(newId, name, price, quantity);
		products.push_back(newProduct);

		writeAuditLog(username, "добавил в список продуктов: " + name);
		cout << "Продукт успешно добавлен!" << endl;
	}

	// Получение следующего свободного ID для продукта
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

	// Удаление продукта
	void deleteProduct() {
		cout << "Введите ID продукта для удаления: ";
		int productId = getInputInt();

		auto it = find_if(products.begin(), products.end(), [productId](const Product& product) {
			return product.getId() == productId;
			});

		if (it == products.end()) {
			cout << "Продукт с таким ID не найден." << endl;
			return;
		}

		products.erase(it);

		writeAuditLog(username, "удалил из списка продуктов: " + it->getName());
		cout << "Продукт успешно удален!" << endl;
	}

	// Просмотр всех продуктов
	void showAllProducts() {
		cout << "Список всех продуктов:" << endl;
		for (const auto& product : products) {
			product.PrintProductInfo();
		}
	}

	// Редактирование учетных записей сотрудников
	void editEmployeeAccounts() {
		TextDeserializer<Employee> deserializer;
		deserializer.Deserialize(employees, "employees.txt");

		int choice;
		do
		{
			cout << "1. Создать учетную запись" << endl;
			cout << "2. Изменить учетную запись" << endl;
			cout << "3. Удалить учетную запись" << endl;
			cout << "4. Просмотреть учетные записи" << endl;
			cout << "5. Просмотреть учётную запись" << endl;
			cout << "6. Выйти" << endl;
			cout << "Выберите действие: " << endl;
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
				cout << "Неверный выбор действия." << endl;
				break;
			}
		} while (choice != 6);

		TextSerializer<Employee> serializer;
		serializer.Serialize(employees, "employees.txt");
	}

	// Создание новой учётной записи сотрудника
	void createEmployeeAccount() {
		cout << "Введите имя пользователя: ";
		string username = getInputString();

		auto it = find_if(employees.begin(), employees.end(), [username](const Employee& employee) {
			return employee.getUsername() == username;
			});

		if (it != employees.end()) {
			cout << "Пользователь с таким именем уже существует." << endl;
			return;
		}

		cout << "Введите пароль: ";
		string password = getInputString();
		cout << "Введите роль: ";
		string role = getInputString();
		cout << "Введите фамилию: ";
		string middleName = getInputString();
		cout << "Введите имя: ";
		string firstName = getInputString();
		cout << "Введите отчество: ";
		string lastName = getInputString();

		Employee newEmployee(username, password, role, firstName, middleName, lastName);
		employees.push_back(newEmployee);

		writeAuditLog(username, "создал сотрудника: " + username);
		cout << "Учетная запись сотрудника успешно создана!" << endl;
	}

	// Изменение учетной записи сотрудника
	void editEmployeeAccount() {
		cout << "Введите имя пользователя: ";
		string username = getInputString();

		auto it = find_if(employees.begin(), employees.end(), [username](const Employee& employee) {
			return employee.getUsername() == username;
			});

		if (it == employees.end()) {
			cout << "Сотрудник с таким именем пользователя не найден." << endl;
			return;
		}

		Employee& employee = *it;
		int choice;
		do {

			cout << "Выберите поле для изменения:" << endl;
			cout << "1. Имя пользователя" << endl;
			cout << "2. Пароль" << endl;
			cout << "3. Роль" << endl;
			cout << "4. Фамилия" << endl;
			cout << "5. Имя" << endl;
			cout << "6. Отчество" << endl;
			cout << "7. Выйти" << endl;
			cout << "Выберите действие: ";
			choice = getInputInt();

			switch (choice) {
			case 1:
				cout << "Введите новое имя пользователя: ";
				employee.setUsername(getInputString());
				break;
			case 2:
				cout << "Введите новый пароль: ";
				employee.setPassword(hashPassword(getInputString()));
				break;
			case 3:
				cout << "Введите новую роль: ";
				employee.setRole(getInputString());
				break;
			case 4:
				cout << "Введите новую фамилию: ";
				employee.setLastName(getInputString());
				break;
			case 5:
				cout << "Введите новое имя: ";
				employee.setFirstName(getInputString());
				break;
			case 6:
				cout << "Введите новое отчество: ";
				employee.setMiddleName(getInputString());
				break;
			case 7:
				break;
			default:
				cout << "Неверный выбор действия." << endl;
				break;
			}
		} while (choice != 7);

		writeAuditLog(username, "изменил сотрудника: " + username);
		cout << "Учетная запись сотрудника успешно изменена!" << endl;
	}

	// Показать информацию о сотруднике
	void showEmployeeInfo() {
		cout << "Введите имя пользователя: ";
		string username = getInputString();

		auto it = find_if(employees.begin(), employees.end(), [username](const Employee& employee) {
			return employee.getUsername() == username;
			});

		if (it == employees.end()) {
			cout << "Сотрудник с таким именем пользователя не найден." << endl;
			return;
		}

		Employee& employee = *it;
		employee.PrintEmployeeInfo();
	}

	// Удаление учетной записи сотрудника
	void deleteEmployeeAccount() {
		cout << "Введите имя пользователя: ";
		string username = getInputString();

		auto it = find_if(employees.begin(), employees.end(), [username](const Employee& employee) {
			return employee.getUsername() == username;
			});

		if (it == employees.end()) {
			cout << "Сотрудник с таким именем пользователя не найден." << endl;
			return;
		}

		employees.erase(it);

		writeAuditLog(username, "удалил сотрудника: " + it->getUsername());
		cout << "Учетная запись сотрудника успешно удалена!" << endl;
	}

	// Просмотр всех сотрудников
	void showAllEmployees() {
		cout << "Список всех сотрудников:" << endl;
		for (const auto& employee : employees) {
			employee.PrintEmployeeInfo();
		}
	}

	// Просмотр журнала аудита
	void showAuditLog() {
		viewAuditLog();
	}
};

// Класс Складской
class Stockkeeper : public Employee {
private:
	vector<Product> products;
	vector<ProductRequest> requests;

public:
	Stockkeeper(const string& username) : Employee(username) {
		showMenu();
	}

	// Метод для отображения меню выбора действия
	void showMenu() {
		TextDeserializer<Product> deserializer;
		deserializer.Deserialize(products, "products.txt");
		TextDeserializer<ProductRequest> deserializer2;
		deserializer2.Deserialize(requests, "productRequests.txt");

		int choice;
		do
		{
			cout << "1. Просмотр склада" << endl;
			cout << "2. Создание заявки на продукт" << endl;
			cout << "3. Выйти" << endl;
			cout << "Выберите действие: " << endl;
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
				cout << "Неверный выбор действия." << endl;
				break;
			}
		} while (choice != 3);

		TextSerializer<ProductRequest> serializer2;
		serializer2.Serialize(requests, "productRequests.txt");
	}

	// Просмотр всех продуктов
	void showAllProducts() {
		cout << "Список всех продуктов:" << endl;
		for (const auto& product : products) {
			product.PrintProductInfo();
		}
	}

	// Создание заявки на продукт
	void createProductRequest() {
		cout << "Создание заявки на продукт" << endl;
		cout << "Введите ID продукта: ";
		int productId = getInputInt();

		auto it = find_if(products.begin(), products.end(), [productId](const Product& product) {
			return product.getId() == productId;
			});

		if (it == products.end()) {
			cout << "Продукт с таким ID не найден." << endl;
			return;
		}

		Product product = *it;

		cout << "Введите запрашиваемое количество: ";
		int requestedQuantity = getInputInt();

		double totalCost = product.getPrice() * requestedQuantity;
		if (totalCost > DeserializeBalance()) {
			cout << "Сумма заявки превышает баланс магазина. Заявка не может быть создана." << endl;
			return;
		}

		int newId = getNextRequestId();
		ProductRequest request(newId, product, requestedQuantity);
		request.setStatus("открыта");
		requests.push_back(request);

		writeAuditLog(username, "создал заявку на продукт: " + product.getName());
		cout << "Заявка на продукт успешно создана!" << endl;
	}

	// Получение следующего свободного ID для заявки на продукт
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

// Класс Поставщик
class Supplier : public Employee {
	vector<Product> products;
	vector<ProductRequest> requests;
public:
	Supplier(const string& username) : Employee(username) {
		showMenu();
	}

	// Метод для отображения меню выбора действия
	void showMenu() {
		TextDeserializer<Product> deserializer;
		deserializer.Deserialize(products, "products.txt");
		TextDeserializer<ProductRequest> deserializer2;
		deserializer2.Deserialize(requests, "productRequests.txt");

		int choice;
		do {
			cout << "Баланс ресторана: " << DeserializeBalance() << endl;
			cout << "1. Просмотр заявок на продукты" << endl;
			cout << "2. Отправить продукты по заявке" << endl;
			cout << "3. Выйти" << endl;
			cout << "Выберите действие: " << endl;
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
				cout << "Неверный выбор действия." << endl;
				break;
			}
		} while (choice != 3);

		TextSerializer<Product> serializer;
		serializer.Serialize(products, "products.txt");
		TextSerializer<ProductRequest> serializer2;
		serializer2.Serialize(requests, "productRequests.txt");
	}

	// Просмотр всех заявок на продукты
	void showAllRequests() {
		cout << "Список всех заявок на продукты:" << endl;
		for (const auto& request : requests) {
			if (request.getStatus() == "открыта") {
				request.PrintRequestInfo();
			}
		}
	}

	void sendProducts() {
		cout << "Отправка продуктов по заявке" << endl;
		cout << "Введите ID заявки: ";
		int requestId = getInputInt();

		auto it = find_if(requests.begin(), requests.end(), [requestId](const ProductRequest& request) {
			return request.getId() == requestId && request.getStatus() == "открыта";
			});

		if (it == requests.end()) {
			cout << "Заявка с таким ID не найдена или уже выполнена." << endl;
			return;
		}

		ProductRequest& request = *it;

		auto it2 = find_if(products.begin(), products.end(), [&request](const Product& product) {
			return product == request.getProduct();
			});

		if (it2 == products.end()) {
			cout << "Продукт с таким ID не найден." << endl;
			return;
		}

		Product& product = *it2;

		double balance = DeserializeBalance();
		double totalCost = product.getPrice() * request.getRequestedQuantity();
		if (totalCost > balance) {
			cout << "Сумма заявки превышает баланс магазина. Заявка не может быть выполнена." << endl;
			return;
		}

		product.setQuantity(product.getQuantity() + request.getRequestedQuantity());
		request.setStatus("выполнена");

		SerializeBalance(balance - totalCost);
		writeAuditLog(username, "выполнил заявку на продукт: " + product.getName());
		writeFinancialEvent(username, "потратил на заявку: " + to_string(totalCost));
		cout << "Заявка на продукт успешно обработана!" << endl;
	}
};

// Класс Бухгалтер
class Accountant : public Employee {
private:
	vector<ProductRequest> productRequests;

public:
	Accountant(const string& username) : Employee(username) {
		showMenu();
	}

	// Метод для отображения меню выбора действия
	void showMenu() {
		TextDeserializer<ProductRequest> deserializer;
		deserializer.Deserialize(productRequests, "productRequests.txt");

		int choice;
		do {
			cout << "1. Просмотр заявок на продукты" << endl;
			cout << "2. Просмотр финансовой отчётности" << endl;
			cout << "3. Выйти" << endl;
			cout << "Выберите действие: " << endl;
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
				cout << "Неверный выбор действия." << endl;
				break;
			}
		} while (choice != 3);
	}

	// Просмотр всех заявок на продукты
	void showAllProductRequests() {
		cout << "Список всех заявок на продукты:" << endl;
		for (const auto& request : productRequests) {
			request.PrintRequestInfo();
		}
	}

	// Метод для просмотра финансовой отчётности
	void viewFinancialReport() {
		double balance = DeserializeBalance();
		double totalSales = calculateTotalSales();
		double totalExpenses = calculateTotalExpenses();

		cout << "Финансовая отчётность:" << endl;
		cout << "Баланс денежных средств: " << balance << endl;
		cout << "Общая сумма продаж: " << totalSales << endl;
		cout << "Сумма расходов на выполнение заявок: " << totalExpenses << endl;
	}

	// Функция для расчета общей суммы продаж
	double calculateTotalSales() {
		double totalSales = 0.0;
		ifstream file("financial_report.txt");
		if (file.is_open()) {
			string line;
			while (getline(file, line)) {
				size_t pos = line.find_last_of(':');
				string amountStr = line.substr(pos + 2);
				double amount = stod(amountStr);
				if (line.find("купил блюдо") != string::npos) {
					totalSales += amount;
				}
			}
			file.close();
		}
		else {
			cerr << "Ошибка открытия файла финансового отчёта." << endl;
		}
		return totalSales;
	}

	// Функция для расчета расходов на выполнение заявок
	double calculateTotalExpenses() {
		double totalExpenses = 0.0;
		ifstream file("financial_report.txt");
		if (file.is_open()) {
			string line;
			while (getline(file, line)) {
				size_t pos = line.find_last_of(':');
				string amountStr = line.substr(pos + 2);
				double amount = stod(amountStr);
				if (line.find("потратил на заявку") != string::npos) {
					totalExpenses += amount;
				}
			}
			file.close();
		}
		else {
			cerr << "Ошибка открытия файла финансового отчёта." << endl;
		}
		return totalExpenses;
	}
};

// Класс Повар
class Chef : public Employee {
private:
	vector<Order> orders;

public:
	Chef(const string& username) : Employee(username) {
		showMenu();
	}

	// Метод для отображения меню выбора действия
	void showMenu() {
		TextDeserializer<Order> deserializer;
		deserializer.Deserialize(orders, "orders.txt");

		int choice;
		do {
			cout << "1. Просмотреть заказы" << endl;
			cout << "2. Начать приготовление заказа" << endl;
			cout << "3. Выйти" << endl;
			cout << "Выберите действие: ";
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
				cout << "Неверный выбор действия." << endl;
				break;
			}
		} while (choice != 3);

		TextSerializer<Order> serializer;
		serializer.Serialize(orders, "orders.txt");
	}

	// Метод для просмотра всех заказов
	void viewOrders() {
		if (orders.empty()) {
			cout << "Список заказов пуст." << endl;
			return;
		}

		cout << "Список заказов:" << endl;
		for (const auto& order : orders) {
			if (order.getStatus() == "Оплачен и передан на кухню") {
				order.PrintOrderInfo();
			}
		}
	}

	// Метод для начала приготовления заказа
	void startOrderPreparation() {
		if (orders.empty()) {
			cout << "Список заказов пуст." << endl;
			return;
		}

		cout << "Введите номер заказа, который вы хотите приготовить: ";
		int orderId = getInputInt();

		auto it = find_if(orders.begin(), orders.end(), [orderId](const Order& order) {
			return order.getId() == orderId;
			});

		if (it == orders.end()) {
			cout << "Заказ с таким номером не найден." << endl;
			return;
		}

		if (it->getStatus() != "Оплачен и передан на кухню") {
			cout << "Заказ не может быть приготовлен, так как его статус не является 'Оплачен и передан на кухню'." << endl;
			return;
		}

		it->setStatus("В процессе приготовления");
		cout << "Заказ " << orderId << " начал приготовление." << endl;

		thread preparationThread(&Chef::prepareOrder, this, ref(*it));
		preparationThread.join();
	}

	// Метод для приготовления заказа
	void prepareOrder(Order& order) {
		vector<int> productNumbers = order.getProductNumbers();
		bool allProductsPrepared = true;

		cout << "Приготовление продуктов для заказа " << order.getId() << ":" << endl;

		for (int productNumber : productNumbers) {
			cout << "Введите номер продукта для приготовления: ";
			int product = getInputInt();

			auto it = find(productNumbers.begin(), productNumbers.end(), product);
			if (it != productNumbers.end()) {
				cout << "Продукт " << product << " приготовлен." << endl;
			}
			else {
				cout << "Продукт с номером " << product << " не найден в заказе." << endl;
				allProductsPrepared = false;
			}
		}

		if (allProductsPrepared) {
			order.setStatus("Передан официанту на выдачу");
			writeAuditLog(username, "приготовил заказ: " + to_string(order.getId()));
			cout << "Заказ " << order.getId() << " готов к выдаче." << endl;
		}
		else {
			cout << "Заказ " << order.getId() << " не готов к выдаче." << endl;
		}
	}
};

// Класс Официант
class Waiter : public Employee {
private:
	vector<Order> orders;

public:
	Waiter(const string& username) : Employee(username) {
		showMenu();
	}

	// Метод для отображения меню выбора действия
	void showMenu() {
		TextDeserializer<Order> deserializer;
		deserializer.Deserialize(orders, "orders.txt");

		int choice;
		do {
			cout << "1. Просмотреть заказы" << endl;
			cout << "2. Выдать заказ" << endl;
			cout << "3. Выйти" << endl;
			cout << "Выберите действие: ";
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
				cout << "Неверный выбор действия." << endl;
				break;
			}
		} while (choice != 3);

		TextSerializer<Order> serializer;
		serializer.Serialize(orders, "orders.txt");
	}

	// Метод для просмотра всех заказов
	void viewOrders() {
		if (orders.empty()) {
			cout << "Список заказов пуст." << endl;
			return;
		}

		cout << "Список заказов, доступных для выдачи:" << endl;
		for (const auto& order : orders) {
			if (order.getStatus() == "Передан официанту на выдачу") {
				order.PrintOrderInfo();
			}
		}
	}

	// Метод для выдачи заказа
	void serveOrder() {
		if (orders.empty()) {
			cout << "Список заказов пуст." << endl;
			return;
		}

		cout << "Введите номер заказа, который вы хотите выдать: ";
		int orderId = getInputInt();

		auto it = find_if(orders.begin(), orders.end(), [orderId](const Order& order) {
			return order.getId() == orderId && order.getStatus() == "Передан официанту на выдачу";
			});

		if (it == orders.end()) {
			cout << "Заказ с таким номером не найден или не доступен для выдачи." << endl;
			return;
		}

		it->setStatus("Выполнен");

		writeAuditLog(username, "выдал заказ: " + to_string(it->getId()));
		cout << "Заказ " << orderId << " выдан." << endl;
	}
};