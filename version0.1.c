#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h> // SQLite library

#define DATABASE_NAME "banking.db"

// Global SQLite database handle
sqlite3 *db; ./1

// Function prototypes
void initializeDatabase();
void createAccount();
void displayAccountDetails();
void updateAccount();
void deleteAccount();
void depositMoney();
void withdrawMoney();
void transferFunds();
void viewTransactionHistory();
int authenticateUser(int accountNumber, int pin);

// Initialize the database and create tables
void initializeDatabase() {
    sqlite3_stmt *stmt;
    char *err_msg = 0;

    // Open the database (or create it if it doesn't exist)
    int rc = sqlite3_open(DATABASE_NAME, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        exit(1);
    }

    // Create the accounts table
    const char *createAccountsTable = 
        "CREATE TABLE IF NOT EXISTS accounts ("
        "accountNumber INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "balance REAL NOT NULL,"
        "pin INTEGER NOT NULL);";

    // Create the transactions table
    const char *createTransactionsTable = 
        "CREATE TABLE IF NOT EXISTS transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "accountNumber INTEGER NOT NULL,"
        "type TEXT NOT NULL,"
        "amount REAL NOT NULL,"
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";

    // Execute the SQL statements
    rc = sqlite3_exec(db, createAccountsTable, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(1);
    }

    rc = sqlite3_exec(db, createTransactionsTable, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(1);
    }
}

// Authenticate user
int authenticateUser(int accountNumber, int pin) {
    sqlite3_stmt *stmt;
    const char *query = "SELECT COUNT(*) FROM accounts WHERE accountNumber = ? AND pin = ?;";
    int authenticated = 0;

    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, accountNumber);
    sqlite3_bind_int(stmt, 2, pin);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        authenticated = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return authenticated;
}

// Create a new account
void createAccount() {
    char name[50];
    int pin;
    printf("Enter account holder's name: ");
    scanf("%s", name);
    printf("Set a PIN (4 digits): ");
    scanf("%d", &pin);

    const char *query = "INSERT INTO accounts (name, balance, pin) VALUES (?, 0.0, ?);";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, pin);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        printf("Account created successfully!\n");
    } else {
        fprintf(stderr, "Failed to create account: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

// Display account details
void displayAccountDetails() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    if (!authenticateUser(accountNumber, pin)) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }

    const char *query = "SELECT name, balance FROM accounts WHERE accountNumber = ?;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, accountNumber);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *name = sqlite3_column_text(stmt, 0);
        double balance = sqlite3_column_double(stmt, 1);

        printf("\n--- Account Details ---\n");
        printf("Account Number: %d\n", accountNumber);
        printf("Name: %s\n", name);
        printf("Balance: %.2lf\n", balance);
    } else {
        printf("Account not found.\n");
    }

    sqlite3_finalize(stmt);
}

// Update account information
void updateAccount() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    if (!authenticateUser(accountNumber, pin)) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }

    char newName[50];
    printf("Enter new name: ");
    scanf("%s", newName);

    const char *query = "UPDATE accounts SET name = ? WHERE accountNumber = ?;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, newName, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, accountNumber);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        printf("Account updated successfully.\n");
    } else {
        fprintf(stderr, "Failed to update account: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

// Delete account
void deleteAccount() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    if (!authenticateUser(accountNumber, pin)) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }

    const char *query = "DELETE FROM accounts WHERE accountNumber = ?;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, accountNumber);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        printf("Account deleted successfully.\n");
    } else {
        fprintf(stderr, "Failed to delete account: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

// Deposit money
void depositMoney() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    if (!authenticateUser(accountNumber, pin)) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }

    double amount;
    printf("Enter amount to deposit: ");
    scanf("%lf", &amount);

    const char *updateQuery = "UPDATE accounts SET balance = balance + ? WHERE accountNumber = ?;";
    const char *insertQuery = "INSERT INTO transactions (accountNumber, type, amount) VALUES (?, 'Deposit', ?);";
    sqlite3_stmt *stmt;

    // Update balance
    int rc = sqlite3_prepare_v2(db, updateQuery, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_double(stmt, 1, amount);
    sqlite3_bind_int(stmt, 2, accountNumber);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to update balance: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    // Insert transaction
    rc = sqlite3_prepare_v2(db, insertQuery, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, accountNumber);
    sqlite3_bind_double(stmt, 2, amount);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        printf("Deposit successful. New balance: %.2lf\n", amount);
    } else {
        fprintf(stderr, "Failed to log transaction: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

// View transaction history
void viewTransactionHistory() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    if (!authenticateUser(accountNumber, pin)) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }

    const char *query = "SELECT type, amount, timestamp FROM transactions WHERE accountNumber = ? ORDER BY timestamp DESC;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, accountNumber);

    printf("\n--- Transaction History ---\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *type = sqlite3_column_text(stmt, 0);
        double amount = sqlite3_column_double(stmt, 1);
        const unsigned char *timestamp = sqlite3_column_text(stmt, 2);

        printf("Type: %s, Amount: %.2lf, Timestamp: %s\n", type, amount, timestamp);
    }

    sqlite3_finalize(stmt);
}

// Main function
int main() {
    initializeDatabase();

    int choice;
    while (1) {
        printf("\n--- Banking System Menu ---\n");
        printf("1. Create New Account\n");
        printf("2. Display Account Details\n");
        printf("3. Update Account Information\n");
        printf("4. Delete Account\n");
        printf("5. Deposit Money\n");
        printf("6. Withdraw Money\n");
        printf("7. Transfer Funds\n");
        printf("8. View Transaction History\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                createAccount();
                break;
            case 2:
                displayAccountDetails();
                break;
            case 3:
                updateAccount();
                break;
            case 4:
                deleteAccount();
                break;
            case 5:
                depositMoney();
                break;
            case 6:
                // Implement withdrawMoney() similarly to depositMoney()
                break;
            case 7:
                // Implement transferFunds() similarly to depositMoney()
                break;
            case 8:
                viewTransactionHistory();
                break;
            case 9:
                sqlite3_close(db);
                printf("Exiting the system. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}