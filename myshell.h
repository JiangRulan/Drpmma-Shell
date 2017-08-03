#ifndef _MYSHELL_H
#define _MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>  
#include <sys/types.h>  
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h> 
#include "job_ctrl.h"

#define FILE_PATH_LENGTH 100
#define COMMAND_NUMBER 20
#define COMMAND_SIZE 64
#define ARGUMENT_SIZE 32

#define TEST_B 0
#define TEST_C 1

#define TEST_E 3
#define TEST_F 4

#define TEST_H 6
#define TEST_L 8
#define TEST_P 10
#define TEST_R 11
#define TEST_W 16
#define TEST_X 17

#define TEST_EQ 30
#define TEST_GE 31
#define TEST_GT 32
#define TEST_LE 33
#define TEST_LT 34
#define TEST_NE 35

extern char** environ;

struct jobs* job_array;

char* HOME;

int shell_cd(char** args);
int shell_umask(char** args);
int shell_time(char** args);
int shell_environ(char** args);
int shell_set(char** args);
int shell_unset(char** args);
int shell_exec(char** args);
int shell_help(char** args);
int shell_exit(char** args);
int shell_test(char** args);
int test_dir(char* arg);
int test_file(char* arg, int flag);
int test_logic(char** args, int flag);

void main_loop();
char* read_line();
char** split_str(char* line, int size, char* delims);
int parse_redirect(char** args, int* pfd_in, int* pfd_out);
int parse_pipe(struct command* cmd_array, int size);
int check_builtin(struct command cmd);
int builtin_cmd(struct command cmd);
int execute(struct command cmd, int fd_in, int fd_out, int fd_err);
void clear_buffer(struct command* cmd_array, char* line, char** cmds);
void signals();

#endif