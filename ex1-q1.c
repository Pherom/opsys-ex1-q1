#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <signal.h>

#define GRADES_FILE "all_std.log"

char* getOutputFileName(int pid);
void getStdNameAndGradeAvg(char* line, char** std_name, float* grade_avg);
void processFile(char* in_file_name);
void disableZombies();

int main(int argc, char* argv[]) {
    int pid;

    disableZombies();

    for (int i = 1; i < argc; i++) {
        pid = fork();
        if (pid == 0) {
            processFile(argv[i]);
            break;
        }
    }
}

void disableZombies() {
    struct sigaction act;
    sigemptyset (&act.sa_mask);
    act.sa_handler = SIG_IGN;
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, NULL);
}

char* getOutputFileName(int pid) {
    char* out_file_name = (char*)malloc((int)((ceil(log10(pid))+6)*sizeof(char)));
    if (out_file_name == NULL) {
        perror("Failed to allocate memory for output file name");
    }
    else {
        sprintf(out_file_name, "%d.temp", pid);
    }
    return out_file_name;
}

void getStdNameAndGradeAvg(char* line, char** std_name, float* grade_avg) {
    size_t curr_grade;
    size_t grade_count = 0;
    size_t grade_sum = 0;
    char* token;

    *std_name = strtok(line, " ");
    token = strtok(NULL, " ");
    while (token != NULL) { 
        grade_sum += atoi(token);
        grade_count++;
        token = strtok(NULL, " ");
    }

    *grade_avg = (float)grade_sum / grade_count;
}

void processFile(char* in_file_name) {
    FILE* in_file = fopen(in_file_name, "r");
    FILE* out_file;
    char* out_file_name;
    char* curr_line = NULL;
    char std_name[11];
    float grade_avg;
    size_t curr_line_len;
    size_t std_count = 0;
    int pid = getpid();

    if (in_file == NULL) {
        perror("Failed to open input file for reading");
    }
    else {
        out_file_name = getOutputFileName(pid);

        if (out_file_name != NULL) {
            out_file = fopen(out_file_name, "w");

            if (out_file == NULL) {
                perror("Failed to open output file for writing");
            }
            else {
                while(getline(&curr_line, &curr_line_len, in_file) != -1) {
                    getStdNameAndGradeAvg(curr_line, &std_name, &grade_avg);
                    if (std_count != 0) {
                        fwrite("\n", sizeof(char), 1, out_file);
                    }
                    fprintf(out_file, "%s %.1f", std_name, grade_avg);
                    free(curr_line);
                    std_count++;
                }
                fclose(out_file);
            }

            free(out_file_name);
        }

        fclose(in_file);
    }

    fprintf(stderr, "process: %d file: %s number of students: %zu\n", pid, in_file_name, std_count);
}

