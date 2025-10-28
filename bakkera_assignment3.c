#include <stdio.h>
#include <stdlib.h>

void processLargestFile() {
    printf("choice 1 worked!\n");
}

void processSmallestFile() {
    printf("choice 2 worked!");
}

void processSearchedFile() {
    printf("choice 3 worked!");
}

void main() {
    int choice1;
    do {
        printf("1. Select file to process\n");
        printf("2. Exit the program\n");
        printf("\n");
        printf("Enter a choice 1 or 2: ");
        scanf("%d", &choice1);

        if (choice1 == 1) {
            int choice2;
            printf("\n");
            printf("Which file you want to process?\n");
            printf("Enter 1 to pick the largest file\n");
            printf("Enter 2 to pick the smallest file\n");
            printf("Enter 3 to specify the name of a file\n");
            printf("\n");
            printf("Enter a choice from 1 to 3: ");
            scanf("%d", &choice2);

            if (choice2 == 1) {
                processLargestFile();
            } else if (choice2 == 2) {
                processSmallestFile();
            } else if (choice2 == 3) {
                processSearchedFile();
            } else {
                printf("You entered an incorrect choice. Try again.\n");
            }

        } else if (choice1 == 2) {
            break;
        } else {
            printf("You entered an incorrect choice. Try again.\n");
        }
    } while (choice1 != 2);
}