/*
 * Vehicle Service Management System
 * Final project, C Programming course.
 * Author: Guy Ifergan
 */

#define _CRT_SECURE_NO_WARNINGS
#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_EMPLOYEES 100
#define MAX_NAME 20
#define MAX_DATE 11
#define MAX_NUMBERPLATE 10
#define MAX_ITEMS 100
#define MAX_PURCHASES_PER_DAY 3
#define CLIENTS_FILE "clients.txt"
#define EMPLOYEES_FILE "Employee.txt"
#define ITEMS_FILE "items.bin"
#define INVENTORY_FILE "inventory.bin"
#define SALES_FILE "sales.bin"
#define LOG_FILE "log.txt"

typedef struct client
{
    char name[20];
    char date[11];
    bool inservice;
    float totalspent;
    char numberplate[10];

}Client;
typedef struct item
{
    char numberplate[10];
    char carbrand[20];
    char color[10];
    char entrydate[11];
    char exitdate[11];
} Item;
typedef struct sale
{
    char numberplate[10];
    char productname[20];
    float price;
    char date[11];
}Sale;
typedef struct inventoryitem {
    char productname[25];
    int quantity;
    float price;
}Inventoryitem;
typedef struct employee {
    char username[20];
    char password[10];
	char firstname[10];
    unsigned int level; // 1 - admin, 2 - employee, 3 - practicioner
} Employee;

void WriteToLog(const char* message)
{
	FILE* LogFile = fopen(LOG_FILE, "a");
	if (LogFile == NULL)
	{
		printf("Error opening log file!\n");
		return;
	}
	time_t now;
	time(&now);
	char* timestr = ctime(&now);
	timestr[strcspn(timestr, "\n")] = '\0';

	fprintf(LogFile, "[%s] %s\n", timestr, message);
	fclose(LogFile);
}


void InitializeInventory()
{
    FILE* inventory = fopen(INVENTORY_FILE, "wb");
    if (!inventory)
    {
        printf("Error creating inventory file!\n");
        WriteToLog("Error creating inventory file.");
        return;
    }

    Inventoryitem Items[20] =
    {
        {"oil change",10,50.0},
        {"air filter",15,30.0},
        {"breake pads",8,100.0},
        {"tire replacement",12,200.0},
        {"battery",7,150.0},
        {"coolant",20,25.0},
        {"headlight",10,40.0},
        {"wipers",18,20.0},
        {"spark plugs",25,15.0},
        {"engine oil",14,60.0},
        {"transmission fluid",10,80.0},
        {"steering fluid",9,35.0},
        {"air conditioning gas",6,120.0},
        {"fuel injector cleaner",15,20.0},
        {"brake fluid",11,50.0},
        {"suspansion kit",5,300.0},
        {"radiator",4,250.0},
        {"alternator",3,350.0},
        {"starter motor",6,180.0},
        {"exhaust pipe",7,220.0}
    };

    fwrite(Items, sizeof(Inventoryitem), 20, inventory);
    fclose(inventory);
    WriteToLog("inventory updated.");
}
int UpdateInventory(const char* productname, int quantity) {
    FILE* inventory = fopen(INVENTORY_FILE, "r+b");
    if (!inventory) {
        printf("Error opening inventory file!\n");
        return 0;
    }

    Inventoryitem tempItem;

    while (fread(&tempItem, sizeof(Inventoryitem), 1, inventory)) {
        if (strcmp(tempItem.productname, productname) == 0) {
            if (tempItem.quantity >= quantity)
            {
                tempItem.quantity -= quantity;
                fseek(inventory, -(long)sizeof(Inventoryitem), SEEK_CUR);
                fwrite(&tempItem, sizeof(Inventoryitem), 1, inventory);
                fclose(inventory);
                return 1;
            }
            else
            {
                printf("Not enough stock for %s! Only %d left.\n", productname, tempItem.quantity);
                fclose(inventory);
                return 0;
            }
        }
    }

    fclose(inventory);
    printf("Product %s not found in inventory!\n", productname);
    return 0;
}
void DisplayInventory(FILE* inventory) {
	Inventoryitem product;
	(void)inventory; /* file pointer is not reused — open by name */

	inventory = fopen(INVENTORY_FILE, "rb");
	if (!inventory) {
		printf("Error opening inventory file!\n");
		return;
	}

	printf("\nAvailable products:\n");
	printf("------------------------------------------------------\n");
	printf("| %-25s | %-10s | %-10s |\n", "Product Name", "Quantity", "Price");
	printf("------------------------------------------------------\n");

	while (fread(&product, sizeof(Inventoryitem), 1, inventory)) {
		printf("| %-25s | %-10d | %-10.2f |\n", product.productname, product.quantity, product.price);
	}

	printf("------------------------------------------------------\n");

	fclose(inventory);
}

