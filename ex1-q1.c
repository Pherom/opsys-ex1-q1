#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define GRADES_FILE "all_std.log"

char* getOutputFileName(int pid);
void getStudentNameAndGradeAvg(char* line, char** studentName, float* studentGradeAvg);
void processFile(char* inputFileName);

int main(int argc, char* argv[]) {
    processFile(argv[1]);
}

char* getOutputFileName(int pid) {
    char* outputFileName = (char*)malloc((int)((ceil(log10(pid))+6)*sizeof(char)));
    if (outputFileName == NULL) {
        perror("Failed to allocate memory for output file name");
    }
    else {
        sprintf(outputFileName, "%d.temp", pid);
    }
    return outputFileName;
}

void getStudentNameAndGradeAvg(char* line, char** studentName, float* studentGradeAvg) {
    size_t currentGrade;
    size_t gradeCount = 0;
    size_t gradeSum = 0;
    char* token;

    *studentName = strtok(line, " ");
    token = strtok(NULL, " ");
    while (token != NULL) { 
        gradeSum += atoi(token);
        gradeCount++;
        token = strtok(NULL, " ");
    }

    *studentGradeAvg = (float)gradeSum / gradeCount;
}

void processFile(char* inputFileName) {
    FILE* inputFile = fopen(inputFileName, "r");
    FILE* outputFile;
    char* outputFileName;
    char* currentLine = NULL;
    char* studentName;
    float studentGradeAvg;
    size_t studentCount = 0;
    size_t currentLineLength;
    int pid = getpid();

    if (inputFile == NULL) {
        perror("Failed to open input file for reading");
    }
    else {
        outputFileName = getOutputFileName(pid);

        if (outputFileName != NULL) {
            outputFile = fopen(outputFileName, "w");

            if (outputFile == NULL) {
                perror("Failed to open output file for writing");
            }
            else {
                while(getline(&currentLine, &currentLineLength, inputFile) != -1) {
                    getStudentNameAndGradeAvg(currentLine, &studentName, &studentGradeAvg);
                    if (studentCount != 0) {
                        fwrite("\n", sizeof(char), 1, outputFile);
                    }
                    fprintf(outputFile, "%s %.1f", studentName, studentGradeAvg);
                    free(currentLine);
                    studentCount++;
                }
                fclose(outputFile);
            }

            free(outputFileName);
        }

        fclose(inputFile);
    }

    fprintf(stderr, "process: %d file: %s number of students: %zu", pid, inputFileName, studentCount);
}

