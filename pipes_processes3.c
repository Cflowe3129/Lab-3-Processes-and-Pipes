#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h> 
#include <stdlib.h>
/**
 * Executes the command "cat scores | grep Lakers".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 * 
 * use ./implementation <arg> to run
 */

int main(int argc, char **argv)
{
  int pipefd[2];   // Used to store two ends of first pipe 
  int pipefd2[2];  // Used to store two ends of second pipe 

  int pid;
  int pid_2;
  
  char output[100];
  
  char *cat_args[] = {"cat", "scores", NULL}; 
  char *grep_args[] = {"grep", argv[1], NULL};
  char *sort_args[] = {"sort", NULL};

  // create pipes

  pipe(pipefd);
  pipe(pipefd2);
  
  pid = fork(); //fork first child
  if (pid == 0)
    {
        pid_2 = fork(); // fork child's child
        if (pid_2 == 0) {

          dup2(pipefd2[0], 0); // replace input 

          // close pipes
          close(pipefd[0]);
          close(pipefd[1]);
          close(pipefd2[0]);
          close(pipefd2[1]);

          // execute sort
          execvp("sort", sort_args);
          exit(0);
        } 
    
        // replace standard input with input part of pipe
        dup2(pipefd[0], 0);
        dup2(pipefd2[1], 1);

        // close pipes
        close(pipefd[0]);
        close(pipefd[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        
        execvp("grep", grep_args);// execute grep

        

    } else if (pid > 0){
          // parent gets here and handles "cat scores"


          dup2(pipefd[1], 1);// replace standard output with output part of pipe

          // close pipes
          close(pipefd[0]);
          close(pipefd[1]);
          close(pipefd2[0]);
          close(pipefd2[1]);

          execvp("cat", cat_args); //execute cat
    
          int status;
          int waitID;
          while ((waitID = wait(&status)) > 0) {
            printf("Child has completed.\n");
          }
            
    }
  return 0;
}