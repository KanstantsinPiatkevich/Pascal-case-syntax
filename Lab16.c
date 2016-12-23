#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <conio.h>

typedef enum {KEY_WORD, ID, NUMBER, SEMICOLON, COLON, RANGE, COMMA, OPERATOR, END} TClass;

typedef struct
{
	TClass tokenClass;
	char str[20];
} TToken;

bool isLetter(char symbol)
{
	if (symbol >= 'a' && symbol <= 'z' || symbol >= 'A' && symbol <= 'Z')
		return true;
	else
		return false;
}

bool isDigit(char symbol)
{
	if (symbol >= '0' && symbol <= '9')
		return true;
	else
		return false;
}

bool isError = false;

TToken *findTokens(char *str, int length, TToken *token, int *tokenAmount)
{
	int i = 0, j;

	while (i < length)
	{
		if (isDigit(str[i]))
		{
			token = (TToken*)realloc(token, *tokenAmount * sizeof(TToken));
			j = 0;
			while (isDigit(str[i]))
			{
				token[*tokenAmount - 1].str[j] = str[i];
				i++;
				j++;
			}
			token[*tokenAmount - 1].str[j] = '\0';
			token[*tokenAmount - 1].tokenClass = NUMBER;
			(*tokenAmount)++;
		}

		else if (isLetter(str[i]))
		{
			token = (TToken*)realloc(token, *tokenAmount * sizeof(TToken));
			j = 0;
			while (isLetter(str[i]) || isDigit(str[i]) || str[i] == '_')
			{
				token[*tokenAmount - 1].str[j] = str[i];
				i++;
				j++;
			}
			token[*tokenAmount - 1].str[j] = '\0';
			if (strcmp(token[*tokenAmount - 1].str, "case") == 0 || strcmp(token[*tokenAmount - 1].str, "of") == 0 || strcmp(token[*tokenAmount - 1].str, "else") == 0 || strcmp(token[*tokenAmount - 1].str, "end") == 0)
				token[*tokenAmount - 1].tokenClass = KEY_WORD;
			else
				token[*tokenAmount - 1].tokenClass = ID;
			(*tokenAmount)++;
		}

		else if (str[i] == ':')
		{
			token = (TToken*)realloc(token, *tokenAmount * sizeof(TToken));
			token[*tokenAmount - 1].str[0] = ':';
			token[*tokenAmount - 1].str[1] = '\0';
			token[*tokenAmount - 1].tokenClass = COLON;
			i++;
			if (i != length)
				if (str[i] == '=')
				{
					token[*tokenAmount - 1].str[1] = '=';
					token[*tokenAmount - 1].str[2] = '\0';
					token[*tokenAmount - 1].tokenClass = OPERATOR;
					i++;
				}
			(*tokenAmount)++;
		}

		else if (str[i] == '+' || str[i] == '-' || str[i] == '/' || str[i] == '*' || str[i] == '=')
		{
			token = (TToken*)realloc(token, *tokenAmount * sizeof(TToken));
			token[*tokenAmount - 1].str[0] = str[i];
			token[*tokenAmount - 1].str[1] = '\0';
			token[*tokenAmount - 1].tokenClass = OPERATOR;
			(*tokenAmount)++;
			i++;
		}

		else if (str[i] == '.')
		{
			token = (TToken*)realloc(token, *tokenAmount * sizeof(TToken));
			i++;
			if (i != length)
				if (str[i] == '.')
				{
					token[*tokenAmount - 1].str[0] = '.';
					token[*tokenAmount - 1].str[1] = '.';
					token[*tokenAmount - 1].str[2] = '\0';
					token[*tokenAmount - 1].tokenClass = RANGE;
					i++;
				}
			(*tokenAmount)++;
		}

		else if (str[i] == ';')
		{
			token = (TToken*)realloc(token, *tokenAmount * sizeof(TToken));
			token[*tokenAmount - 1].str[0] = str[i];
			token[*tokenAmount - 1].str[1] = '\0';
			token[*tokenAmount - 1].tokenClass = SEMICOLON;
			(*tokenAmount)++;
			i++;
		}

		else if (str[i] == ',')
		{
			token = (TToken*)realloc(token, *tokenAmount * sizeof(TToken));
			token[*tokenAmount - 1].str[0] = str[i];
			token[*tokenAmount - 1].str[1] = '\0';
			token[*tokenAmount - 1].tokenClass = COMMA;
			(*tokenAmount)++;
			i++;
		}

		else if (str[i] >= 0 && str[i] <= 32)
		{
			i++;
		}

		else
		{
			i = length;
			isError = true;
		}
	}

	token = (TToken*)realloc(token, *tokenAmount * sizeof(TToken));
	token[*tokenAmount - 1].tokenClass = END;
	token[*tokenAmount - 1].str[0] = '$';
	token[*tokenAmount - 1].str[1] = '\0';

	return token;
}

