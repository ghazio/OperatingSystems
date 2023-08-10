/* Swarthmore College, CS 45, Lab 0
 * Copyright (c) 2019 Swarthmore College Computer Science Department,
 * Swarthmore PA
 * Professor Kevin Webb */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "parsecmd.h"

/* The maximum size of your circular history queue. */
#define MAXHIST 10

/*
 * A struct to keep information one command in the history of
 * command executed
 */
struct histlist_t {
    unsigned int cmd_num;
    char cmdline[MAXLINE]; // command line for this process
};

/* Global variables declared here.
 * For this assignment, only use globals to manage history list state.
 * all other variables should be allocated on the stack or heap.
 *
 * Recall "static" means these variables are only in scope in this .c file. */
static struct histlist_t history[MAXHIST];

/* Keep some state so that we know the array index to use when
 * storing the value of the next insert operation. */
static int history_next = 0;

/* State for us to keep track of the number of items in the queue. */
static int history_size = 0;

/* State for us to keep track of the number of entries made for history */
static int cmd_id = 1;

//Function prototypes
void show_history();
static void add_queue(char* cmd);
static void print_queue(void);
void find_cmd(char* cmd);
void child_handler(int sig);
void run_foreground(struct cmd* cmd_struct);
void run_background(struct cmd* cmd_struct);
void struct_freer(struct cmd* cmd_struct);
char* getPath (char* cmd);


void struct_freer(struct cmd* cmd_struct);

