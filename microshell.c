#include "microshell.h"

int ft_putstr(char *str)
{
    while (*str)
        write(2, str++, 1);
    return (1);
}

int execute_cd(int argc, char **argv)
{
    if (argc != 2)
    {
        ft_putstr("error: cd: bad arguments\n");
        return (1);
    }
    else if (chdir(argv[1]) == -1)
    {
        ft_putstr("error: cd: cannot change directory to ");
        ft_putstr(argv[1]);
        ft_putstr("\n");
        return (1);
    }
    return (0);
}

int execute_command(char **argv, int i, char **envp)
{
    int pid;
    int next_pipe[2];
    int status;
    t_bool has_pipe = argv[i] && !strcmp(argv[i], "|");

    if (has_pipe == 1)
        pipe(next_pipe);
    pid = fork();
    if (pid == 0)
    {
        argv[i] = NULL;
        if (has_pipe == 1)
        {
            dup2(next_pipe[1], STDOUT_FILENO);
            close(next_pipe[1]);
            close(next_pipe[0]);
        }
        execve(*argv, argv, envp);
        return (ft_putstr("error: cannot execute "), ft_putstr(*argv), ft_putstr("\n"));
    }
    else
    {
        waitpid(pid, &status, 0);
        status = WEXITSTATUS(status);
        if (has_pipe == 1)
        {
            dup2(next_pipe[0], STDIN_FILENO);
            close(next_pipe[0]);
            close(next_pipe[1]);
        }
    }
    return (status);
}

int main(int argc, char **argv, char **envp)
{
    int status;
    int i;

    i = 0;
    while (argv[i] && argv[++i])
    {
        argv += i;
        i = 0;
        while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
            i++;
        if (strcmp(*argv, "cd") == 0)
            status = execute_cd(argc -1 , argv);
        else if (i)
            status = execute_command(argv, i, envp);
    }
    return (status);
}