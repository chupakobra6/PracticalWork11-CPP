#pragma once

// ����� �������
class Product {
private:
	int id;
	string name;
	double price;
	int quantity;

	friend ostream& operator<<(ostream& os, const Product& product) {
		os << product.id << ' ' << product.name << ' ' << product.price << ' ' << product.quantity;
		return os;
	}

	friend istream& operator>>(istream& is, Product& product) {
		is >> product.id >> product.name >> product.price >> product.quantity;
		return is;
	}

public:
	bool operator==(const Product& other) const {
		return (id == other.id) && (name == other.name) && (price == other.price);
	}

	Product() {}
	Product(int id, const string& productName, double productPrice, int productQuantity)
		: id(id), name(productName), price(productPrice), quantity(productQuantity) {}

	int getId() const {
		return id;
	}

	string getName() const {
		return name;
	}

	double getPrice() const {
		return price;
	}

	int getQuantity() const {
		return quantity;
	}

	void setQuantity(int newQuantity) {
		quantity = newQuantity;
	}

	void PrintProductInfo() const {
		cout << "\nID: " << id << endl;
		cout << "��������: " << name << endl;
		cout << "����: " << price << endl;
		cout << "����������: " << quantity << endl;
		cout << "============================" << endl;
	}
};

// ����� ����
class MenuItem {
private:
	string name;
	int weight;
	vector<Product> composition;
	string description;
	int preparationTime;
	double cost;

	// ��������������� ��������� ������ <<
	friend ostream& operator<<(ostream& os, const MenuItem& menuItem) {
		os << menuItem.name << '\n';
		os << menuItem.weight << '\n';

		os << menuItem.composition.size() << '\n';
		for (const auto& product : menuItem.composition) {
			os << product << '\n';
		}

		os << menuItem.description << '\n';
		os << menuItem.preparationTime << '\n';
		os << menuItem.cost << '\n';

		return os;
	}

	// ��������������� ��������� ����� >>
	friend istream& operator>>(istream& is, MenuItem& menuItem) {
		getline(is, menuItem.name);
		is >> menuItem.weight;

		int compositionSize;
		is >> compositionSize;
		is.ignore();

		menuItem.composition.clear();
		for (int i = 0; i < compositionSize; i++) {
			Product product;
			is >> product;
			menuItem.composition.push_back(product);
		}

		is.ignore();

		getline(is, menuItem.description);
		is >> menuItem.preparationTime;
		is >> menuItem.cost;

		return is;
	}

public:
	MenuItem() {}
	MenuItem(const string& name, int weight, vector<Product> composition, const string& description, int preparationTime, double cost)
		: name(name), weight(weight), composition(composition), description(description), preparationTime(preparationTime), cost(cost) {}

	string getName() const {
		return name;
	}

	double getPrice() const {
		return cost;
	}

	vector<Product> getComposition() const {
		return composition;
	}

	void addProduct(const Product& product) {
		composition.push_back(product);
	}

	// ����� ���������� � ������ ����
	void PrintMenuItemInfo() const {
		cout << "��������: " << name << endl;
		cout << "���: " << weight << endl;
		cout << "������: ";
		for (const auto& product : composition) {
			cout << product.getName() << ", ";
		}
		cout << endl;
		cout << "��������: " << description << endl;
		cout << "����� �������������: " << preparationTime << endl;
		cout << "���������: " << cost << endl;
		cout << "------------------------------" << endl;
	}
};

// ����� �����
class Order {
private:
	int id;
	vector<MenuItem> cart;
	double totalCost;
	string status;

public:
	void setId(int newId) {
		id = newId;
	}

	int getId() const {
		return id;
	}

	void setItems(const vector<MenuItem>& items) {
		cart = items;
	}

	string getStatus() const {
		return status;
	}

	void setStatus(const string& newStatus) {
		status = newStatus;
	}

	void setTotalCost(double cost) {
		totalCost = cost;
	}

	vector<int> getProductNumbers() const {
		vector<int> productNumbers;
		for (const auto& item : cart) {
			vector<Product> composition = item.getComposition();
			for (const auto& product : composition) {
				productNumbers.push_back(product.getId());
			}
		}
		return productNumbers;
	}

	void PrintOrderInfo() const {
		cout << "����� ������: " << id << endl;
		cout << "���������� ������:" << endl;
		for (const auto& item : cart) {
			item.PrintMenuItemInfo();
		}
		cout << "����� ��������� ������: " << totalCost << endl;
		cout << "������ ������: " << status << endl;
	}

	// ��������������� ��������� ������ <<
	friend ostream& operator<<(ostream& os, const Order& order) {
		os << order.id << '\n';
		os << order.cart.size() << '\n';
		for (const auto& item : order.cart) {
			os << item << '\n';
		}
		os << order.totalCost << '\n';
		os << order.status << '\n';
		return os;
	}

	// ��������������� ��������� ����� >>
	friend istream& operator>>(istream& is, Order& order) {
		is >> order.id;

		int cartSize;
		is >> cartSize;
		is.ignore();

		order.cart.clear();
		for (int i = 0; i < cartSize; i++) {
			MenuItem item;
			is >> item;
			order.cart.push_back(item);
		}

		is >> order.totalCost;
		is.ignore();

		getline(is, order.status);

		return is;
	}
};

// ����� ������ �� ��������
class ProductRequest {
private:
	int id;
	Product product;
	int requestedQuantity;
	string status;

	friend ostream& operator<<(ostream& os, const ProductRequest& request) {
		os << request.id << " " << request.product << " " << request.requestedQuantity << " " << request.status;
		return os;
	}

	friend istream& operator>>(istream& is, ProductRequest& request) {
		is >> request.id >> request.product >> request.requestedQuantity >> request.status;
		return is;
	}

public:
	ProductRequest() {}
	ProductRequest(int id, const Product& product, int requestedQuantity) : id(id), product(product), requestedQuantity(requestedQuantity) {}

	int getId() const {
		return id;
	}

	Product getProduct() const {
		return product;
	}

	int getRequestedQuantity() const {
		return requestedQuantity;
	}

	string getStatus() const {
		return status;
	}

	void setStatus(const string& newStatus) {
		status = newStatus;
	}

	void PrintRequestInfo() const {
		cout << "\nID ������: " << id << endl;
		cout << "�������� ��������: " << product.getName() << endl;
		cout << "��������� ��������: " << product.getPrice() << endl;
		cout << "������� ����������: " << product.getQuantity() << endl;
		cout << "����������� ����������: " << requestedQuantity << endl;
		cout << "������: " << status << endl;
		cout << "============================" << endl;
	}
};