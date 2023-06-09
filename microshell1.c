/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pealexan <pealexan@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/21 18:23:30 by pealexan          #+#    #+#             */
/*   Updated: 2023/05/22 12:38:53 by pealexan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell1.h"

static void put_error(char *str)
{
    while (*str)
        write(2, str++, 1);
}

int main(int argc, char **argv, char **env)
{
    int temp;
    int fd[2];
    int i = 0;
    (void)argc;

    temp = dup(0);
    while (argv[i] && argv[i + 1])
    {
        i = 0;
        argv = &argv[i + 1];
        while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
            i++;
        if (!strcmp(argv[0], "cd"))
        {
            if (i != 2)
                put_error("error: cd: bad arguments\n");
            else
            {
                if (chdir(argv[1]) != 0)
                {
                    put_error("error: cd: cannot change directory to ");
                    put_error(argv[1]);
                    write(2, "\n", 1);
                }
            }
        }
        else if (i != 0 && (argv[i] == 0 || !strcmp(argv[i], ";")))
        {
            if (fork() == 0)
            {
                argv[i] = 0;
                dup2(temp, 0);
                close(temp);
                execve(argv[0], argv, env);
                put_error("error: cannot execute ");
                put_error(argv[0]);
                write(2, "\n", 1);
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
        else if (i != 0 && !strcmp(argv[i], "|"))
        {
            pipe(fd);
            if (fork() == 0)
            {
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
                argv[i] = 0;
                dup2(temp, 0);
                close(temp);
                execve(argv[0], argv, env);
                put_error("error: cannot execute ");
                put_error(argv[0]);
                write(2, "\n", 1);
                return (1);
            }
            else
            {
                close(fd[1]);
                close(temp);
                temp = fd[0];
            }
        }
    }
    close(temp);
    return (0);
}
