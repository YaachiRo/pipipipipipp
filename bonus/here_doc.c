
# include "pipex_bonus.h"

void heredoc_process(int ac, char **av, t_var *var)
{
	(void)ac;

	char *line;
    while(1)
    {
        write(1, "heredoc>", 8);
        line = get_next_line(0);
        if (!line)
        {
            perror("");
            exit(1);
        }
        if(ft_strnstr(line, av[2], ft_strlen(av[2])))
            break;
        else
        {
            write(var->in_file ,line ,ft_strlen(line));
            free(line);
        }
    }
    free(line);
    close(var->in_file);
}

void	init(int ac, char **av, char **env, t_var *var)
{
	var->env = env;
	var->in_file = open("tmp", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	var->out_file = open(av[ac - 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (var->in_file == -1)
		perror("");
	if (var->out_file == -1)
	{
		perror("");
		exit(1);
	}
	var->old_fd = var->in_file;
    heredoc_process(ac, av, var);
	var->paths = get_path(env);
	if (var->paths == NULL)
		error("path is empty\n", 1);
}

void here_doc(int ac, char **av, char **env)
{
   	t_var	var;

	var.index = 3;
	if (ac < 6)
		error("heredoc Arguments are not Valid\n", 1);
	init(ac ,av , env, &var);
	while (var.index <= (ac - 2))
	{
		if (pipe(var.fd) == -1)
			error("Pipe failed\n", 1);
		var.pid = fork();
		if (var.pid == -1)
			error("failed to fork child process\n", 1);
		else if (var.pid == 0)
			child_process(&var, ac, av);
		else
		{
			waitpid(var.pid, &var.status, 0);
			close(var.fd[1]);
			var.old_fd = var.fd[0];
			var.index++;
		}
	}
	if (WIFEXITED(var.status))
		exit(WEXITSTATUS(var.status)); 
}