void GetCurrentDate(char* dateBuffer, size_t bufferSize) {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	snprintf(dateBuffer, bufferSize, "%02d-%02d-%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}
void trimWhitespace(char* str) {
	char* end;

	while (isspace((unsigned char)*str)) str++;

	if (*str == 0) return;

	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end)) end--;

	*(end + 1) = '\0';
}

void createDefaultAdmin(FILE* fEmployees)
{
    Employee Admin;
    (void)fEmployees;
    fEmployees = fopen(EMPLOYEES_FILE, "w");
    if (fEmployees == NULL)
    {
        printf("Error opening file!\n");
        WriteToLog("error opening employee file.");
        return;
    }
    strcpy(Admin.username, "admin");
    strcpy(Admin.firstname, "maneger");
    strcpy(Admin.password, "12345678");
    Admin.level = 1;
    fprintf(fEmployees, "%-20s %10s %10s %5s\n", "USERNAME", 
		"PASSWORD", "FIRSTNAME", "LEVEL");
    fprintf(fEmployees, "%-20s %10s %10s %5u\n", Admin.username, Admin.password,
		Admin.firstname, Admin.level);
    fclose(fEmployees);
    WriteToLog("added admin user.");
}
Employee login(FILE* fEmployee) {
	(void)fEmployee;
	fEmployee = fopen(EMPLOYEES_FILE, "r");
	if (fEmployee == NULL) {
		printf("Failed to open file!\n");
		WriteToLog("Error opening employee file.");
		exit(1);
	}

	Employee login, search;
	int attempts = 3;

	char header[100];
	fgets(header, sizeof(header), fEmployee);

	while (attempts > 0) {
		printf("Please enter username:\n");
		scanf("%19s", login.username);

		printf("Please enter password:\n");
		scanf("%9s", login.password);

		rewind(fEmployee);
		fgets(header, sizeof(header), fEmployee);

		int found = 0;
		while (fscanf(fEmployee, "%s %s %s %d", search.username, search.password,
			search.firstname, &search.level) == 4) {
			if (strcmp(login.username, search.username) == 0
				&& strcmp(login.password, search.password) == 0) {
				found = 1;
				break;
			}
		}

		if (found) {
			if (search.level == 0) {
				printf("Access denied. You are blocked!\n");
				WriteToLog("Blocked user attempted login.");
				fclose(fEmployee);
				exit(1);
			}

			printf("Hi %s!\n", search.firstname);
			WriteToLog("Employee login successful.");
			fclose(fEmployee);
			return search;
		}
		else {
			printf("Incorrect username or password. Please try again!\n");
			WriteToLog("Failed login attempt.");
			attempts--;
		}
	}

	printf("Login failed, please contact the manager.\n");
	WriteToLog("3 failed login attempts.");
	fclose(fEmployee);
	exit(1);
}
void AddNewEmployee(unsigned int employeelevel, FILE* fEmployees)
{
    int level = employeelevel;
	int value;
    if (level == 1)
    {
        Employee New, existing;
        (void)fEmployees;
        fEmployees = fopen(EMPLOYEES_FILE, "a+");
        if (fEmployees == NULL)
        {
            printf("Error opening file!\n");
            WriteToLog("Error opening employee file.");
            return;
        }
        printf("please enter username:\n");
        scanf("%19s", New.username);
        fseek(fEmployees, 0, SEEK_SET);
        while (fscanf(fEmployees, "%19s %9s %9s %u", existing.username, existing.password, existing.firstname, &existing.level) == 4)
        {
            if (strcmp(New.username, existing.username) == 0)
            {
                printf("username already exists!\n");
                WriteToLog("employee register failed, already exists.");
                fclose(fEmployees);
                return;
            }
        }

        printf("please enter password:\n");
        scanf("%9s", New.password);

        printf("please enter firstname:\n");
        scanf("%9s", New.firstname);

        printf("please enter level (1=admin, 2=employee, 3=practitioner):\n");
        scanf("%d", &value);
        if (value > 0 && value < 4)
        {
            New.level = (unsigned int)value;
        }
        else
        {
            printf("level not within range! try again.\n");
            fclose(fEmployees);
            return;
        }

        fseek(fEmployees, 0, SEEK_END);
        fprintf(fEmployees, "%-20s %10s %10s %5u\n", New.username, New.password, New.firstname, New.level);

        fclose(fEmployees);
        printf("New employee added successfully!\n");
    }
    else
    {
        printf("Action not allowed!\n");
        WriteToLog("register attempt by unauthurized employee.");
        return;
    }
}
void BlockEmployee(FILE* fEmployee)
{
	(void)fEmployee;
	fEmployee = fopen(EMPLOYEES_FILE, "r");
	if (fEmployee == NULL) {
		printf("Error: Could not open Employee.txt for reading!\n");
		return;
	}

	Employee employees[MAX_EMPLOYEES];
	int count = 0;

	char header[100];
	fgets(header, sizeof(header), fEmployee);  

	while (fscanf(fEmployee, "%19s %9s %9s %d",
		employees[count].username,
		employees[count].password,
		employees[count].firstname,
		&employees[count].level) == 4) {
		count++;
		if (count >= MAX_EMPLOYEES) {
			printf("Warning: Too many employees in file, some may not be processed!\n");
			break;
		}
	}

	fclose(fEmployee);

	char targetUsername[20];
	printf("Enter username to block: ");
	scanf("%19s", targetUsername);

	int found = 0;
	for (int i = 0; i < count; i++) {
		if (strcmp(employees[i].username, targetUsername) == 0) {
			employees[i].level = 0;  
			found = 1;
			break;
		}
	}

	if (!found) {
		printf("Error: User not found!\n");
		return;
	}

	fEmployee = fopen(EMPLOYEES_FILE, "w");
	if (fEmployee == NULL) {
		printf("Error: Could not open Employee.txt for writing!\n");
		return;
	}

	fprintf(fEmployee, "%-20s %-10s %-10s %-5s\n", "USERNAME", "PASSWORD", "FIRSTNAME", "LEVEL");

	for (int i = 0; i < count; i++) {
		fprintf(fEmployee, "%-20s %-10s %-10s %-5d\n",
			employees[i].username,
			employees[i].password,
			employees[i].firstname,
			employees[i].level);
	}

	fclose(fEmployee);

	printf("User '%s' has been blocked successfully!\n", targetUsername);
}

