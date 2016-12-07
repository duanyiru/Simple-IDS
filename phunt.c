
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/resource.h>
#define CONFIG 100

// Global variables.
char logFile[100];
char configFile[100];
int configRows = 0;
FILE* logFilePointer;

// Will hold an array of restrictions as strings (i.e. "kill user badguy")
char configuration[CONFIG][40];
int test;

// Function declarations.
void logEntry(char* entry);
void parseCommandLine(int argc, char** argv);
void parseConfigFile();
void loopThroughProcFolders();
void checkAgainstConfig(char* statusFileName, int pid);
void getStatusFileInfo(char* statusFileName, char** username, int* memory);
void getPathInfo(char* processFolderName, char** path);
void checkUser(char* action, char* param, char* username, int pid);
void checkPath(char* action, char* param, char* path, int pid);
void checkMemory(char* action, char* param, int memory, int pid);
void takeAction(char* action, int pid);

// Main method.
int main(int argc, char** argv) {
    test = 0;
    parseCommandLine(argc, argv);

    logFilePointer = fopen(logFile, "w");
    char entry[40] = "Phunt program has started up, PID: ";
    int pid = getpid();
    char pidString[5];
    sprintf(pidString, "%d", pid);
    strcat(entry, pidString);
    logEntry(entry);

    parseConfigFile();
    loopThroughProcFolders();

    logEntry("Program terminating");
    fclose(logFilePointer);

    return 0;
}

// Called to print to log file.
void logEntry(char* entry) {
    time_t currentTime = time(NULL);
    char timeString[30];
    strcpy(timeString, asctime(localtime(&currentTime)));
    timeString[strlen(timeString) - 1] = '\0';

    fprintf(logFilePointer, "%s : ", timeString);
    fprintf(logFilePointer, "%s\n", entry);
}

// Parses the command line options and sets the config and log files accordingly.
void parseCommandLine(int argc, char** argv) {
    int option;
    strcpy(logFile, "/var/log/phunt.log"); // Default log file.
    strcpy(configFile, "/etc/phunt.conf"); // Default config file.

    while((option = getopt(argc, argv, "l:c:")) != -1) {
        switch(option) {
            case 'l':
                strcpy(logFile, optarg);
                break;
            case 'c':
                strcpy(configFile, optarg);
                break;
            default:
                break;
        }
    }
}

// Determines the program instructions by parsing the config file.
void parseConfigFile() {
    // Open the config file for reading.
    FILE *file;
    file = fopen(configFile, "r");

    char* line = NULL;
    size_t length = 0;
    size_t size;

    for(int i = 0; i < CONFIG; i++) {
        memset(configuration[i], '\0', 40);
    }

    // Go through each line of the config file.
    while((size = getline(&line, &length, file)) != -1) {
        // Ignore lines that are commented out or blank
        if((line[0] != '#') && (line[0] != '\n')) {
            strcpy(configuration[configRows], line);
            configRows++;
        }
    }

    logEntry("Config file parsed successfully");

    fclose(file);
}

// Meat of the program. Loops through PID folders in /proc,
// calls another function to check whether action needs
// to be taken on that process. If so, carries specified action out.
void loopThroughProcFolders() {
    DIR* directory;
    struct dirent* entry;

    directory = opendir("/proc");
    while(entry = readdir(directory)) {
        if(isdigit(entry->d_name[0])) {
            int pid = atoi(entry->d_name);
            char processFolderName[20] = "/proc/";
            strcat(processFolderName, entry->d_name);

            checkAgainstConfig(processFolderName, pid);
        }
    }
}

