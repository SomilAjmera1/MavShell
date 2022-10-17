/*

Name: Somil Ajmera
Mav ID: 1001864370

*/
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define Exclamition "!"

#define WHITESPACE " \t\n" // We want to split our command line up into tokens
                           // so we need to define what delimits our tokens.
                           // In this case  white space
                           // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 12 // Mav shell only supports five arguments

int main()
{

  char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);
  int pids[] = {};
  int counter1 = 0;

  while (1)
  {

    // Print out the msh prompt
    printf("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin))
      ;

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;

    char *working_str = strdup(cmd_str);

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while (((argument_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
           (token_count < MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }

      token_count++;
    }

    // this is used if a blank line is entered by the user so the >msh prints again
    // this help us run to the next iteration of the loop by continue
    if (token[0] == NULL)
    {
      continue;
    }

    // this is used when the user wants to exit out of the shell program by
    // using quit and exit as keywords by using strcmp to check the cmd_str 
    if (strcmp(token[0], "exit") == 0 || strcmp(token[0], "quit") == 0)
    {
      exit(0);
    }

    if (!strcmp(token[0], "cd"))
    {
      chdir(token[1]);
      continue;
    }

    // this part is used to create a new child and run the process here that is determined
    // by the user and execvp will take that command and convert it to a unix binary string
    // helping us run it in the docker
    pid_t pid = fork();

    if (pid == 0 && strcmp(token[0], "cd"))
    {
      if (execvp(token[0], token) == -1)
      {
        printf("%s: Command not found.\n", token[0]);
        exit(0);
      }
    }
    else
    {
      // I am waiting for the child process to end so I can store the second return value
      // of fork which is the pid of the child process so I can print the pids later
      wait(NULL);
      pids[counter1] = pid;
      counter1++;
    }

    // freeing the memory that was dynamically allocted using malloc to avoid seg faults
    free(working_str);
  }
  return 0;
}