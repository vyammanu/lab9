#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <string.h>


pid_t child_pid = -1; // Global variable to store the child's PID

// Signal handler for SIGINT
void handle_sigint(int signo) {
    printf("Parent process [%ld] received SIGINT.\n", (long)getpid());
    if (child_pid > 0) {
        kill(child_pid, SIGINT); // Send SIGINT to the child process
    }
}

// Signal handler for SIGTSTP
void handle_sigtstp(int signo) {
    printf("Parent process [%ld] received SIGTSTP.\n", (long)getpid());
    if (child_pid > 0) {
        kill(child_pid, SIGTSTP); // Send SIGTSTP to the child process
    }
}

// Signal handler for SIGQUIT
void handle_sigquit(int signo) {
    printf("Parent process [%ld] received SIGQUIT, terminating.\n", (long)getpid());
    if (child_pid > 0) {
        kill(child_pid, SIGTERM); // Terminate the child process
    }
    exit(0); // Exit the parent process
}

int main(int argc, char **argv) {
    int status;

    if (argc < 2) {
        printf("Usage: %s <command> [args]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Set up the signal handlers
    struct sigaction sa_int, sa_tstp, sa_quit;

    // Setup for SIGINT
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    // Setup for SIGTSTP
    sa_tstp.sa_handler = handle_sigtstp;
    sigemptyset(&sa_tstp.sa_mask);
    sa_tstp.sa_flags = 0;
    sigaction(SIGTSTP, &sa_tstp, NULL);

    // Setup for SIGQUIT
    sa_quit.sa_handler = handle_sigquit;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);

    // Fork to create the child process
    child_pid = fork();
    if (child_pid == 0) { /* Child process */
        execvp(argv[1], &argv[1]);
        // If execvp returns, there was an error
        printf("execvp failed with error: %s\n", strerror(errno));
        exit(EXIT_FAILURE); 
    } else if (child_pid > 0) { /* Parent process */
        int wstatus;
        pid_t wpid;
        do {
            wpid = waitpid(child_pid, &wstatus, WUNTRACED | WCONTINUED);
            if (wpid == -1) {
                if (errno == EINTR) {
                    // System call was interrupted by a signal handler
                    // Resume waiting
                    continue;
                } else {
                    perror("waitpid");
                    exit(EXIT_FAILURE);
                }
            }

            if (WIFEXITED(wstatus)) {
                printf("Child exited, status=%d\n", WEXITSTATUS(wstatus));
            } else if (WIFSIGNALED(wstatus)) {
                printf("Child killed by signal %d\n", WTERMSIG(wstatus));
            } else if (WIFSTOPPED(wstatus)) {
                printf("Child stopped by signal %d\n", WSTOPSIG(wstatus));
            } else if (WIFCONTINUED(wstatus)) {
                printf("Child continued\n");
            }
        } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));

        // Now we can safely exit or do other work in the parent
        for (;;){
 				pause();
 		}
    } else { /* Error */
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // This line should not be reached
    return 0;
}