bool checkExpession(int &i, TToken *token)
{
	int rang = 0;

	while (token[i].tokenClass == NUMBER || token[i].tokenClass == ID || token[i].tokenClass == OPERATOR)
	{
		if (token[i].tokenClass == NUMBER || token[i].tokenClass == ID)
			rang += 1;
		else
			rang -= 1;
		i++;
	}

	if (rang != 1)
		return false;
	else
		return true;
}

bool checkCorrectness(TToken *token, int *tokenAmount)
{
	bool isLoop;

	if (strcmp(token[0].str, "case") != 0)
	{
		return false;
	}

	int i = 1;

	if (!checkExpession(i, token))
		return false;

	if (strcmp(token[i].str, "of") != 0)
		return false;

	do
	{
		do
		{
			i++;

			if (token[i].tokenClass != NUMBER)
				return false;

			i++;

			if (token[i].tokenClass == RANGE)
			{
				i++;

				if (token[i].tokenClass != NUMBER)
					return false;
				
				i++;

			}
		} while (token[i].tokenClass == COMMA);
		if (token[i].tokenClass != COLON)
			return false;

		i++;

		if (token[i].tokenClass != ID)
			return false;
		
		i++;
		
		if (strcmp(token[i].str, ":=") != 0)
			return false;
		
		i++;

		if (!checkExpession(i, token))
			return false;

		isLoop = false;
		if (i < *tokenAmount - 2)
			if (token[i].tokenClass == SEMICOLON && token[i + 1].tokenClass == NUMBER)
				isLoop = true;
	} while (isLoop);

	if (strcmp(token[i].str, ";") == 0)
		i++;
	else if (strcmp(token[i].str, "else") != 0)
		return false;

	if (strcmp(token[i].str, "else") == 0)
	{
		i++;

		if (token[i].tokenClass != ID)
			return false;
		
		i++;

		if (strcmp(token[i].str, ":=") != 0)
			return false;
		
		i++;

		if (!checkExpession(i, token))
			return false;

		if (token[i].tokenClass == SEMICOLON)
			i++;
	}

	if (strcmp(token[i].str, "end") != 0)
		return false;
	i++;
		
	if (strcmp(token[i].str, ";") != 0)
		return false;

	return true;
}

void main()
{
	TToken *token = NULL;
	int tokenAmount = 1;
	FILE *inFile;
	char fName[50];
	inFile = fopen("case.txt", "rt");

	char str[255];

	puts("Code:\n");
	while (!feof(inFile))
	{
		fgets(str, 255, inFile);

		printf("%s", str);
		token = findTokens(str, strlen(str), token, &tokenAmount);
	}
	if (isError)
		puts("\n\nError: Unknown character in code");
	else if (checkCorrectness(token, &tokenAmount))
	{
		puts("\n\nToken's list:\n");
		for (int i = 0; i < tokenAmount - 1; i++)
		{
			switch (token[i].tokenClass)
			{
			case KEY_WORD:
				printf("<key word, ");
				break;
			case ID:
				printf("<identifier, ");
				break;
			case NUMBER:
				printf("<number, ");
				break;
			case RANGE:
				printf("<range, ");
				break;
			case SEMICOLON:
				printf("<semicolon, ");
				break;
			case COLON:
				printf("<colon, ");
				break;
			case OPERATOR:
				printf("<operator, ");
				break;
			case COMMA:
				printf("<comma, ");
				break;
			}
			printf("%s>\n", token[i].str);
		}
	}
	else
	{
		puts("\n\nSyntax error");
	}
	puts("\nPress any key...");
	_getch();
}
