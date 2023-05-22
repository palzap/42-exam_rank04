#include "microshell3.h"

static void put_error(char *str)
{
    while (*str)
        write(2, str++, 1);
}

int main(int ac, char **av, char **env)
{
    int i = 0;
    int fd[2];
    int temp;
    (void)ac;

    temp = dup(0);
    while (av[i] && av[i + 1])
    {
        av = &av[i + 1];
        i = 0;
        while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
            i++;
        if (!strcmp(av[0], "cd"))
        {
            if (i != 2)
                put_error("bad arguments\n");
            else
            {
                if (chdir(av[1]) != 0)
                {
                    put_error("cannot change to ");
                    put_error(av[1]);
                    put_error("\n");
                }
            }
        }
        else if (i != 0 && (av[i] == 0 || !strcmp(av[i], ";")))
        {
            if (fork() == 0)
            {
                av[i] = 0;
                dup2(temp, 0);
                close(temp);
                execve(av[0], av, env);
                put_error("cannot execute ");
                put_error(av[0]);
                put_error("\n");
                return (1);
            }
            else
            {
                close(temp);
                while (waitpid(-1, 0, WUNTRACED) != -1)
                    ;
                temp = dup(0);
            }
        }
        else if (i != 0 && !strcmp(av[i], "|"))
        {
            pipe(fd);
            if (fork() == 0)
            {
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
                av[i] = 0;
                dup2(temp, 0);
                close(temp);
                execve(av[0], av, env);
                put_error("cannot execute ");
                put_error(av[0]);
                put_error("\n");
                return (1);
            }
        }
        else
        {
            close(temp);
            close(fd[1]);
            temp = fd[0];
        }
    }
    close(temp);
    return (0);
}
