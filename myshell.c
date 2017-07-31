#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>  
#include <sys/types.h>  
#include <time.h>
#include "myshell.h"


int main(void)
{
    main_loop();                          // 主循环
}

void main_loop()
{
    char* line;
    char** args;
    int status = 1;
    char *file_path;

    file_path=(char *)malloc(FILE_PATH_LENGTH);
    getcwd(file_path, FILE_PATH_LENGTH);
    file_path=strcat(file_path, "/myshell");
    setenv("MYSHELL", file_path, 0);
    while(status)
    {
        getcwd(file_path, FILE_PATH_LENGTH);
        printf("myshell:%s>", file_path);
        line = read_line();
        args = split_line(line);
        status = execute(args);
        printf("return:status:%d\n", status);
    }
}

char* read_line()
{
    char *line = NULL;
    ssize_t bufsize = 0;
    getline(&line, &bufsize, stdin);            // 通过getline函数可以方便的读入一行
    return line;
}

char** split_line(char* line)
{
    int buf_size = ARGUMENT_SIZE, pos = 0;
    char* arg;
    char** args = malloc(buf_size * sizeof(char*));

    arg = strtok(line, " \t\n");
    while(arg != NULL)
    {
        args[pos] = arg;
        pos++;

        if(pos >= buf_size)
        {
            buf_size += ARGUMENT_SIZE;
            args = realloc(args, buf_size * sizeof(char*));
        }

        arg = strtok(NULL, " \t\n");
    }
    args[pos] = NULL;
    return args;
}

int parse_pipe()
{
    char** args = malloc(64 * sizeof(char*));
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("myshell");
    }
    else if(pid == 0)
    {
        args[0] = "ls";
        close(0);
        close(fd[0]);
        close(fd[1]);
        dup(fd[0]);
        execute(args);
    }
    else
    {
        args[0] = "grep";
        args[1] = "m";
        close(1);
        close(fd[0]);
        close(fd[1]);
        dup(fd[1]);
        execute(args);
    }
    return 1;
}

int execute(char** args)
{
    if(args[0] == NULL)
        return 1;

    pid_t pid, w_pid;
    int status;

    pid = fork();
    if(pid < 0)
    {
        perror("myshell");
    }
    else if(pid == 0)
    {
        // signal(SIGINT, SIG_DFL);
        // signal(SIGTSTP, SIG_DFL);
        // signal(SIGCONT, SIG_DFL);

        for (int i = 0; i < sizeof(internal_str) / sizeof(char*); i++) 
        {
            if (strcmp(args[0], internal_str[i]) == 0) 
            {
                return (*internal_cmd[i])(args);
            }
        }
        if(execvp(args[0], args) == -1)
        {
            perror("myshell");
        }
        exit(1);
    }
    else
    {
        // signal(SIGINT, SIG_IGN);
        // signal(SIGTSTP, SIG_IGN);
        // signal(SIGCONT, SIG_DFL);
        do {
            w_pid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    printf("status:%d\n", status);
    return !status;
}

int shell_cd(char** args)
{
    if(args[1] == NULL)
    {
        chdir(getenv("HOME"));
    }
    else
    {
        if(chdir(args[1]) != 0)
            perror("myshell");
    }
    return 0;
}

int shell_time(char** args)
{
    time_t now;
    struct tm* time_now;
    time(&now);
    time_now = localtime(&now);
    printf("%s", asctime(time_now));
    return 0;
}

int shell_umask(char** args)
{
    mode_t new_umask = 0666, old_umask;
    old_umask = umask(new_umask);
    if(args[1] == NULL)
    {
        printf("%04o\n", old_umask);
        umask(old_umask);
    }
    else
    {
        new_umask = strtoul(args[1], 0, 8);
        printf("%04o\n", new_umask);
        umask(new_umask);
    }
    return 0;
}

int shell_environ(char** args)
{
    int i = 0;
    for(i = 0; environ[i] != NULL; i++)
    {
        printf("%s\n",environ[i]);
    }
    return 0;
}

int shell_set(char** args)
{
    if(args[1] == NULL)
    {
        shell_environ(args);
    }
    else if(args[2] == NULL)
    {
        setenv(args[1], "NULL", 0);
    }
    else
    {
        setenv(args[1], args[2], 0);
    }
    return 0;
}

int shell_unset(char** args)
{
    if(args[1] != NULL)
    {
        unsetenv(args[1]);
    }
    return 0;
}

int shell_exec(char** args)
{
    execvp(args[1], args + 1);
    return 0;
}

int shell_help(char** args)
{
    char* cmd[] = {"more", "README.md"};
    execvp(cmd[0], cmd);
    return 0;
}

int shell_exit(char** args)
{
    return 1;
}