#pragma once
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <locale>
#include <wtypes.h>
#include <iomanip>
#include <functional>
#include <vector>
#include <algorithm>
#include <limits>
#include <thread>
#include <map>

using namespace std;

string hashPassword(const string& password);

string getInputString();
int getInputInt();
double getInputDouble();

void writeAuditLog(const string& username, const string& action);
void viewAuditLog();

void writeFinancialEvent(const string& username, const string& event);
void viewFinancialReport();