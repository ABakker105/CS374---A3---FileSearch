#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

/*
    Program Name: bakkera_assignment3.c
    Author: Allessandra Bakker
    Email: bakkera@oregonstate.edu

    Description: This program reads directory entries, finds a file in the current directory based on user 
    specified criteria, reads and processes the data in the chosen file, creates a directory, and creates new files 
    in the newly created directory and writes processed data to these files. 
*/

/*
    Struct: movie

    title - Dynamically allocated string for the movie title.
    year - Integer representing the year a movie was released.
    languages - Dynamically allocated string that contains the language or languages in which the movie was released.
    ratingVal - Double that represnts the movie's rating.
    next - Pointer to the next movie in the linked list.

    Description: Holds info for one movie.
*/
struct movie {
    char* title;
    int year;
    char* languages;
    double ratingVal;
    struct movie* next;
};

/* 
    Function: createMovie

    Parameters: 
    title - String containing the title of the movie.
    year - Integer for the year of release of the movie.
    languages - String containing the language/languages in which the movie was released.
    ratingVal - Double rating value of the movie.

    Description: Creates a movie structure using dynamic memory allocation.
    
    Returns:
    (currMovie) - Pointer to the new movie struct
*/
struct movie* createMovie(char* title, int year, char* languages, double ratingVal) {
    struct movie* currMovie = malloc(sizeof(struct movie));

    // Allocate memory for the title in the structure
    currMovie->title = calloc(strlen(title) + 1, sizeof(char));

    // Copy the value of title into the title in the structure
    strcpy(currMovie->title, title);

    // Allocate memory for the langauges in the structure
    currMovie->languages = calloc(strlen(languages) + 1, sizeof(char));

    // Copy the value of langauges into the langauges in the structure
    strcpy(currMovie->languages, languages);

    // Sets the year
    currMovie->year = year;
    currMovie->ratingVal = ratingVal;

    // Sets the next node to NULL in the newly created movie entry
    currMovie->next = NULL;

    return currMovie;
}

/*
 * Function: processMovieFile
 *   Opens a file, reads and prints each line, and builds a linked list of movies.
 *   filePath: path to the file
 * 
 *  This function shows sample code that opens a file, then in a loop reads and prints each line in that file.
 *  You can use this code however you want in your Prog Assignment 2.
 * 
 * Returns:
 * (head) - Pointer to the first movie in the linked list.
 */
 struct movie* processMovieFile(char* filePath){
    char *currLine = NULL;
    size_t len = 0;
    int movieCount = 0;
    char* savePtr;

    struct movie* head = NULL;
    struct movie* tail = NULL;

    // Open the specified file for reading only
    FILE *movieFile = fopen(filePath, "r");

    // Skips the header line
    getline(&currLine, &len, movieFile);

    // Read the file line by line
    while(getline(&currLine, &len, movieFile) != -1)
    {
        movieCount++;
        
        char* titleToken = strtok_r(currLine, ",", &savePtr);
        char* yearToken = strtok_r(NULL, ",", &savePtr);
        char* langToken = strtok_r(NULL, ",", &savePtr);
        char* ratingValToken = strtok_r(NULL, ",", &savePtr);

        // Converts year from string to int, only if yearToken is valid
        int year = 0;
        if (yearToken != NULL) {
            year = atoi(yearToken);
        } else {
            continue;
        }

        // Converts ratingVal from a string to a float
        double ratingVal = 0.0;
        if (ratingValToken != NULL) {
            ratingVal = strtof(ratingValToken, NULL);
        } else {
            continue;
        }

        // Create movie struct
        struct movie* newMovie = createMovie(titleToken, year, langToken, ratingVal);

        // Adds values to linked list
        if (head == NULL) {
            head = newMovie;
            tail = newMovie;
        } else {
            tail->next = newMovie;
            tail = newMovie;
        }
    }

    // Free the memory allocated by getline for currLine
    free(currLine);
    // Close the file
    fclose(movieFile);
    printf("Now processing the chosen file named %s\n", filePath);
    printf("\n");
    return head;
}

/* 
    Function: processMovieData

    Parameters: 
    listHead: Pointer to the head of the linked list
    onid: String representing my ONID 

    Description: Creates a new directory named with my ONID and a random number. Also iterates 
    through the linked list of movies and writes each movie title into a file 
    YYYY.txt inside the directory. Each file contains titles of the movies released in that year.
*/
void processMovieData(struct movie* listHead, const char* onid) {
    srandom(time(NULL));
    // Generates random number between 0 and 99999
    int randVal;
    do {
        randVal = random();
    } while (randVal < 0 || randVal > 99999);

    // Builds directory name using my ONID and a random number 
    char dirName[256];
    sprintf(dirName, "%s.movies.%d", onid, randVal);

    // Creates new directory with permissions rwxr-x---
    mkdir(dirName, 0750);
    chmod(dirName, 0750);

    printf("Created directory with name %s\n", dirName);

    struct movie* curr = listHead;
    while (curr != NULL) {
        // Builds the file path for the current movie's year
        char filePath[500];
        sprintf(filePath, "%s/%d.txt", dirName, curr->year);

        // Opens file for writing, creates if needed, and appends to the end (permissions: rw-r-----)
        int fd = open(filePath, O_WRONLY | O_CREAT | O_APPEND, 0640);
        if (fd == -1) {
            printf("Error opening file");
            curr = curr->next;
            continue;
        }

        char line[1000];
        sprintf(line, "%s\n", curr->title);
        // Writes the title to the file
        int howMany = write(fd, line, strlen(line));

        close(fd);
        // Moves to the next movie in the list
        curr = curr->next;
    }
}