void AddClient(FILE* clients, FILE* items)
{
	(void)clients; (void)items;
	clients = fopen(CLIENTS_FILE, "a");
	if (clients == NULL) {
		printf("Error: Could not open clients file for writing!\n");
		return;
	}

	items = fopen(ITEMS_FILE, "ab");
	if (items == NULL) {
		printf("Error: Could not open items file for writing!\n");
		fclose(clients);
		return;
	}

	Client newclient;
	Item newitem;
	int temp;

	printf("What is the client name?\n");
	scanf("%19s", newclient.name);

	printf("What is the date? (DD-MM-YYYY)\n");
	scanf("%10s", newclient.date);

	printf("What is the cost?\n");
	scanf("%f", &newclient.totalspent);

	printf("Is the car in need of repairs? (1-yes, 2-no)\n");
	scanf("%d", &temp);
	newclient.inservice = (temp == 1);

	printf("What is the client numberplate?\n");
	scanf("%9s", newclient.numberplate);

	strcpy(newitem.numberplate, newclient.numberplate);

	printf("What is the client car brand?\n");
	scanf("%19s", newitem.carbrand);

	printf("What is the client car paint?\n");
	scanf("%9s", newitem.color);

	printf("What date does the car enter? (DD-MM-YYYY)\n");
	scanf("%10s", newitem.entrydate);

	printf("What date does the car leave? (DD-MM-YYYY)\n");
	scanf("%10s", newitem.exitdate);

	fprintf(clients, "%-20s %-10s %-10d %-10.2f %-10s\n",
		newclient.name, newclient.date, newclient.inservice, newclient.totalspent, newclient.numberplate);

	fwrite(&newitem, sizeof(Item), 1, items);

	fclose(items);
	fclose(clients);

	printf("Client and car added successfully!\n");
}
void findbynumberplate(FILE* clients, FILE* items)
{
	Client findclient;
	printf("please enter plate number:\n");
	scanf("%s", findclient.numberplate);

	clients = fopen(CLIENTS_FILE, "r");
	items = fopen(ITEMS_FILE, "rb");

	if (clients == NULL || items == NULL)
	{
		printf("Error opening one of the files!\n");
		if (clients) fclose(clients);
		if (items) fclose(items);
		WriteToLog("one of the files failed to open.");
		return;
	}
	Client readclient;
	Item readitem;
	bool clientfound = false;
	bool itemfound = false;
	int inservice_int;

	while (fscanf(clients, "%s %s %d %f %s", readclient.name, readclient.date, &inservice_int, &readclient.totalspent, readclient.numberplate) == 5)
	{
		readclient.inservice = (inservice_int != 0);
		if (strcmp(readclient.numberplate, findclient.numberplate) == 0)
		{
			clientfound = true;
			printf("\n=== client information ===\n");
			printf("client name: %s\n", readclient.name);
			printf("Registration date: %s\n", readclient.date);
			printf("In service: %s\n", readclient.inservice ? "Yes" : "No");
			printf("Total spent: %.2f\n", readclient.totalspent);
			printf("Car number plate: %s\n", readclient.numberplate);
			WriteToLog("found client.");
			break;
		}
	}
	if (!clientfound)
	{
		printf("No client found with number plate: %s", findclient.numberplate);
		fclose(clients);
		fclose(items);
		WriteToLog("didnt find client.");
		return;
	}

	while (fread(&readitem, sizeof(Item), 1, items) == 1)
	{
		if (strcmp(readitem.numberplate, findclient.numberplate) == 0)
		{
			itemfound = true;
			printf("\n=== client information ===\n");
			printf("Car number plate: %s\n", readitem.numberplate);
			printf("Car brand %s\n", readitem.carbrand);
			printf("Color %s\n", readitem.color);
			printf("Entry date: %s\n", readitem.entrydate);
			printf("Exit date: %s\n", readitem.exitdate);
			WriteToLog("found item.");
			break;
		}
	}
	if (!itemfound)
	{
		printf("No car found with number plate: %s", findclient.numberplate);
		WriteToLog("didnt found item.");
	}
	fclose(clients);
	fclose(items);
}
int ConvertDateToInt(const char* date)
{
	int day, month, year;
	sscanf(date, "%2d-%2d-%4d", &day, &month, &year);
	return year * 1000 + month * 100 + day;
}
int CompareByDate(const void* a, const void* b)
{
	Client* clinetA = (Client*)a;
	Client* clinetB = (Client*)b;

	int dateA = ConvertDateToInt(clinetA->date);
	int dateB = ConvertDateToInt(clinetB->date);

	return dateA - dateB;
}
void SortClientsByDate(FILE* clients)
{
	(void)clients;
	clients = fopen(CLIENTS_FILE, "r");
	if (clients == NULL)
	{
		printf("Error opening client file!\n");
		WriteToLog("failed to open clients file.");
		return;
	}

	Client* Clients = (Client*)malloc(sizeof(Client) * MAX_EMPLOYEES);
	if (Clients == NULL)
	{
		printf("Memory allocation failed!\n");
		fclose(clients);
		return;
	}

	int count = 0;
	int inservice_int;
	while (count < MAX_EMPLOYEES &&
		fscanf(clients, "%s %s %d %f %s",
			Clients[count].name, Clients[count].date,
			&inservice_int, &Clients[count].totalspent,
			Clients[count].numberplate) == 5)
	{
		Clients[count].inservice = (inservice_int != 0);
		count++;
	}
	fclose(clients);

	if (count == 0)
	{
		printf("no clients found!\n");
		WriteToLog("no clients found in file.");
		free(Clients);
		return;
	}
	qsort(Clients, count, sizeof(Client), CompareByDate);

	clients = fopen(CLIENTS_FILE, "w");
	if (clients == NULL)
	{
		printf("Error opening the file!\n");
		WriteToLog("failed to open clients file.");
		free(Clients);
		return;
	}
	for (int i = 0; i < count; i++)
	{
		fprintf(clients, "%s %s %d %.2f %s\n",
			Clients[i].name, Clients[i].date, Clients[i].inservice,
			Clients[i].totalspent, Clients[i].numberplate);
	}
	WriteToLog("rearranged clients file.");
	fclose(clients);
	free(Clients);
	printf("Clients sorted by registration date successfully!\n");
}

