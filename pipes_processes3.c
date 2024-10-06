#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <search_term>\n", argv[0]);
        exit(1);
    }

    int pipe1[2], pipe2[2]; // Two pipes for communication

    // Create the first pipe
    if (pipe(pipe1) == -1) {
        perror("pipe1 failed");
        exit(1);
    }

    pid_t pid1 = fork(); // Create the first child process

    if (pid1 < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid1 == 0) {
        // Child 1: Run "cat scores"
        close(pipe1[0]); 
        dup2(pipe1[1], STDOUT_FILENO); 
        close(pipe1[1]);

        execlp("cat", "cat", "scores", NULL); // Run 'cat scores'
        perror("execlp cat failed");
        exit(1);
    } else {
        // Create the second pipe
        if (pipe(pipe2) == -1) {
            perror("pipe2 failed");
            exit(1);
        }

        pid_t pid2 = fork(); // Create the second child process

        if (pid2 < 0) {
            perror("fork failed");
            exit(1);
        }

        if (pid2 == 0) {
            // Child 2: Run "grep <search_term>"
            close(pipe1[1]); 
            dup2(pipe1[0], STDIN_FILENO); 
            close(pipe1[0]);

            close(pipe2[0]); 
            dup2(pipe2[1], STDOUT_FILENO); 
            close(pipe2[1]);

            execlp("grep", "grep", argv[1], NULL); // Run 'grep <search_term>'
            perror("execlp grep failed");
            exit(1);
        } else {
            // Parent process runs "sort"
            close(pipe1[0]); 
            close(pipe1[1]);

            close(pipe2[1]); 
            dup2(pipe2[0], STDIN_FILENO); 
            close(pipe2[0]);

            execlp("sort", "sort", NULL); // Run 'sort'
            perror("execlp sort failed");
            exit(1);
        }
    }

    return 0;
}
