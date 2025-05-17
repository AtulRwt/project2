#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // For pow() function

#define MAX_ACCOUNTS 100
#define MAX_STACK_SIZE 5
#define MAX_QUEUE_SIZE 10

// Account structure
typedef struct {
    int accountNumber;
    char name[50];
    double balance;
    int pin;
} Account;

// Transaction node for linked list
typedef struct TransactionNode {
    int accountNumber;
    char type[20]; // "Deposit", "Withdraw", "Transfer"
    double amount;
    struct TransactionNode *next;
} TransactionNode;

// Stack for operation log
typedef struct {
    int operationLog[MAX_STACK_SIZE];
    int top;
} Stack;

// Queue for pending transactions
typedef struct {
    int queue[MAX_QUEUE_SIZE];
    int front, rear;
} Queue;

// Global variables
Account accounts[MAX_ACCOUNTS];
int accountCount = 0;
TransactionNode *transactionHistoryHead = NULL; // Linked list for transaction history
Stack operationStack;
Queue pendingTransactions;

// Function prototypes
void initializeStack(Stack *stack);
void initializeQueue(Queue *queue);
void pushOperation(int operation);
void enqueuePendingTransaction(int accountNumber);
void processPendingTransactions();
void addTransaction(int accountNumber, const char *type, double amount);
void viewTransactionHistory();
void loadAccounts();
void saveAccounts();
void createAccount();
void displayAccountDetails();
void updateAccount();
void deleteAccount();
void depositMoney();
void withdrawMoney();
void transferFunds();
int authenticateUser(int accountNumber, int pin);
void sortAccountsByBalance();
void calculateCompoundInterest();
void detectSuspiciousActivity();

// Initialize stack
void initializeStack(Stack *stack) {
    stack->top = -1;
}

// Initialize queue
void initializeQueue(Queue *queue) {
    queue->front = queue->rear = -1;
}

// Push operation to stack
void pushOperation(int operation) {
    if (operationStack.top >= MAX_STACK_SIZE - 1) {
        printf("Operation log full. Cannot log more operations.\n");
        return;
    }
    operationStack.operationLog[++operationStack.top] = operation;
}

// Enqueue pending transaction
void enqueuePendingTransaction(int accountNumber) {
    if ((pendingTransactions.rear + 1) % MAX_QUEUE_SIZE == pendingTransactions.front) {
        printf("Pending transaction queue is full.\n");
        return;
    }
    if (pendingTransactions.front == -1) {
        pendingTransactions.front = pendingTransactions.rear = 0;
    } else {
        pendingTransactions.rear = (pendingTransactions.rear + 1) % MAX_QUEUE_SIZE;
    }
    pendingTransactions.queue[pendingTransactions.rear] = accountNumber;
}

// Process pending transactions
void processPendingTransactions() {
    while (pendingTransactions.front != -1) {
        int accountNumber = pendingTransactions.queue[pendingTransactions.front];
        printf("Processing pending transaction for account %d...\n", accountNumber);
        if (pendingTransactions.front == pendingTransactions.rear) {
            pendingTransactions.front = pendingTransactions.rear = -1;
        } else {
            pendingTransactions.front = (pendingTransactions.front + 1) % MAX_QUEUE_SIZE;
        }
    }
}

// Add transaction to linked list
void addTransaction(int accountNumber, const char *type, double amount) {
    TransactionNode *newNode = (TransactionNode *)malloc(sizeof(TransactionNode));
    newNode->accountNumber = accountNumber;
    strcpy(newNode->type, type);
    newNode->amount = amount;
    newNode->next = transactionHistoryHead;
    transactionHistoryHead = newNode;
}

// View transaction history
void viewTransactionHistory() {
    TransactionNode *current = transactionHistoryHead;
    if (current == NULL) {
        printf("No transactions found.\n");
        return;
    }

    printf("\n--- Transaction History ---\n");
    while (current != NULL) {
        printf("Account: %d, Type: %s, Amount: %.2lf\n",
               current->accountNumber, current->type, current->amount);
        current = current->next;
    }
}

// Load accounts from file
void loadAccounts() {
    FILE *file = fopen("accounts.txt", "r");
    if (file == NULL) {
        printf("No existing accounts found. Starting fresh.\n");
        return;
    }

    while (fscanf(file, "%d %s %lf %d", &accounts[accountCount].accountNumber,
                  accounts[accountCount].name, &accounts[accountCount].balance,
                  &accounts[accountCount].pin) != EOF) {
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
        fprintf(file, "%d %s %.2lf %d\n", accounts[i].accountNumber,
                accounts[i].name, accounts[i].balance, accounts[i].pin);
    }
    fclose(file);
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

    accounts[accountCount++] = newAccount;
    printf("Account created successfully! Your account number is %d.\n", newAccount.accountNumber);
    pushOperation(1); // Log "Create Account" operation
}

// Display account details
void displayAccountDetails() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    int index = authenticateUser(accountNumber, pin);
    if (index == -1) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }

    printf("\n--- Account Details ---\n");
    printf("Account Number: %d\n", accounts[index].accountNumber);
    printf("Name: %s\n", accounts[index].name);
    printf("Balance: %.2lf\n", accounts[index].balance);
    pushOperation(2); // Log "Display Account" operation
}