void checkAgainstConfig(char* processFolderName, int pid) {
    char statusFileName[30];
    strcpy(statusFileName, processFolderName);
    strcat(statusFileName, "/status");

    char* username = (char*)malloc(20);
    memset(username, '\0', 20);
    int* memory = (int*)malloc(5);
    char* path = (char*)malloc(200);
    memset(path, '\0', 200);
    getStatusFileInfo(statusFileName, &username, memory);
    getPathInfo(processFolderName, &path);
    if(*memory > test) test = *memory;

    for(int i = 0; i < configRows; i++) {
        char action[10], type[10], param[100], config[40];
        memset(action, '\0', 10);
        memset(type, '\0', 10);
        memset(param, '\0', 100);
        memset(config, '\0', 40);

        strcpy(config, configuration[i]);

        strcpy(action, strtok(config, " \t\n"));
        strcpy(type, strtok(NULL, " \t\n"));
        strcpy(param, strtok(NULL, " \t\n"));

        if(strcmp(type, "user") == 0) {
            checkUser(action, param, username, pid);
        } else if(strcmp(type, "path") == 0) {
            checkPath(action, param, path, pid);
        } else if(strcmp(type, "memory") == 0) {
            checkMemory(action, param, *memory, pid);
        }
    }

    free(username);
    free(memory);
    free(path);
}

void getStatusFileInfo(char* statusFileName, char** username, int* memory) {
    uid_t uid = 0;
    FILE* file;
    file = fopen(statusFileName, "r");
    char* line = (char*)malloc(50);
    size_t length = 0;
    size_t size;

    // Go through each line of the status file.
    int reachedUID = 0;
    int reachedMemory = 0;
    while((size = getline(&line, &length, file)) != -1) {
        char* word = (char*)malloc(15);
        word = strtok(line, " \t\n");
        if(strcmp(word, "Uid:") == 0) reachedUID = 1;
        if(strcmp(word, "VmSize:") == 0) reachedMemory = 1;
        while(word != NULL) {
            word = strtok(NULL, " \t\n");
            if(reachedUID == 1) {
                uid = atoi(word);
                reachedUID = 0;
            } else if(reachedMemory == 1) {
                *memory = (atoi(word) / 1000);
                reachedMemory = 0;
            }
        }
        free(word);
    }

    free(line);
    fclose(file);
    struct passwd *password = getpwuid(uid);
    strcpy(*username, password->pw_name);
}

void getPathInfo(char* processFolderName, char** path) {
    char symbolicLinkName[30];
    strcpy(symbolicLinkName, processFolderName);
    strcat(symbolicLinkName, "/exe");

    readlink(symbolicLinkName, *path, 200);
}

void checkUser(char* action, char* param, char* username, int pid) {
    if(strcmp(param, username) == 0) {
        takeAction(action, pid);
    }
}

void checkPath(char* action, char* param, char* path, int pid) {
    if(strcmp(param, path) == 0) {
        takeAction(action, pid);
    }
}

void checkMemory(char* action, char* param, int memory, int pid) {
    int restriction = atoi(param);
    if(restriction < memory) {
        takeAction(action, pid);
    }
}

void takeAction(char* action, int pid) {
    char entry[65];
    char pidString[5];
    sprintf(pidString, "%d", pid);
    if(strcmp(action, "kill") == 0) {
        int status = kill(pid, SIGKILL);
        if(status > -1) {
            strcpy(entry, "Killed process with PID ");
            strcat(entry, pidString);
        } else {
            strcpy(entry, "Something went wrong. Failed to kill process with PID ");
            strcat(entry, pidString);
        }
    } else if(strcmp(action, "suspend") == 0) {
        int status = kill(pid, SIGTSTP);
        if(status > -1) {
            strcpy(entry, "Suspended process with PID ");
            strcat(entry, pidString);
        } else {
            strcpy(entry, "Something went wrong. Failed to suspend process with PID ");
            strcat(entry, pidString);
        }
    } else if(strcmp(action, "nice") == 0) {
        int status = setpriority(PRIO_PROCESS, pid, -20);
        if(status > -1) {
            strcpy(entry, "Niced process with PID ");
            strcat(entry, pidString);
        } else {
            strcpy(entry, "Something went wrong. Failed to nice process with PID ");
            strcat(entry, pidString);
        }
    }
    logEntry(entry);
}