bool GetClientNameByNumberPlate(FILE* clients, const char* carnumberplate, char* clientsname)
{
	(void)clients;
	clients = fopen(CLIENTS_FILE, "r");
	if (clients == NULL)
	{
		printf("Error opening clients file!\n");
		WriteToLog("Error opening clients file.");
		return false;
	}
	Client readclient;
	int inservice_int;
	while (fscanf(clients, "%s %s %d %f %s", readclient.name, readclient.date, &inservice_int, &readclient.totalspent, readclient.numberplate) == 5)
	{
		if (strcmp(readclient.numberplate, carnumberplate) == 0)
		{
			strcpy(clientsname, readclient.name);
			fclose(clients);
			WriteToLog("found client.");
			return true;
		}
	}
	fclose(clients);
	return false;
}
void AddSaleToClient(FILE* sales, FILE* clients, FILE* inventory) {
	(void)sales; (void)clients; (void)inventory;
	char numberplate[10];
	printf("Enter client number plate: ");
	scanf("%9s", numberplate);

	char currentDate[11];
	GetCurrentDate(currentDate, sizeof(currentDate));

	int purchaseCount = 0;
	Sale tempSale;

	sales = fopen(SALES_FILE, "rb+");
	if (!sales) {
		sales = fopen(SALES_FILE, "wb+");
	}
	if (!sales) {
		printf("Error opening sales file!\n");
		WriteToLog("Error opening sales file.");
		return;
	}

	inventory = fopen(INVENTORY_FILE, "rb+");
	if (!inventory) {
		printf("Error opening inventory file!\n");
		WriteToLog("Error opening inventory file.");
		fclose(sales);
		return;
	}

	rewind(sales);
	while (fread(&tempSale, sizeof(Sale), 1, sales)) {
		if (strcmp(tempSale.numberplate, numberplate) == 0 && strcmp(tempSale.date, currentDate) == 0) {
			purchaseCount++;
		}
	}

	if (purchaseCount >= MAX_PURCHASES_PER_DAY) {
		printf("Client has reached the daily purchase limit of %d items.\n", MAX_PURCHASES_PER_DAY);
		fclose(sales);
		return;
	}

	DisplayInventory(inventory);

	int totalItemsPurchased = 0;
	float totalCost = 0.0;

	while (totalItemsPurchased < MAX_PURCHASES_PER_DAY) {
		char productName[25];
		int found = 0;
		int quantity;

		printf("Enter product name (or 'done' to finish): ");
		getchar();
		fgets(productName, sizeof(productName), stdin);
		productName[strcspn(productName, "\n")] = '\0';

		if (_stricmp(productName, "done") == 0) {
			break;
		}

		rewind(inventory);
		long pos = 0;
		Inventoryitem product;

		while (fread(&product, sizeof(Inventoryitem), 1, inventory)) {
			if (_stricmp(product.productname, productName) == 0) {
				found = 1;
				break;
			}
			pos = ftell(inventory);
		}

		if (!found) {
			printf("Product not found in inventory.\n");
			continue;
		}

		printf("Enter quantity (max %d available, total left today %d): ", product.quantity, MAX_PURCHASES_PER_DAY - totalItemsPurchased);
		scanf("%d", &quantity);

		if (quantity <= 0 || quantity > product.quantity || (totalItemsPurchased + quantity) > MAX_PURCHASES_PER_DAY) {
			printf("Invalid quantity!\n");
			continue;
		}

		product.quantity -= quantity;
		fseek(inventory, pos, SEEK_SET);
		fwrite(&product, sizeof(Inventoryitem), 1, inventory);
		fflush(inventory);

		for (int i = 0; i < quantity; i++) {
			Sale newSale;
			strcpy(newSale.numberplate, numberplate);
			strcpy(newSale.productname, product.productname);
			newSale.price = product.price;
			strcpy(newSale.date, currentDate);

			fseek(sales, 0, SEEK_END);
			fwrite(&newSale, sizeof(Sale), 1, sales);
			fflush(sales);
		}

		totalItemsPurchased += quantity;
		totalCost += (quantity * product.price);
		printf("Sale successfully added for client %s on %s.\n", numberplate, currentDate);

		if (totalItemsPurchased >= MAX_PURCHASES_PER_DAY) {
			printf("Client has reached the daily purchase limit.\n");
			break;
		}
	}

	fclose(inventory);
	fclose(sales);

	clients = fopen(CLIENTS_FILE, "r");
	if (!clients) {
		printf("Error opening clients file.\n");
		return;
	}

	FILE* tempFile = fopen("temp_clients.txt", "w");
	if (!tempFile) {
		printf("Error opening temporary file.\n");
		fclose(clients);
		return;
	}

	Client client;
	int inservice_int;
	while (fscanf(clients, "%19s %10s %d %f %9s", client.name, client.date, &inservice_int, &client.totalspent, client.numberplate) == 5) {
		client.inservice = (inservice_int != 0);
		if (strcmp(client.numberplate, numberplate) == 0) {
			client.totalspent += totalCost;
		}
		fprintf(tempFile, "%-19s %-10s %-2d %-10.2f %-9s\n", client.name, client.date, client.inservice, client.totalspent, client.numberplate);
	}

	fclose(clients);
	fclose(tempFile);
	remove(CLIENTS_FILE);
	rename("temp_clients.txt", CLIENTS_FILE);

	printf("Client's total spent updated successfully.\n");
}
void RemoveClient(FILE* fclients)
{
	(void)fclients;
	FILE* tempFile;
	Client temp;
	char numberplatetoremove[10];
	int found = 0;
	int inservice_int;

	printf("Enter the number plate of the client to remove: \n");
	scanf("%9s", numberplatetoremove);

	fclients = fopen(CLIENTS_FILE, "r");
	tempFile = fopen("Temp.txt", "w");

	if (!fclients || !tempFile)
	{
		printf("Error opening files!\n");
		WriteToLog("error opening files.");
		if (fclients) fclose(fclients);
		if (tempFile) fclose(tempFile);
		return;
	}

	while (fscanf(fclients, "%19s %10s %d %f %9s",
		temp.name, temp.date, &inservice_int,
		&temp.totalspent, temp.numberplate) == 5)
	{
		temp.inservice = (inservice_int != 0);
		if (strcmp(temp.numberplate, numberplatetoremove) == 0)
		{
			found = 1;
			continue;
		}
		fprintf(tempFile, "%-19s %-10s %-2d %-10.2f %-9s\n",
			temp.name, temp.date, temp.inservice,
			temp.totalspent, temp.numberplate);
	}
	fclose(fclients);
	fclose(tempFile);

	if (found)
	{
		remove(CLIENTS_FILE);
		rename("Temp.txt", CLIENTS_FILE);
		printf("Client with number plate %s removed successfully!\n", numberplatetoremove);
		WriteToLog("client removed.");
	}
	else
	{
		remove("Temp.txt");
		printf("Client not found.\n");
		WriteToLog("client to remove not found.");
	}
}
void RemoveClientSales(FILE* fsales, FILE* fitems) {
	FILE* tempSales, * tempItems;
	Sale tempsale;
	Item tempitem;
	int foundItem = 0, foundSale = 0;
	char numberplate[10];

	printf("Please enter the number plate:\n");
	scanf("%9s", numberplate);

	fsales = fopen(SALES_FILE, "rb");
	fitems = fopen(ITEMS_FILE, "rb");
	tempSales = fopen("temp_sales.bin", "wb");
	tempItems = fopen("temp_items.bin", "wb");

	if (!fsales || !fitems || !tempSales || !tempItems) {
		printf("Error opening files!\n");
		WriteToLog("Error opening files in RemoveClientSales.");
		if (fsales) fclose(fsales);
		if (fitems) fclose(fitems);
		if (tempSales) fclose(tempSales);
		if (tempItems) fclose(tempItems);
		return;
	}

	while (fread(&tempitem, sizeof(Item), 1, fitems)) {
		if (strcmp(tempitem.numberplate, numberplate) == 0) {
			foundItem = 1;
			continue;
		}
		fwrite(&tempitem, sizeof(Item), 1, tempItems);
	}

	while (fread(&tempsale, sizeof(Sale), 1, fsales)) {
		if (strcmp(tempsale.numberplate, numberplate) == 0) {
			foundSale = 1;
			continue;
		}
		fwrite(&tempsale, sizeof(Sale), 1, tempSales);
	}

	fclose(fsales);
	fclose(fitems);
	fclose(tempSales);
	fclose(tempItems);

	if (foundItem || foundSale) {
		remove(SALES_FILE);
		rename("temp_sales.bin", SALES_FILE);
		remove(ITEMS_FILE);
		rename("temp_items.bin", ITEMS_FILE);
		printf("All data for client with number plate %s has been removed.\n", numberplate);
		WriteToLog("Client data removed.");
	}
	else {
		remove("temp_sales.bin");
		remove("temp_items.bin");
		printf("No records found for this client.\n");
		WriteToLog("No records found for client.");
	}
}
int CalculateDaysDifference(const char* saledate)
{
	struct tm sale_tm = { 0 };
	struct tm current_tm = { 0 };

	sscanf(saledate, "%d-%d-%d", &sale_tm.tm_mday, &sale_tm.tm_mon, &sale_tm.tm_year);
	sale_tm.tm_mon -= 1;
	sale_tm.tm_year -= 1900;

	time_t t = time(NULL);
	struct tm* now = localtime(&t);
	current_tm = *now;

	time_t sale_time = mktime(&sale_tm);
	time_t current_time = mktime(&current_tm);

	double difference = difftime(current_time, sale_time) / (60 * 60 * 24);
	return (int)difference;
}
void ReturnSale(FILE* sales, FILE* inventory) {
	char numberplate[10];
	char productname[25];
	int found = 0;
	Sale tempsale;

	printf("Enter number plate of the client: ");
	scanf("%9s", numberplate);
	getchar();

	printf("Enter product name to return: ");
	fgets(productname, sizeof(productname), stdin);
	productname[strcspn(productname, "\n")] = '\0';

	(void)sales; (void)inventory;
	FILE* tempfile = fopen("temp_sales.bin", "wb");
	sales = fopen(SALES_FILE, "rb");

	if (!sales || !tempfile) {
		printf("Error opening sales file!\n");
		WriteToLog("Error opening sales file.");
		if (sales) fclose(sales);
		if (tempfile) fclose(tempfile);
		return;
	}

	char currentDate[11];
	GetCurrentDate(currentDate, sizeof(currentDate));

	while (fread(&tempsale, sizeof(Sale), 1, sales)) {
		if (strcmp(tempsale.numberplate, numberplate) == 0 && _stricmp(tempsale.productname, productname) == 0) {
			int daysPassed = CalculateDaysDifference(tempsale.date);
			if (daysPassed <= 14) {
				found = 1;
				printf("Product %s has been successfully returned!\n", productname);
				WriteToLog("Product returned successfully.");

				inventory = fopen(INVENTORY_FILE, "rb+");
				if (!inventory) {
					printf("Error opening inventory file!\n");
					WriteToLog("Error opening inventory file.");
					fclose(sales);
					fclose(tempfile);
					return;
				}

				Inventoryitem product;
				long pos = 0;
				int productFound = 0;

				while (fread(&product, sizeof(Inventoryitem), 1, inventory)) {
					if (_stricmp(product.productname, productname) == 0) {
						product.quantity++; 
						productFound = 1;
						fseek(inventory, pos, SEEK_SET);
						fwrite(&product, sizeof(Inventoryitem), 1, inventory);
						fflush(inventory);
						break;
					}
					pos = ftell(inventory);
				}

				fclose(inventory);

				if (!productFound) {
					printf("Warning: Returned product not found in inventory list!\n");
					WriteToLog("Returned product not found in inventory.");
				}

				continue; 
			}
			else {
				printf("Return period exceeded! Product %s was purchased %d days ago.\n", productname, daysPassed);
				WriteToLog("Return attempt failed - exceeded 14 days.");
			}
		}
		fwrite(&tempsale, sizeof(Sale), 1, tempfile);
	}

	fclose(sales);
	fclose(tempfile);

	if (found) {
		remove(SALES_FILE);
		rename("temp_sales.bin", SALES_FILE);
	}
	else {
		remove("temp_sales.bin");
		printf("Sale not found or return period exceeded.\n");
	}
}
void ViewClientSales(FILE* sales) {
	char numberplate[10];
	int found = 0;

	printf("Please enter the client number plate: \n");
	scanf("%9s", numberplate);

	(void)sales;
	sales = fopen(SALES_FILE, "rb");
	if (!sales) {
		printf("Error opening sales file!\n");
		WriteToLog("Error opening sales file.");
		return;
	}

	Sale tempsale;

	printf("\nSales history for client [%s]:\n", numberplate);
	printf("------------------------------------------------------------\n");
	printf("| %-20s | %-10s | %-12s |\n", "Product Name", "Price", "Date");
	printf("------------------------------------------------------------\n");

	while (fread(&tempsale, sizeof(Sale), 1, sales)) {
		if (strcmp(tempsale.numberplate, numberplate) == 0) {
			printf("| %-20s | %-10.2f | %-12s |\n", tempsale.productname, tempsale.price, tempsale.date);
			found = 1;
		}
	}

	fclose(sales);

	if (!found) {
		printf("\nNo sales found for client [%s].\n", numberplate);
	}
	else {
		printf("------------------------------------------------------------\n");
	}
}
void CheckExitDate(FILE* items) {
	char numberplate[10];
	int found = 0;

	printf("Enter the number plate of the vehicle: ");
	scanf("%9s", numberplate);  

	(void)items;
	items = fopen(ITEMS_FILE, "rb");
	if (!items) {
		printf("Error opening items file!\n");
		WriteToLog("Error opening items file.");
		return;
	}

	Item tempItem;
	
	memset(&tempItem, 0, sizeof(Item));

	while (fread(&tempItem, sizeof(Item), 1, items)) {
		tempItem.carbrand[sizeof(tempItem.carbrand) - 1] = '\0';
		tempItem.color[sizeof(tempItem.color) - 1] = '\0';
		tempItem.exitdate[sizeof(tempItem.exitdate) - 1] = '\0';

		if (strcmp(tempItem.numberplate, numberplate) == 0) {
			printf("Vehicle %s (Brand: %s, Color: %s) is scheduled to exit on: %s\n",
				tempItem.numberplate, tempItem.carbrand, tempItem.color, tempItem.exitdate);
			found = 1;

			WriteToLog("Exit date check successful.");
			fclose(items);
			return;
		}
	}

	fclose(items);

	if (!found) {
		printf("Vehicle with number plate %s is not found in the system.\n", numberplate);
		WriteToLog("Exit date check failed.");
	}
}
void AdminMenu(FILE* employees, FILE* clients, FILE* items, FILE* inventory, FILE* sales) {
	int choice;
	do {
		printf("\n=== Admin Menu (Level 1) ===\n");
		printf("1. Add Employee\n");
		printf("2. Block Employee\n");
		printf("3. Add Client\n");
		printf("4. View Client Sales\n");
		printf("5. Check Vehicle Exit Date\n");
		printf("6. Return Sale\n");
		printf("7. Add Sale\n");
		printf("8. Display Inventory\n");
		printf("9. Exit\n");
		printf("Enter your choice: ");
		scanf("%d", &choice);

		switch (choice) {
		case 1: AddNewEmployee(1, employees); break;
		case 2: BlockEmployee(employees); break;
		case 3: AddClient(clients, items); break;
		case 4: ViewClientSales(sales); break; 
		case 5: CheckExitDate(items); break;
		case 6: ReturnSale(sales, inventory); break; 
		case 7: AddSaleToClient(sales, clients, inventory); break; 
		case 8: DisplayInventory(inventory); break;
		case 9: printf("Logging out...\n"); return;
		default: printf("Invalid choice. Try again.\n");
		}
	} while (choice != 9);
}
void EmployeeMenu(FILE* employees, FILE* clients, FILE* items, FILE* inventory, FILE* sales) {
	(void)employees;
	int choice;
	do {
		printf("\n=== Employee Menu (Level 2) ===\n");
		printf("1. Add Client\n");
		printf("2. View Client Sales\n");
		printf("3. Check Vehicle Exit Date\n");
		printf("4. Return Sale\n");
		printf("5. Add Sale\n");
		printf("6. Display Inventory\n");
		printf("7. Exit\n");
		printf("Enter your choice: ");
		scanf("%d", &choice);

		switch (choice) {
		case 1: AddClient(clients, items); break;
		case 2: ViewClientSales(sales); break; 
		case 3: CheckExitDate(items); break;
		case 4: ReturnSale(sales, inventory); break; 
		case 5: AddSaleToClient(sales, clients, inventory); break; 
		case 6: DisplayInventory(inventory); break;
		case 7: printf("Logging out...\n"); return;
		default: printf("Invalid choice. Try again.\n");
		}
	} while (choice != 7);
}
void PractitionerMenu(FILE* clients, FILE* items, FILE* inventory, FILE* sales) {
	int choice;
	do {
		printf("\n=== Practitioner Menu (Level 3) ===\n");
		printf("1. Add Client\n");
		printf("2. View Client Sales\n");
		printf("3. Check Vehicle Exit Date\n");
		printf("4. Display Inventory\n");
		printf("5. Exit\n");
		printf("Enter your choice: ");
		scanf("%d", &choice);

		switch (choice) {
		case 1: AddClient(clients, items); break;
		case 2: ViewClientSales(sales); break; 
		case 3: CheckExitDate(items); break;
		case 4: DisplayInventory(inventory); break;
		case 5: printf("Logging out...\n"); return;
		default: printf("Invalid choice. Try again.\n");
		}
	} while (choice != 5);
}
void showMenu() {
	FILE* employees = fopen(EMPLOYEES_FILE, "r");
	if (!employees) {
		printf("Employee file not found. Creating default admin...\n");
		createDefaultAdmin(NULL);
	}
	else {
		fclose(employees);
	}

	FILE* clients = fopen(CLIENTS_FILE, "r");
	if (!clients) {
		printf("Client file not found. Creating an empty file...\n");
		clients = fopen(CLIENTS_FILE, "w");
		if (clients) fclose(clients);
	}
	else {
		fclose(clients);
	}

	FILE* items = fopen(ITEMS_FILE, "rb");
	if (!items) {
		printf("Items file not found. Initializing vehicle database...\n");
		items = fopen(ITEMS_FILE, "wb");
		if (items) fclose(items);
	}
	else {
		fclose(items);
	}

	FILE* inventory = fopen(INVENTORY_FILE, "rb");
	if (!inventory) {
		printf("Inventory file not found. Initializing inventory database...\n");
		InitializeInventory();
	}
	else {
		fclose(inventory);
	}

	FILE* sales = fopen(SALES_FILE, "rb");
	if (!sales) {
		printf("Sales file not found. Creating sales database...\n");
		sales = fopen(SALES_FILE, "wb");
		if (sales) fclose(sales);
	}
	else {
		fclose(sales);
	}

	printf("\n=== Login ===\n");
	Employee loggedInEmployee = login(NULL);

	switch (loggedInEmployee.level) {
	case 1: AdminMenu(NULL, NULL, NULL, NULL, NULL); break;
	case 2: EmployeeMenu(NULL, NULL, NULL, NULL, NULL); break;
	case 3: PractitionerMenu(NULL, NULL, NULL, NULL); break;
	default: printf("Invalid credentials. Exiting.\n");
	}
}

int main() {
    printf("\n=====================================\n");
    printf("  Welcome to the Vehicle System!\n");
    printf("=====================================\n\n");

    showMenu();

    printf("\nExiting system. Goodbye!\n");
    return 0;
}
