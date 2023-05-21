/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pealexan <pealexan@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/21 18:36:11 by pealexan          #+#    #+#             */
/*   Updated: 2023/05/21 18:45:47 by pealexan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell2.h"

void    put_error(char *str)
{
    while (*str)
        write(2, str++, 1);
}

int main(int ac, char **av, char **env)
{
    int fd[2];
    int temp;
    int i = 0;

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
                put_error("error: cd: bad arguments\n");
            else
            {
                if (chdir(av[1]) != 0)
                {
                    put_error("error: cd: cannot change directory to ");
                    put_error(av[1]);
                    write(2, "\n", 1);
                }
            }
        }
        else if(i != 0 && (av[i] == 0 || !strcmp(av[i], ";")))
        {
            if (fork() == 0)
            {
                av[i] = 0;
                dup2(temp, 0);
                close(temp);
                execve(av[0], av, env);
                put_error("error: cannot execute ");
                put_error(av[0]);
                write(2, "\n", 1);
                return (1);
            }
            else
            {
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
                close(fd[1]);
                close(fd[0]);
                av[i] = 0;
                dup2(temp, 0);
                close(temp);
                execve(av[0], av, env);
                put_error("error: cannot execute ");
                put_error(av[0]);
                write(2, "\n", 1);
                return (1);
            }
            else
            {
                close(temp);
                close(fd[1]);
                temp = dup(fd[0]);
            }
        }
    }
    close(temp);
    return (0);
}