int main(int argc, char **argv) {

  char cmdline[MAXLINE];
  int bg;

  // TODO: add a call to signal to register your signal handler on
  //       SIGCHLD here

  while(1) {
    // (1) print the shell prompt
    printf("cs45shell> ");
    fflush(stdout);

    // (2) read in the next command entered by the user
    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
      perror("fgets error");
    }
    if (feof(stdin)) { /* End of file (ctrl-d) */
      fflush(stdout);
      exit(0);
    }


    //TODO: check if we run the function if cmdline is nothing

    //make a call to your parsing library to parse it into its
    //     args format
    struct cmd* cmd_struct=  parse_cmd_dynamic(cmdline, &bg);

    // Check to make sure the user didn't type ENTER (to avoid a segfault)
    if(cmd_struct->cmd1_argv[0]!=NULL){
      if(strncmp(cmd_struct->cmd1_argv[0],"cd",2)==0){
        char* filename_null=strchr(cmd_struct->cmd1_argv[1],'\n');
        if(filename_null){

          *filename_null=0;
        }

        chdir(cmd_struct->cmd1_argv[1]);

      }




      //If command is "exit", exit the program.
      if(strncmp(cmd_struct->cmd1_argv[0],"exit",4)==0){

            fflush(stdout);
            printf("Quiting!! \n");
            struct_freer(cmd_struct);  // free malloc'ed memory
            exit(1);
          }

      // If command is !num, call find_cmd which checks in the circular
      // history queue to see if the num is a vaild entry in the queue.
      // Find_num also updates cmdline so that we can reparse the newly
      // found, old command.
      if(cmd_struct->cmd1_argv[0][0] == '!'){
          find_cmd(cmdline);
          // Checks history queue for command
          // Parse the newly found, old cmdline so that args contains the
          // tokens that should be executed
          struct_freer(cmd_struct); // free the old args
          cmd_struct=  parse_cmd_dynamic(cmdline, &bg);
      }

      // 4c. If the find_cmd function DID change the !num cmd into a
      // different command, then we need to add the 'current cmdline'^
      // to the history queue.
          // ^ Note that the 'current cmdline' is either what the user
          // entered at this cs31shell> prompt, OR it is a different string
          // from the history queue (if !num was used validly.)
      if (cmd_struct->cmd1_argv[0][0] != '!') {
        // Put the cmdline into the circular queue
        add_queue(cmdline);
      }

      // 4d. If command is "history", display the last 10 entries!
      if (!strncmp("history", cmd_struct->cmd1_argv[0],7)) {
        print_queue();
      }

      // 5a. Run command in the FOREGROUND
          // else stmt so 'history' is not run in a child process
          // && so that !num is not run in a child process, since
          // using else if for the conditionals above is not compatible
          // with our code. Else if multiway conditional statements would
          // result in only one of 4b and 4c executing, but we need BOTH
          // conditionals to be evaluated/checked each time.
      else if ((bg == 0) && (cmdline[0] != '!') && (strncmp(cmd_struct->cmd1_argv[0],"cd",2)!=0)) {

        run_foreground(cmd_struct);
      }

      // 5b. Run command in the BACKGROUND
      if (bg == 1 && (strncmp(cmd_struct->cmd1_argv[0],"cd",2)!=0)) {
        run_background(cmd_struct);
      }
    }
    // (6) DONE: free all that malloc'd memory!
     struct_freer(cmd_struct);// a parsecmd.c library function

     }
     return 0;
 }


 /* Purpose: Checks in the circular history queue to see if the num of a
  * !num command is a vaild entry in the queue.
  * - If num IS valid, find_cmd sets cmdline to be the string of the
  * command line at the num index in the history queue.
  * - If num is NOT valid, find_num prints an error message if the integer is
  * not found and does not change cmdline.
  * Parameter: char* cmd: the command string
  * Returns: none
  */
 void find_cmd(char* cmd){
     char* cmd_copy = cmd;
     int cmd_not_found = 1; // set flag
     cmd_copy++;                 // sets value of cmd to num (gets rid of !)
     int cmd_int = atoi(cmd_copy);  // converts string to integer

     // Interate through the entries in the history array
     for (int i = 0; i < history_size; i++){
       // if the cmd id corresponds to a valid entry in history
       if (history[i].cmd_num == cmd_int){
         cmd_copy--; // decrement cmd to point back to the start of cmd string
         // overwrite cmd (!num) with the command to be executed
         strcpy(cmd, history[i].cmdline);
         cmd_not_found = 0;  // set flag to show that command WAS found
       }
     }
     // If the cmd_id was not found, print an error and don't change the cmd
     // string
     if (cmd_not_found){
     printf("Error: !%d not found in history \n", cmd_int);
     fflush(stdout);
   }
 }

 /* Purpose: Adds one string to the circular history queue, puts the string in
  * the first empty bucket or the oldest bucket if no buckets are empty.
  * Then sets the cmd_id for that entry and increments the history_next and
  * history_size global variables.
  * Parameter: char* cmd is a pointer the string to add to the queue
  * Returns: none
  */
 static void add_queue(char* cmd) {
     // Function Goal: Update the state to reflect that a new value has been
     // added.

     // make sure that a "!num" cmd was not invalid causing cmd == NULL
     if (cmd != NULL) {
       // if the array is not filled completely yet
       if (history_size < MAXHIST) {
         // put cmd into history, the array of historyt_t structs
         strcpy(history[history_next].cmdline, cmd);

         // set the command ID for the entry into the history queue
         history[history_next].cmd_num = cmd_id;
         cmd_id++;

         history_next++;
         history_size++;

       // when array is full, queue loops around to the beginning of array
       } else {
         history_next %= history_size;
         // put cmd into the history array of historyt_t structs
         strcpy(history[history_next].cmdline, cmd);

         // set the command ID for the entry into the history queue
         history[history_next].cmd_num = cmd_id;
         cmd_id++;

         history_next++;
       }
     }
 }

 /* Purpose: Prints the circular queue in order based on the current most
  * recent value.
  * Parameters: none
  * Return: none
  */
 static void print_queue(void){
     // DONE: Print out the queue's values in the order that they were added,
     // first to last.  (For clarity, print both the index and the value stored
     // there).
     int copy_hn = history_next; // copy of history_next that we can manipulate
     for(int i = 0; i < history_size; i++) {

       // if history_next reaches the end of the filled array, resets to the
       // beginning of array
       if(copy_hn == history_size){
         copy_hn = 0;
       }
       printf("%u: %s\n", history[copy_hn].cmd_num, history[copy_hn].cmdline);
       fflush(stdout);
       copy_hn++;
       }
 }

 char* getPath (char* cmd){
    int flag = 0;

    //if there is a trailing new line character, we change it to null terminator
    char* filename_null=strchr(cmd,'\n');
    if(filename_null){

      *filename_null=0;
    }
    if(strlen(cmd)>=2){
      if(cmd[0]=='.' && cmd[1]=='/'){
        flag=1;
      }
      if(cmd[0]=='/'){
        flag=1;
      }
    }
    if(strlen(cmd)>=3){
      if(cmd[0]=='.' && cmd[1]=='.' && cmd[2]== '/'){
        flag=1;
      }
    }
    //if flag hasn't been trigged, ie. the command doesn't start with .,./, ../
    if(flag == 0){
      //call the getenv function to get a string containing the path
      char * path = getenv("PATH");
      //copy the path variable
      char* path_copy = strdup(path);
      //read in the first token(tokenise by ':'' character)
      char* token = strtok(path_copy, ":");

      //while we dont run out of tokens
       while(token!=NULL){
         //copy the token and cmd
        int size_copy=strlen(token)+strlen(cmd)+strlen("/");
        char* new_path=malloc((size_copy+1)*sizeof(char));
        //concatonate the path and token to get a final possible path
        char* copy_pointer=token;
        int i=0;
        while(copy_pointer[i]!='\0'){
          new_path[i]=copy_pointer[i];
          i++;
        }
        new_path[i]='/';
        i++;
        int j=0;
        copy_pointer=cmd;
        while(copy_pointer[j]!='\0'){
          new_path[i]=copy_pointer[j];
          i++;
          j++;
        }
         new_path[i]='\0';

        // check if that path is executeable and return it if it is
        if(access(new_path,X_OK)==0){
          return new_path;
        }
        //move to the next token
        token = strtok(NULL, ":");
      }
    }
    return cmd;
}