// Authenticate user
int authenticateUser(int accountNumber, int pin) {
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].accountNumber == accountNumber && accounts[i].pin == pin) {
            return i;
        }
    }
    return -1;
}

// Update account information
void updateAccount() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    int index = authenticateUser(accountNumber, pin);
    if (index == -1) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }

    printf("Enter new name: ");
    scanf("%s", accounts[index].name);
    printf("Account updated successfully.\n");
    pushOperation(6); // Log "Update Account" operation
}

// Delete account
void deleteAccount() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    int index = authenticateUser(accountNumber, pin);
    if (index == -1) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }

    for (int i = index; i < accountCount - 1; i++) {
        accounts[i] = accounts[i + 1];
    }
    accountCount--;
    printf("Account deleted successfully.\n");
    pushOperation(7); // Log "Delete Account" operation
}

// Deposit money
void depositMoney() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    int index = authenticateUser(accountNumber, pin);
    if (index == -1) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }

    double amount;
    printf("Enter amount to deposit: ");
    scanf("%lf", &amount);

    accounts[index].balance += amount;
    printf("Deposit successful. New balance: %.2lf\n", accounts[index].balance);
    addTransaction(accountNumber, "Deposit", amount);
    pushOperation(3); // Log "Deposit" operation
}

// Withdraw money
void withdrawMoney() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    int index = authenticateUser(accountNumber, pin);
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
    addTransaction(accountNumber, "Withdraw", amount);
    pushOperation(4); // Log "Withdraw" operation
}

// Transfer funds
void transferFunds() {
    int senderAccount, senderPin, receiverAccount;
    printf("Enter your account number: ");
    scanf("%d", &senderAccount);
    printf("Enter your PIN: ");
    scanf("%d", &senderPin);

    int senderIndex = authenticateUser(senderAccount, senderPin);
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
    addTransaction(senderAccount, "Transfer Out", amount);
    addTransaction(receiverAccount, "Transfer In", amount);
    pushOperation(5); // Log "Transfer" operation
}

// Sort accounts by balance
void sortAccountsByBalance() {
    for (int i = 0; i < accountCount - 1; i++) {
        for (int j = 0; j < accountCount - i - 1; j++) {
            if (accounts[j].balance > accounts[j + 1].balance) {
                Account temp = accounts[j];
                accounts[j] = accounts[j + 1];
                accounts[j + 1] = temp;
            }
        }
    }
    printf("Accounts sorted by balance.\n");
}

// Calculate compound interest
void calculateCompoundInterest() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    int index = authenticateUser(accountNumber, pin);
    if (index == -1) {
        printf("Authentication failed. Invalid account number or PIN.\n");
        return;
    }

    double rate, years;
    printf("Enter annual interest rate (e.g., 0.05 for 5%%): ");
    scanf("%lf", &rate);
    printf("Enter number of years: ");
    scanf("%lf", &years);

    double principal = accounts[index].balance;
    double result = principal * pow(1 + rate, years);
    printf("Compound interest after %.0lf years: %.2lf\n", years, result);
}

// Detect suspicious activity (graph traversal)
void detectSuspiciousActivity() {
    int graph[MAX_ACCOUNTS][MAX_ACCOUNTS] = {0};
    int visited[MAX_ACCOUNTS] = {0};

    // Build a simple graph of transfers
    for (TransactionNode *node = transactionHistoryHead; node != NULL; node = node->next) {
        if (strcmp(node->type, "Transfer Out") == 0) {
            int sender = node->accountNumber;
            int receiver = -1;
            for (TransactionNode *nextNode = node->next; nextNode != NULL; nextNode = nextNode->next) {
                if (strcmp(nextNode->type, "Transfer In") == 0 && nextNode->amount == node->amount) {
                    receiver = nextNode->accountNumber;
                    break;
                }
            }
            if (receiver != -1) {
                graph[sender][receiver] = 1;
            }
        }
    }

    // BFS to detect cycles (suspicious activity)
    for (int i = 0; i < accountCount; i++) {
        if (!visited[i]) {
            int queue[MAX_ACCOUNTS], front = 0, rear = 0;
            queue[rear++] = i;
            visited[i] = 1;

            while (front < rear) {
                int current = queue[front++];
                for (int j = 0; j < accountCount; j++) {
                    if (graph[current][j] && !visited[j]) {
                        visited[j] = 1;
                        queue[rear++] = j;
                    } else if (graph[current][j] && visited[j]) {
                        printf("Suspicious activity detected between accounts %d and %d.\n", current, j);
                    }
                }
            }
        }
    }
}

// Main function
int main() {
    int choice;
    initializeStack(&operationStack);
    initializeQueue(&pendingTransactions);
    loadAccounts();

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
        printf("9. Process Pending Transactions\n");
        printf("10. Sort Accounts by Balance\n");
        printf("11. Calculate Compound Interest\n");
        printf("12. Detect Suspicious Activity\n");
        printf("13. Exit\n");
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
                withdrawMoney();
                break;
            case 7:
                transferFunds();
                break;
            case 8:
                viewTransactionHistory();
                break;
            case 9:
                processPendingTransactions();
                break;
            case 10:
                sortAccountsByBalance();
                break;
            case 11:
                calculateCompoundInterest();
                break;
            case 12:
                detectSuspiciousActivity();
                break;
            case 13:
                saveAccounts();
                printf("Exiting the system. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}