/*
    Function: freeMovieList

    Parameters: 
    listHead - The head of the movie linked list 

    Description: Frees all the memory used for each movie
*/
void freeMovieList(struct movie* listHead) {
    struct movie* curr;
    
    while (listHead != NULL) {
        curr = listHead;
        listHead = listHead->next;
        free(curr->title);
        free(curr->languages);
        free(curr);
    }
}

/* 
    Function: processLargestFile

    Description: Searches the current directory for the largest CSV file whose name starts with 
    "movies_" and ends with "csv".  
*/
void processLargestFile() {
    DIR* currDir;
    struct dirent *entry;
    struct stat dirStat;

    char dirBegin[] = "movies_";
    char dirEnd[] = ".csv";
    char largestFile[256] = "";
    int largestSize = 0;

    // Open the current directory
    currDir = opendir(".");

    // Go through all the entries
    while((entry = readdir(currDir)) != NULL) {
        // Checks if file name starts with movies_ and ends with .csv
        if (strncmp(entry->d_name, dirBegin, strlen(dirBegin)) == 0 && strlen(entry->d_name) >= strlen(dirEnd) && strcmp(entry->d_name + strlen(entry->d_name) - strlen(dirEnd), dirEnd) == 0) {
            stat(entry->d_name, &dirStat);

            // Checks if the file is larger than the current largest
            if (largestSize == 0 || dirStat.st_size > largestSize) {
                largestSize = dirStat.st_size;
                strcpy(largestFile, entry->d_name);
            }
        }
    }

    closedir(currDir);

    if (largestSize > 0) {
        struct movie* head = processMovieFile(largestFile);
        processMovieData(head, "bakkera");
        freeMovieList(head);
    }
}

/* 
    Function: processSmallestFile

    Description: Searches the current directory for the smallest CSV file whose name starts with 
    "movies_" and ends with "csv".  
*/
void processSmallestFile() {
    DIR* currDir;
    struct dirent *entry;
    struct stat dirStat;

    char dirBegin[] = "movies_";
    char dirEnd[] = ".csv";
    char smallestFile[256] = "";
    int smallestSize = 0;

    // Open the current directory
    currDir = opendir(".");

    // Go through all the entries
    while((entry = readdir(currDir)) != NULL) {
        // Checks if file name starts with movies_ and ends with .csv
        if (strncmp(entry->d_name, dirBegin, strlen(dirBegin)) == 0 && strlen(entry->d_name) >= strlen(dirEnd) && strcmp(entry->d_name + strlen(entry->d_name) - strlen(dirEnd), dirEnd) == 0) {
            stat(entry->d_name, &dirStat);

            // Checks if the file is smaller than the current smallest
            if (smallestSize == 0 || dirStat.st_size < smallestSize) {
                smallestSize = dirStat.st_size;
                strcpy(smallestFile, entry->d_name);
            }
        }
    }

    closedir(currDir);

    if (smallestSize > 0) {
        struct movie* head = processMovieFile(smallestFile);
        processMovieData(head, "bakkera");
        freeMovieList(head);
    }
}

/* 
    Function: processSearchedFile

    Description: Prompts the user to enter the exact name of a CSV file to process. Checks 
    if this file exists in the current directory. If the file is not found, the program  
    writes an error message and agains give the user the 3 choices about picking a file.
*/
void processSearchedFile() {
    struct stat dirStat;

    char fileName[256] = "";
    int found = 0;

    while (!found) {
        printf("Enter the complete file name: ");
        scanf("%s", fileName);

        // Checks if the file being searched exists
        if (stat(fileName, &dirStat) == 0) {
            struct movie* head = processMovieFile(fileName);
            processMovieData(head, "bakkera");
            freeMovieList(head);
            found = 1;
        } else {
            printf("The file %s was not found. Try again", fileName);

            int choice;
            printf("\n\n");
            printf("Which file you want to process?\n");
            printf("Enter 1 to pick the largest file\n");
            printf("Enter 2 to pick the smallest file\n");
            printf("Enter 3 to specify the name of a file\n");
            printf("\n");
            printf("Enter a choice from 1 to 3: ");
            scanf("%d", &choice);

            if (choice == 1) {
                processLargestFile();
                found = 1;
            } else if (choice == 2) {
                processSmallestFile();
                found = 1;
            } else if (choice == !3) {
                printf("You entered an incorrect choice. Try again.\n");
            }
        }
    }
}

/*
    Function: main

    Description: Displays a menu of interactive choices to the user 
    and calls functions based on the user's choice
 */
int main() {
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