void setup_io(struct cmd* cmd_struct, int pipe_flag){
  //if there is an input file
  if(pipe_flag==1){
  if(cmd_struct->cmd1_fds[0]!=NULL){

  //open file
    int input_file=open(cmd_struct->cmd1_fds[0],O_RDONLY);
    //error checking
    if(input_file==-1){
      perror("input_file error");
      fflush(stdout);
      exit(1);
    }
   //put input file into fds table
   dup2(input_file,0);
 }
 //if there is a standard output file
 if(cmd_struct->cmd1_fds[1]!=NULL){

 //open file
   int output_file=open(cmd_struct->cmd1_fds[1],O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
   //error checking
   if(output_file==-1){
     perror("output error");
     fflush(stdout);
     exit(1);
   }
  //put input file into fds table
  dup2(output_file,1);
}
//if there is a standard error file
if(cmd_struct->cmd1_fds[2]!=NULL){
//this is for the left side of pipe
// dup2(pipefds[1],1);
// }

//open file
  int error_file=open(cmd_struct->cmd1_fds[2],O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  //error checking
  if(error_file==-1){
    perror("error_file error");
    fflush(stdout);
    exit(1);
  }
 //put input file into fds table
 dup2(error_file,2);
 }
}
//if this is after the pipe
else{
  if(cmd_struct->cmd2_fds[0]!=NULL){

  //open file
    int input_file=open(cmd_struct->cmd2_fds[0],O_RDONLY);
    //error checking
    if(input_file==-1){
      perror("input_file error");
      fflush(stdout);
      exit(1);
    }
   //put input file into fds table
   dup2(input_file,0);
 }
 //if there is a standard output file
 if(cmd_struct->cmd2_fds[1]!=NULL){

 //open file
   int output_file=open(cmd_struct->cmd2_fds[1],O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
   //error checking
   if(output_file==-1){
     perror("output error");
     fflush(stdout);
     exit(1);
   }
  //put input file into fds table
  dup2(output_file,1);
}
//if there is a standard error file
if(cmd_struct->cmd2_fds[2]!=NULL){
//this is for the left side of pipe
// dup2(pipefds[1],1);
// }

//open file
  int error_file=open(cmd_struct->cmd2_fds[2],O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  //error checking
  if(error_file==-1){
    perror("error_file error");
    fflush(stdout);
    exit(1);
  }
 //put input file into fds table
 dup2(error_file,1);
 }
}

}


 /* Purpose: Executes the forking required for the patient parent (foreground)
  * Thus, the program forks and then:
  * if PARENT: waits for child to exit before doing anything else
  * if CHILD: calls execvp on the command and then exits
  * Parameters: char** args, the array of tokens from the command line
  * Returns: none
  */
 void run_foreground(struct cmd* cmd_struct){

   //if there is a pipe
   if(cmd_struct->pipe_flag == 1){
   //create pipe
   int pipefds[2];
   int ret = pipe(pipefds);
   //error checking
   if(ret==-1){
     perror("fgets error");
     fflush(stdout);
     exit(1);
   }

   //fork the process
   pid_t pid_1 = fork();

   //child process
   if(pid_1 == 0 ){

     //call setup_io
     setup_io(cmd_struct,1);
    //this is for the left side of pipe
      dup2(pipefds[1],1);
     //close read end
     close(pipefds[0]);
    char* path=getPath(cmd_struct->cmd1_argv[0]);

    int exec_ret = execv(path, cmd_struct->cmd1_argv);

   //if execvp returns, there is an error and so we print an error message,
   // free the allocated memory of args, and exit.
     if(exec_ret){
       // if command is not valid and exec fails, print error
         perror("child1 exec error");
         printf("Error in exec: invalid command\n");
         fflush(stdout);
         exit(1);
     }
   }


   //this if for the right side of the pipe
   //TODO: maybe add condition to check if this is right side

   //fork for second command
   pid_t pid_2 = fork();

   //run exec like normal
   //for second child, call dup function and run exec function
   if(pid_2== 0 ){
     //puts the first pipe into the input for the second pipe
     //call setup_io with pipe flag == 2
     setup_io(cmd_struct,2);
     dup2(pipefds[0],0);
     //closing the write end of pipe
     close(pipefds[1]);
   char* path=getPath(cmd_struct->cmd2_argv[0]);
   int exec_ret = execv(path, cmd_struct->cmd2_argv);

   //if execvp returns, there is an error and so we print an error message,
   // free the allocated memory of args, and exit.
     if(exec_ret){
       // if command is not valid and exec fails, print error

         perror("child2 exec error");
         printf("Error: invalid command\n");
         fflush(stdout);
         exit(1);
     }

   }
   // if PARENT, wait for child
   else {

     close(pipefds[0]);
     close(pipefds[1]);
      waitpid(pid_2, NULL, 0);
   }

  }


//if there is no pipe
  else{
    pid_t pid = fork();
    // run execvp() with the user's input command
    if(pid== 0 ){
      //call setup_io
    setup_io(cmd_struct,1);
    char* path=getPath(cmd_struct->cmd1_argv[0]);

    int exec_ret = execv(path, cmd_struct->cmd1_argv);


    //if execvp returns, there is an error and so we print an error message,
    // free the allocated memory of args, and exit.
      if(exec_ret){
        // if command is not valid and exec fails, print error
          perror("fgets error");
          printf("Error: invalid command\n");
          fflush(stdout);
          exit(1);
      }
    }
    // if PARENT, wait for child
    else {
      waitpid(pid, NULL, 0);
    }
  }
}

 //
 // /* Purpose: This child_handler function is only called when a SIGCHLD signal
 //  * is received from the child to the parent, notifying the parent that
 //  * there is an exited child process to reap.
 //  * Parameter: int sig: a SIGCHLD signal from the child process
 //  * Returns: none
 //  */
 void child_handler(int sig){
     pid_t pid;
     // Calls an inpatient parent waitpid (with -1)
     // While no children were reaped, do other stuff
     while((pid = waitpid(-1, NULL, WNOHANG)) >0){
    }
 }
 //
 // /* Purpose: Executes the forking required for the inpatient parent (background)
 //  * Thus, the program forks and then:
 //  * if PARENT: does not wait for children to exit before continuing
 //  * if CHILD: calls execvp on the command and then exits
 //  * Parameters: char** args, the array of tokens from the command line
 //  * Returns: none
 //  */
 void run_background(struct cmd* cmd_struct){
   // add a call to signal to register your SIGCHLD signal handler.
   signal(SIGCHLD, child_handler);
   if(cmd_struct->pipe_flag==1){
     //create pipe
     int pipefds[2];
     int ret = pipe(pipefds);
     //error checking
     if(ret==-1){
       perror("fgets error");
       fflush(stdout);
       exit(1);
     }

     //fork the process
     pid_t pid_1 = fork();

     //child process
     if(pid_1 == 0 ){

       //call setup_io
       setup_io(cmd_struct,1);
      //this is for the left side of pipe
        dup2(pipefds[1],1);
       //close read end
       close(pipefds[0]);
      char* path=getPath(cmd_struct->cmd1_argv[0]);
      int exec_ret = execv(path, cmd_struct->cmd1_argv);

     //if execvp returns, there is an error and so we print an error message,
     // free the allocated memory of args, and exit.
       if(exec_ret){
         // if command is not valid and exec fails, print error
           perror("child1 exec error");

           printf("Error: invalid command\n");
           fflush(stdout);
           exit(1);
       }
     }


     //this if for the right side of the pipe
     //TODO: maybe add condition to check if this is right side

     //fork for second command
     pid_t pid_2 = fork();

     //run exec like normal
     //for second child, call dup function and run exec function
     if(pid_2== 0 ){
       //puts the first pipe into the input for the second pipe
       //call setup_io with pipe flag == 2
       setup_io(cmd_struct,2);
       dup2(pipefds[0],0);
       //closing the write end of pipe
       close(pipefds[1]);
     char* path=getPath(cmd_struct->cmd2_argv[0]);

     int exec_ret = execv(path, cmd_struct->cmd2_argv);

     //if execvp returns, there is an error and so we print an error message,
     // free the allocated memory of args, and exit.
       if(exec_ret){
         // if command is not valid and exec fails, print error

           perror("child2 exec error");

           printf("Error: invalid command\n");
           fflush(stdout);
           exit(1);
       }

     }
     // if PARENT, wait for child
     else {

       close(pipefds[0]);
       close(pipefds[1]);
       waitpid(-1, NULL, WNOHANG);
     }

   }
   else{
     pid_t pid = fork();
     // run execvp() with the user's input command
     if(pid== 0 ){
       //call setup_io
     setup_io(cmd_struct,1);
     char* path=getPath(cmd_struct->cmd1_argv[0]);
     int exec_ret = execv(path, cmd_struct->cmd1_argv);

     //if execvp returns, there is an error and so we print an error message,
     // free the allocated memory of args, and exit.
       if(exec_ret){
         // if command is not valid and exec fails, print error
           perror("Invalid path");
           printf("Error: invalid command\n");
           fflush(stdout);
           exit(1);
       }
     }

   // if PARENT, reap exited child processes
   else {
     // reaps exited child processes until there are none left.
     waitpid(-1, NULL, WNOHANG);
   }
  }
  }


/* Parameters: cmd_struct from parsecmd.c
  Funtion: Frees all the elements from the struct
  Returns: Nothing
*/

void struct_freer(struct cmd* cmd_struct){
    int i =0;
    while(cmd_struct->cmd1_argv[i]!=NULL){
      free(cmd_struct->cmd1_argv[i]);
      i++;
    }

    free(cmd_struct->cmd1_argv);
    i =0;

    while(i<3){
      if(cmd_struct->cmd1_fds[i]!=NULL){
        free(cmd_struct->cmd1_fds[i]);
      }
      i++;
    }


    if(cmd_struct->pipe_flag==1){
      i=0;

      while(cmd_struct->cmd2_argv[i]!=NULL){

        free(cmd_struct->cmd2_argv[i]);
        i++;
      }

      free(cmd_struct->cmd2_argv);
      i =0;

      while(i<3){
        if(cmd_struct->cmd2_fds[i]!=NULL){
          free(cmd_struct->cmd2_fds[i]);
        }
        i++;
      }


    }

    free(cmd_struct);

}
