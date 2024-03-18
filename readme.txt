README for forkexecvp.c

This program demonstrates the creation of a child process using fork() and its execution of a new program using execvp(). Additionally, it shows how to handle signals such as SIGINT (Control-C), SIGTSTP (Control-Z), and SIGQUIT (Control-\) using custom signal handlers.

USAGE:
Compile the program with:
  gcc -Wall -o forkexecvp forkexecvp.c

Run the program with:
  ./forkexecvp <command> [args]

Where <command> is the command you want to run in the child process, and [args] are the arguments to the command.

EXAMPLE:
  ./forkexecvp ls -l /home/user

BEHAVIOR:
- When the child process is running, if the user sends a SIGINT signal (usually by pressing Control-C), the parent process will print a message, forward the SIGINT signal to the child process, and continue running.
- If the user sends a SIGTSTP signal (usually by pressing Control-Z), the parent process will print a message, forward the SIGTSTP signal to the child process, and continue running.
- When the parent process receives a SIGQUIT signal (usually by pressing Control-\), it will print a message, terminate the child process if it is still running, and then exit itself.

The program is designed to handle signals cleanly to ensure that the child process can be interrupted or stopped without causing the parent process to exit prematurely.

NOTES:
- The parent process uses a loop with waitpid() to wait for the child process to change state, handling EINTR (Interrupted system call) errors by continuing to wait.
- After handling the termination or stopping of the child process, the parent process enters an infinite loop where it pauses and waits for the SIGQUIT signal.
- Ensure the terminal is set up to send signals to the foreground process group for correct signal handling.
- The global variable child_pid is used to store the child's process ID, which is needed by the signal handlers to send signals to the correct process.

