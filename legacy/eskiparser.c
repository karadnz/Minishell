/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkaraden <mkaraden@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/27 20:06:57 by mkaraden          #+#    #+#             */
/*   Updated: 2023/04/04 22:59:08 by mkaraden         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */





typedef enum
{
	NODE_NONE, //**
	NODE_COMMAND,
	NODE_PIPE,
	NODE_REDIRECT_IN,
	NODE_REDIRECT_OUT,
	NODE_REDIRECT_APPEND,
	NODE_REDIRECT_HEREDOC
}				NodeType;

typedef struct Node
{
	NodeType	type;
	
	char		**args;
	int			num_args;
	
	
	char		**infile;
	char		**outfile;
	
	int			inf_count;
	int			out_count;

	struct Node	*next;
}				Node;

Node			*parse_main(const char **input);

Node			*create_node();
void			free_node(Node *node);
void			print_parser(Node *head);
void			expand_parsed_nodes(Node *head);


#include "minishell.h"

Node	*create_node()
{
	Node	*node;

	node = (Node *)malloc(sizeof(Node));
	node->type = NODE_NONE; //
	node->num_args = 0;
	node->inf_count = 0;
	node->out_count = 0;
	node->args = NULL;
	node->outfile = NULL;
	node->infile = NULL;
	node->next = NULL;
	return (node);
}

Node	*parse_main(const char **input)
{
	Node *head;
	Node *iter;
	Token *token;

	head = create_node();
	iter = head;
	token = get_next_token(input);
	while (token->type != TOKEN_EOF)
	{
		
		//printf("Token type: %d, args: %s\n", token->type, token->value);
		if (token->type == TOKEN_WORD)
		{
			iter->args = (char **)realloc(iter->args, (iter->num_args + 1)
					* sizeof(char *));
			iter->args[iter->num_args] = strdup(token->value);
			iter->num_args++;
		}

		else if (token->type == TOKEN_PIPE)
		{
			//iter->type = NODE_PIPE; ***
			iter->next = create_node();
			iter = iter->next;
		}

		else if (token->type == TOKEN_O)
		{
			iter->type = NODE_REDIRECT_OUT;
			token = get_next_token(input);
			if (token->type == TOKEN_WORD)
			{
				iter->outfile = (char **)realloc(iter->outfile, (iter->out_count+ 1)
					* sizeof(char *));
				iter->outfile[iter->out_count] = strdup(token->value); //gordugun yerde olustur
				iter->out_count++;
			 
			}
			else
			{
				fprintf(stderr, "Error: Missing file for output redirection\n");
				//free_token(token);
				//free_command(head);
				return (NULL);
			}
		}
		else if (token->type == TOKEN_O_O)
		{
			iter->type = NODE_REDIRECT_APPEND;
			token = get_next_token(input);
			if (token->type == TOKEN_WORD) //gordugun yerde olustur
			{
				iter->outfile = (char **)realloc(iter->outfile, (iter->out_count+ 1)
					* sizeof(char *));
				iter->outfile[iter->out_count] = strdup(token->value); //gordugun yerde olustur
				iter->out_count++;
			}
			else
			{
				fprintf(stderr, "Error: Missing file for output redirection\n");
				//free_token(token);
				//free_command(head);
				return (NULL);
			}
		}

		else if (token->type == TOKEN_I)
		{
			iter->type = NODE_REDIRECT_IN;
			token = get_next_token(input);
			if (token->type == TOKEN_WORD)
			{
				iter->infile = (char **)realloc(iter->infile, (iter->inf_count+ 1)
					* sizeof(char *));
				iter->infile[iter->inf_count] = strdup(token->value); //gordugun okumaya calis
				iter->inf_count++; 
			}
			else
			{
				fprintf(stderr, "Error: Missing file for input redirection\n");
				//free_token(token);
				//free_command(head);
				return (NULL);
			}
			
		}

		else if (token->type == TOKEN_I_I)
		{
			iter->type = NODE_REDIRECT_HEREDOC;
			token = get_next_token(input);
			if (token->type == TOKEN_WORD)
			{				
				iter->infile = (char **)realloc(iter->infile, (iter->inf_count+ 1)
					* sizeof(char *));
				iter->infile[iter->inf_count] = strdup(token->value); //gordugun okumaya calis
				iter->inf_count++; 
			}
			else
			{
				fprintf(stderr, "Error: Missing file for input redirection\n");
				//free_token(token);
				//free_command(head);
				return (NULL);
			}
			
		}
		token = get_next_token(input);
	}

	return (head);

}


void print_parser(Node *head)
{
	Node *iter = head;

	int i = 0;
	while(iter != NULL)
	{
		printf("Node: %d \n", i);
		printf("Type: %d \n", iter->type);
		printf("args: ");
		for (int i = 0; i < iter->num_args; i++)
		{
			printf("%s , ", iter->args[i]);
		}
		printf("\n");

		printf("infile: ");
		for (int i = 0; i < iter->inf_count; i++)
		{
			printf("%s , ", iter->infile[i]);
		}
		printf("\n");

		printf("outfile: ");
		for (int i = 0; i < iter->out_count; i++)
		{
			printf("%s , ", iter->outfile[i]);
		}
		printf("\n\n\n");
		iter=iter->next;
		i++;
	}
}

void expand_parsed_nodes(Node *head)
{
	int		i;
	Node	*iter;
	
	iter = head;
	i = 0;
	while(iter != NULL)
	{
		i = 0;
		while (i < iter->num_args)
		{
			iter->args[i] = get_expanded(iter->args[i]);
			i++;
		}
		i = 0;
		while (i < iter->inf_count)
		{
			iter->infile[i] = get_expanded(iter->infile[i]);
			i++;
		}
		i = 0;
		while (i < iter->out_count)
		{
			iter->outfile[i] = get_expanded(iter->outfile[i]);
			i++;
		}
		iter=iter->next;
	}
}
