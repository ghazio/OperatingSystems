/* Swarthmore College, CS 45, Lab 0
 * Copyright (c) 2019 Swarthmore College Computer Science Department,
 * Swarthmore PA
 * Professor Kevin Webb */

#ifndef _PARSECMD__H_
#define _PARSECMD__H_

#define MAXLINE    1024   // max command line size
#define MAXARGS     128   // max number of arguments on a command line

/*
* parse_cmd_dynamic - Parse the passed command line into an argv array.
*
*    cmdline: The command line string entered at the shell prompt
*             (const means that this function cannot modify cmdline).
*             Your code should NOT attempt to modify these characters!
*
*         bg: A pointer to an integer, whose value your code should set.
*             The value pointed to by bg should be 1 if the command is to be
*             run in the background, otherwise set it to 0.
*
*    returns: A dynamically allocated array of strings, each element
*             stores a string corresponding to a command line argument.
*             (Note: the caller is responsible for freeing the returned
*             argv list, not your parse_cmd_dynamic function).
*/

struct cmd* parse_cmd_dynamic(const char *cmdline, int *bg);
//ls -l <a1 1>a2 2> a3 | lsd -ld <b1 1> b2 2> b3
struct cmd {
    char **cmd1_argv;
    char **cmd2_argv;

    char *cmd1_fds[3];
    char *cmd2_fds[3];
    int pipe_flag;
    int cmd1_sizeargv;
    int cmd2_sizeargv;
};

/*
Takes in cmd struct
Searches for a pipe arguement in cmdline
Changes the value of cmd2_argv to NULL if there are no pipes present in cmdline
*/

void has_pipe(struct cmd* cmd_arguments, char* cmdline);

/*
Input: a string(which is our commandline)
FUnction: counts the number of tokens in the commandline
Return: an int that is the number of tokens in the commandline

*/

int count_tokens(char *cmdline);

/*
Parameters: An instance of struct cmd
Function: prints out all the relevant information contained in the struct
Return: Nothing
*/
void print_struct (struct cmd* cmd_struct);

/*
Parameters: commandline string, instance of struct cmd, int indicating first or
            second command of cmdline
Function: Checks for < char (which indicated input stream) and assigns the token
          to the proper element in the cmd struct
Returns: nothing
*/
void inputredirector(char* cmdline, struct cmd* cmd_struct, int command_num);
/*
Parameters: commandline string, instance of struct cmd, int indicating first
            or second command of cmdline
Function: Checks for > char (which indicates output stream) and
          checks for each output stream and assigns the token to the proper
          element in the cmd struct
Returns: nothing
*/
void outputredirector(char* cmdline, struct cmd* cmd_struct, int command_num);

/*
Parameters: commandline string, instance of struct cmd, int indicating first
            or second command of cmdline
Function: Creates an appropriate argv array and assigns the proper commands
          (from the passed in cmdline) into them
Returns: nothing
*/
void argv_generator(char* cmdline, struct cmd* cmd_struct, int command_num);



#endif
