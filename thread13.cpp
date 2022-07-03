#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

int main()
{
   int status;
   pid_t pid = fork();

   // Child process will sleep for 10 second
   if(pid == 0)
   {
      execl("/usr/bin/sleep", "/usr/bin/sleep", 10, NULL);
   }
   // Parent process will wait for child process to terminate
   // Then, it will report the exit status of the child process
   else
   {
      waitpid(pid, &status, 0);
      printf("status = %d\n", status);  // print out ->  status = 65280
   }
}