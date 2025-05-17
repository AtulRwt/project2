#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // For OTP generation

#define MAX_ACCOUNTS 100

// Account structure
typedef struct {
    int accountNumber;
    char name[50];
    double balance;
    int pin;
    int failedAttempts; // Track failed login attempts
    int isLocked;       // Lock account after too many failed attempts
} Account;

// Global variables
Account accounts[MAX_ACCOUNTS];
int accountCount = 0;

// Function prototypes
void createAccount();
void displayAccountDetails();
void depositMoney();
void withdrawMoney();
void transferFunds();
int authenticateUserWith2FA(int accountNumber, int pin);
int generateOTP();
void saveAccounts();
void loadAccounts();

// Generate a random 6-digit OTP
int generateOTP() {
    srand(time(NULL));
    return rand() % 900000 + 100000; // Random number between 100000 and 999999
}

// Authenticate user with lockout and 2FA
int authenticateUserWith2FA(int accountNumber, int pin) {
    int index = -1;
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].accountNumber == accountNumber) {
            index = i;
            break;
        }
    }
    if (index == -1 || accounts[index].isLocked) {
        printf("Authentication failed or account is locked.\n");
        return -1;
    }
    if (accounts[index].pin != pin) {
        accounts[index].failedAttempts++;
        if (accounts[index].failedAttempts >= 3) {
            accounts[index].isLocked = 1;
            printf("Too many failed attempts. Account locked.\n");
        } else {
            printf("Invalid PIN. Attempts left: %d\n", 3 - accounts[index].failedAttempts);
        }
        return -1;
    }
    accounts[index].failedAttempts = 0; // Reset failed attempts on successful login

    // Generate OTP for 2FA
    int otp = generateOTP();
    printf("Your One-Time Password (OTP): %d\n", otp); // Simulate sending OTP
    printf("Enter the OTP: ");
    int enteredOTP;
    scanf("%d", &enteredOTP);
    if (enteredOTP != otp) {
        printf("Invalid OTP. Authentication failed.\n");
        return -1;
    }
    printf("2FA successful. Access granted.\n");
    return index;
}

// Create a new account
void createAccount() {
    if (accountCount >= MAX_ACCOUNTS) {
        printf("Cannot create more accounts. Limit reached.\n");
        return;
    }
    Account newAccount;
    newAccount.accountNumber = accountCount + 1;
    printf("Enter account holder's name: ");
    scanf("%s", newAccount.name);
    printf("Set a PIN (4 digits): ");
    scanf("%d", &newAccount.pin);
    newAccount.balance = 0.0;
    newAccount.failedAttempts = 0;
    newAccount.isLocked = 0;
    accounts[accountCount++] = newAccount;
    printf("Account created successfully! Your account number is %d.\n", newAccount.accountNumber);
}

// Display account details
void displayAccountDetails() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);
    int index = authenticateUserWith2FA(accountNumber, pin);
    if (index == -1) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }
    printf("\n--- Account Details ---\n");
    printf("Account Number: %d\n", accounts[index].accountNumber);
    printf("Name: %s\n", accounts[index].name);
    printf("Balance: %.2lf\n", accounts[index].balance);
}

// Deposit money
void depositMoney() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);
    int index = authenticateUserWith2FA(accountNumber, pin);
    if (index == -1) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }
    double amount;
    printf("Enter amount to deposit: ");
    scanf("%lf", &amount);
    accounts[index].balance += amount;
    printf("Deposit successful. New balance: %.2lf\n", accounts[index].balance);
}

// Withdraw money
void withdrawMoney() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);
    int index = authenticateUserWith2FA(accountNumber, pin);
    if (index == -1) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }
    double amount;
    printf("Enter amount to withdraw: ");
    scanf("%lf", &amount);
    if (amount > accounts[index].balance) {
        printf("Insufficient balance.\n");
        return;
    }
    accounts[index].balance -= amount;
    printf("Withdrawal successful. New balance: %.2lf\n", accounts[index].balance);
}

// Transfer funds
void transferFunds() {
    int senderAccount, senderPin, receiverAccount;
    printf("Enter your account number: ");
    scanf("%d", &senderAccount);
    printf("Enter your PIN: ");
    scanf("%d", &senderPin);
    int senderIndex = authenticateUserWith2FA(senderAccount, senderPin);
    if (senderIndex == -1) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }
    printf("Enter receiver's account number: ");
    scanf("%d", &receiverAccount);
    int receiverIndex = -1;
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].accountNumber == receiverAccount) {
            receiverIndex = i;
            break;
        }
    }
    if (receiverIndex == -1) {
        printf("Receiver's account not found.\n");
        return;
    }
    double amount;
    printf("Enter amount to transfer: ");
    scanf("%lf", &amount);
    if (amount > accounts[senderIndex].balance) {
        printf("Insufficient balance.\n");
        return;
    }
    accounts[senderIndex].balance -= amount;
    accounts[receiverIndex].balance += amount;
    printf("Transfer successful. New balance: %.2lf\n", accounts[senderIndex].balance);
}

// Load accounts from file
void loadAccounts() {
    FILE *file = fopen("accounts.txt", "r");
    if (file == NULL) {
        printf("No existing accounts found. Starting fresh.\n");
        return;
    }
    while (fscanf(file, "%d %s %lf %d %d %d", &accounts[accountCount].accountNumber,
                  accounts[accountCount].name, &accounts[accountCount].balance,
                  &accounts[accountCount].pin, &accounts[accountCount].failedAttempts,
                  &accounts[accountCount].isLocked) != EOF) {
        accountCount++;
    }
    fclose(file);
}

// Save accounts to file
void saveAccounts() {
    FILE *file = fopen("accounts.txt", "w");
    if (file == NULL) {
        printf("Error saving accounts.\n");
        return;
    }
    for (int i = 0; i < accountCount; i++) {
        fprintf(file, "%d %s %.2lf %d %d %d\n", accounts[i].accountNumber,
                accounts[i].name, accounts[i].balance, accounts[i].pin,
                accounts[i].failedAttempts, accounts[i].isLocked);
    }
    fclose(file);
}

// Main function
int main() {
    int choice;
    loadAccounts();
    while (1) {
        printf("\n--- Banking System Menu ---\n");
        printf("1. Create New Account\n");
        printf("2. Display Account Details\n");
        printf("3. Deposit Money\n");
        printf("4. Withdraw Money\n");
        printf("5. Transfer Funds\n");
        printf("6. Exit\n");
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
                depositMoney();
                break;
            case 4:
                withdrawMoney();
                break;
            case 5:
                transferFunds();
                break;
            case 6:
                saveAccounts();
                printf("Exiting the system. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}