#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

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

        // Converts year from string to int
        int year = atoi(yearToken);
        // Converts ratingVal from a string to a float
        double ratingVal = strtof(ratingValToken, NULL);

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
        if (strncmp(entry->d_name, dirBegin, strlen(dirBegin)) == 0 && strcmp(entry->d_name + strlen(entry->d_name) - strlen(dirEnd), dirEnd) == 0) {
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
        freeMovieList(head);
    }
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