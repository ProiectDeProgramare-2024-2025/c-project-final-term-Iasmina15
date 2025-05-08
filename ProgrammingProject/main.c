#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

// ANSI Color Codes
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define CYAN "\x1b[36m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

struct Restaurant {
    char name[50];
    char menu[10][50];
    int itemCount;
} restaurants[10];

int restaurantCount = 0;

void clearScreen() {
    system(CLEAR);
}

int getValidInt(const char *prompt, int min, int max) {
    int num;
    printf("%s", prompt);
    if (scanf("%d", &num) != 1 || num < min || num > max) {
        printf(RED "Invalid input. Please enter a number between %d and %d.\n" RESET, min, max);
        while (getchar() != '\n');
        printf("Press Enter to continue...");
        getchar();
        return -1;
    }
    while (getchar() != '\n');
    return num;
}

int isValidAddress(const char *address) {
    return strstr(address, "Str.") && strstr(address, "nr.");
}

void loadMenu() {
    FILE *file = fopen("menu.txt", "r");
    if (!file) return;

    char line[50];
    int newRestaurant = 1;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "---") == 0) {
            restaurantCount++;
            newRestaurant = 1;
        } else if (restaurantCount < 10) {
            if (newRestaurant) {
                strcpy(restaurants[restaurantCount].name, line);
                restaurants[restaurantCount].itemCount = 0;
                newRestaurant = 0;
            } else if (restaurants[restaurantCount].itemCount < 10) {
                strcpy(restaurants[restaurantCount].menu[restaurants[restaurantCount].itemCount++], line);
            }
        }
    }
    fclose(file);
}

void saveOrder(char *restaurant, char *item, char *location) {
    FILE *file = fopen("orders.txt", "a");
    if (file) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char dateTime[100];
        strftime(dateTime, sizeof(dateTime), "%d/%m/%Y %H:%M:%S", t);
        fprintf(file, "[%s] %s from %s delivered to %s\n", dateTime, item, restaurant, location);
        fclose(file);
    }
}

void orderFood() {
    clearScreen();
    printf("\n********** Select a Restaurant **********\n");
    for (int i = 0; i < restaurantCount; i++)
        printf("%d. %s%s%s\n", i + 1, CYAN, restaurants[i].name, RESET);
    printf("******************************************\n");

    int choice = getValidInt("Choose a restaurant (1-4): ", 1, restaurantCount);
    if (choice == -1) return;
    int index = choice - 1;

    clearScreen();
    printf("\n********** Menu for %s%s%s **********\n", CYAN, restaurants[index].name, RESET);
    for (int i = 0; i < restaurants[index].itemCount; i++)
        printf("%d. %s%s%s\n", i + 1, GREEN, restaurants[index].menu[i], RESET);
    printf("******************************************\n");

    choice = getValidInt("Choose an item number or 0 to cancel: ", 0, restaurants[index].itemCount);
    if (choice <= 0) return;

    char location[100];
    printf("Enter delivery address (e.g., Str. Mihai Eminescu, nr. 23): ");
    fgets(location, sizeof(location), stdin);
    location[strcspn(location, "\n")] = 0;

    if (!isValidAddress(location)) {
        printf(RED "Invalid address format. Must contain \"Str.\" and \"nr.\"\n" RESET);
        printf("Press Enter to try again...");
        getchar();
        return;
    }

    saveOrder(restaurants[index].name, restaurants[index].menu[choice - 1], location);
    printf(GREEN "\nOrder confirmed: %s from %s to %s\n" RESET,
           restaurants[index].menu[choice - 1], restaurants[index].name, location);
    printf("Your food is on the way!\n");
    printf("\nPress Enter to return to the main menu...");
    getchar();
}

void showOrderHistory() {
    clearScreen();
    FILE *file = fopen("orders.txt", "r");
    int count = 0;

    printf(CYAN "\n********** Order History **********\n\n" RESET);
    if (!file) {
        printf(RED "No orders found.\n" RESET);
    } else {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            char time[30], item[50], restaurant[50], location[100];
            if (sscanf(line, "[%[^]]] %[^'] from %[^'] delivered to %[^\n]", time, item, restaurant, location) == 4) {
                printf("🕒 " CYAN "[%s]" RESET " " GREEN "%s" RESET " from " GREEN "%s" RESET " to " YELLOW "%s" RESET "\n", time, item, restaurant, location);
            } else {
                printf("%s", line);
            }
        }
        fclose(file);
    }
    printf(CYAN "************************************\n" RESET);
    printf("Press Enter to return to main menu...");
    getchar();
}

int main() {
    loadMenu();
    int choice;

    while (1) {
        clearScreen();
        printf("\n********** Welcome to Food Delivery **********\n");
        printf("1. Order Food\n");
        printf("2. View Order History\n");
        printf("3. Exit\n");
        printf("**********************************************\n");
        printf("Choose an option (1-3): ");

        if (scanf("%d", &choice) != 1) {
            printf(RED "Invalid input. Please enter a number.\n" RESET);
            while (getchar() != '\n');
            printf("Press Enter to continue...");
            getchar();
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                orderFood();
                break;
            case 2:
                showOrderHistory();
                break;
            case 3:
                printf(GREEN "\n Thank you for using our service! Goodbye!\n" RESET);
                return 0;
            default:
                printf(RED "Invalid choice. Try again.\n" RESET);
                printf("Press Enter to continue...");
                getchar();
        }
    }

    return 0;
}
