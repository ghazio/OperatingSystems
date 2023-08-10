/* Swarthmore College, CS 45, Lab 0
 * Copyright (c) 2019 Swarthmore College Computer Science Department,
 * Swarthmore PA
 * Professor Kevin Webb
 Names: Sidhika Tripathee and Ghazi Randhawa
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parsecmd.h"



void has_pipe(struct cmd *cmd_arguments, char* cmdline){
  //if there is a |, then set flag to 0
  if(strchr(cmdline, '|')==NULL){
    cmd_arguments->pipe_flag =0;
  }
  //else set flag to 1
  else{
    cmd_arguments->pipe_flag=1;
  }
}


void argv_generator(char* cmdline, struct cmd* cmd_struct, int command_num){

  //searches for < character and stores its address
  char* less_address=strchr(cmdline, '<');
  //if there is a < character, then it replaces it with a null terminator
  //so that Argv array is left with relevant commands
  if(less_address!=NULL){
      *less_address='\0';
  }

  //searches for > character and stores its address
  char* greater_than_address=strchr(cmdline, '>');
  //if there is a > character, then it replaces the number before it
  // with a null terminator so that Argv array is left with relevant commands
  if(greater_than_address!=NULL){
      greater_than_address=greater_than_address-1;
      *greater_than_address='\0';
  }

  //make a copy of the cmdline that we can tokenize it
  char *cmdline_copy = strdup(cmdline);
  //counts the number of tokens left in our edited commandline's copy
  int n_tokens=count_tokens(cmdline_copy);

  //if there was no pipe or if the command was before the pipe
  if(command_num==1){
    //allocate an array with size equal to the number of tokens+1(the +1 is for NULL terminator)
    cmd_struct->cmd1_argv= malloc((n_tokens+1)*(sizeof(char*)));
    //store the number of tokens in our argv array in the struct element
    cmd_struct->cmd1_sizeargv=n_tokens;
    free(cmdline_copy);
    //recopy the cmdline before tokenizing on it
    cmdline_copy = strdup(cmdline);
    int i=0;
    //read in the first token
    char* token = strtok(cmdline_copy, " ");
    //while we dont run out of tokens
    while(token!=NULL){
      //if there is a trailing new line character, we change it to null terminator
      //before we pass it to the execvp function
      char* filename_null=strchr(token,'\n');
      if(filename_null){

        *filename_null=0;
      }
      //store the token in the array
      cmd_struct->cmd1_argv[i]=strdup(token);
      token = strtok(NULL, " ");
      i++;
    }
    //store NULL in the last element of argv array as per convention
    cmd_struct->cmd1_argv[i]=NULL;
    free(cmdline_copy);
  }
  //else we do the same process on the command after the pipe
  else{
    cmd_struct->cmd2_argv= malloc((n_tokens+1)*(sizeof(char*)));
    cmd_struct->cmd2_sizeargv=n_tokens;
    free(cmdline_copy);
    cmdline_copy = strdup(cmdline);
    int i=0;
    char* token = strtok(cmdline_copy, " ");
    while(token!=NULL){
      //if there is a trailing new line character, we change it to null terminator
      char* filename_null=strchr(token,'\n');
      if(filename_null){

        *filename_null=0;
      }
      cmd_struct->cmd2_argv[i]=strdup(token);
      token = strtok(NULL, " ");
      i++;
    }
    cmd_struct->cmd2_argv[i]=NULL;
    free(cmdline_copy);
  }

}


int count_tokens(char *cmdline){
  char* token = strtok(cmdline, " ");
  int counter = 0;
  while(token!=NULL){
    token = strtok(NULL, " ");
    counter++;
  }
  return counter;
}


void print_struct (struct cmd* cmd_struct){

  //Checks if struct element is null
  if(cmd_struct->cmd1_argv==NULL){
    printf("cmd1_argv: %s\n", "NULL");
  }
  //Else, goes through the argv1 array and prints its elements
  else{
    printf("cmd1_argv: [");
    for(int i=0;i<cmd_struct->cmd1_sizeargv;i++){
        printf("%c%s%c, ",'"',cmd_struct->cmd1_argv[i],'"');
    }
    printf("%s]\n","NULL");
  }
  //Checks if struct element is null
  if(cmd_struct->cmd2_argv==NULL){
    printf("cmd2_argv: %s\n", "NULL");
  }
  //Else, goes through the argv2 array and prints its elements
  else{
    printf("cmd2_argv: [");
    for(int i=0;i<cmd_struct->cmd2_sizeargv;i++){
        printf("%c%s%c, ",'"',cmd_struct->cmd2_argv[i],'"');

    }
    printf("%s]\n","NULL");
  }



  //these series of paired if-else statements
  //Check if the relevant struct element is null
  //else it prints the element

  if(cmd_struct->cmd1_fds[0]==NULL){
    printf("cmd1_fds[0]: %s\n", "NULL");
  }
  else{
    printf("cmd1_fds[0]: %s\n",cmd_struct->cmd1_fds[0]);
  }
  if(cmd_struct->cmd1_fds[1]==NULL){
    printf("cmd1_fds[1]: %s\n", "NULL");
  }
  else{

    printf("cmd1_fds[1]: %s\n",cmd_struct->cmd1_fds[1]);
  }
  if(cmd_struct->cmd1_fds[2]==NULL){
    printf("cmd1_fds[2]: %s\n\n", "NULL");
  }
  else{
    printf("cmd1_fds[2]: %s\n\n",cmd_struct->cmd1_fds[2]);
  }

  if(cmd_struct->cmd2_fds[0]==NULL){
    printf("cmd2_fds[0]: %s\n", "NULL");
  }
  else{
    printf("cmd2_fds[0]: %s\n",cmd_struct->cmd2_fds[0]);
  }
  if(cmd_struct->cmd2_fds[1]==NULL){
    printf("cmd2_fds[1]: %s\n", "NULL");
  }
  else{

    printf("cmd2_fds[1]: %s\n",cmd_struct->cmd2_fds[1]);
  }
  if(cmd_struct->cmd2_fds[2]==NULL){
    printf("cmd2_fds[2]: %s\n", "NULL");
  }
  else{
    printf("cmd2_fds[2]: %s\n",cmd_struct->cmd2_fds[2]);
  }

}

void inputredirector(char* cmdline, struct cmd* cmd_struct, int command_num){
  //searches for the < char in the input string
  char* less_address=strchr(cmdline, '<');
  //if we find a < char,
  if(less_address!=NULL){
    //then we copy the token after the < char appropriately
    char* after_less=less_address+1;
    char *after_less_copy = strdup(after_less);

    //get the first token
    char* token = strtok(after_less_copy, " ");
    //if there is a trailing new line character, we change it to null terminator
    char* filename_null=strchr(token,'\n');
    if(filename_null){

      *filename_null=0;
    }
    //if input string is before pipe
    if(command_num==1){
      cmd_struct->cmd1_fds[0]=strdup(token);

      }
    //else it is the command after pipe
    else{
      cmd_struct->cmd2_fds[0]=strdup(token);
    }
    free(after_less_copy);
  }
}


void outputredirector(char* cmdline, struct cmd* cmd_struct, int command_num){
  //searches for the > char in the input string
  char* output_address=strchr(cmdline, '>');
  //if > char is in cmdline
  if(output_address!=NULL){
    //finds out the type of output stream
    char* output_num=output_address-1;

    //copy the token after the > char appropriately
    char* after_greater=output_address+1;
    char *after_greater_copy = strdup(after_greater);

    //get the first token
    char* token = strtok(after_greater_copy, " ");
    //if there is a trailing new line character, we change it to null terminator
    char* filename_null=strchr(token,'\n');
    if(filename_null){

      *filename_null=0;
    }
    //assigns the token value after > to the corresponding element in struct
    if(*output_num=='1' && command_num==1){
      cmd_struct->cmd1_fds[1]=strdup(token);
    }
    else if (*output_num=='2' && command_num==1){
      cmd_struct->cmd1_fds[2]=strdup(token);
    }
    else if (*output_num=='1' && command_num==2){
      cmd_struct->cmd2_fds[1]=strdup(token);
    }
    else if (*output_num=='2' && command_num==2){
      cmd_struct->cmd2_fds[2]=strdup(token);
    }

    //if there is another output stream in the rest of the command,
    free(after_greater_copy);
    after_greater_copy = strdup(after_greater);

    if(strchr(after_greater_copy,'>')!=NULL){
      //recursively calls the function
      outputredirector(after_greater_copy,cmd_struct,command_num);
    }
    free(after_greater_copy);
  }


}



struct cmd* parse_cmd_dynamic(const char *cmdline, int *bg) {

    char **result = NULL;

    *bg=0; // initialize bg to 0 so we can set to 1 later based on cond'tls

    //set bg to 1 if & is found in cmdline
    if(strchr(cmdline,'&')!=NULL){
      *bg=1;
    }

    /* Make a copy of the original command string. */
    char *cmdline_copy = strdup(cmdline);
    if (cmdline_copy == NULL) {
        perror("strdup");
        return NULL;
    }
    //we get the number of tokens in our commandline
    int tok_len = count_tokens(cmdline_copy);
    free(cmdline_copy);
    //recopy the cmdline
    cmdline_copy = strdup(cmdline);

    //create a dynammically allocated struct cmd
    struct cmd *cmd_struct = malloc(sizeof(struct cmd));
    cmd_struct->cmd1_fds[0]=NULL;
    cmd_struct->cmd1_fds[1]=NULL;
    cmd_struct->cmd1_fds[2]=NULL;
    cmd_struct->cmd2_fds[0]=NULL;
    cmd_struct->cmd2_fds[1]=NULL;
    cmd_struct->cmd2_fds[2]=NULL;
    cmd_struct->cmd1_argv=NULL;
    cmd_struct->cmd2_argv=NULL;


    //check for presence of '|' in command line
    has_pipe(cmd_struct, cmdline_copy);
    char* ampersand_location= strchr(cmdline_copy,'&');
    if(ampersand_location != NULL){
      *ampersand_location='\0';
    }

    //if the commandline has a pipe character,
    if(cmd_struct->pipe_flag==1){
      //find address of the pipe character
      char* pipe_address=strchr(cmdline_copy, '|');
      //save the second command
      char* after_pipe=pipe_address+1;
      //then replace the pipe characte with a null terminator.
      *pipe_address='\0';
      //save the first command
      char* before_pipe=cmdline_copy;

      //compute I/O redirection functions
      inputredirector(before_pipe,cmd_struct,1);
      inputredirector(after_pipe,cmd_struct,2);
      outputredirector(before_pipe,cmd_struct,1);
      outputredirector(after_pipe,cmd_struct,2);
      argv_generator(before_pipe,cmd_struct,1);
      argv_generator(after_pipe,cmd_struct,2);

    }
    //else there is no pipe
    else{
      // cmd_struct->cmd1=cmdline;
      inputredirector(cmdline_copy,cmd_struct,1);
      outputredirector(cmdline_copy,cmd_struct,1);
      argv_generator(cmdline_copy,cmd_struct,1);

    }


    print_struct(cmd_struct);
    //free the malloced arrays
    free(cmdline_copy);

    return cmd_struct;
}
