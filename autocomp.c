#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>

//define a size for the array, plus an extra space for the apostrophe
//define max word length
#define LENGTH 32
#define SIZE 27

//define ANSI color codes
#define RED     "\e[38;2;255;255;255;1m\e[48;2;220;20;60;1m"
#define GREEN   "\e[38;2;255;255;255;1m\e[48;2;106;170;100;1m"
#define RESET   "\e[0;39m"

typedef struct node
{
    bool word;
    struct node *next[SIZE];
}
node;

//initialize nodes to null
node *root[SIZE - 1] = {NULL}; //root array initialized to SIZE-1, no word starts with an apostrophe
node *h1, *h2 = NULL;

bool create(int k);
node *insert(void);
bool add(char *word);
bool word_exists(char *word);
void unload(node *ptr);
void delete_trie(void);
void autocomplete(char *input);
char *get_string(void);
bool gibberish(char *input);


int main(int argc, char *argv[])
{
    //ensure correct usage
    if (argc != 2)
    {
        printf("Usage: ./autocomplete -s OR -i\n");
        return 5;
    }

    if (strcmp(argv[1], "-i") != 0 && strcmp(argv[1], "-s") != 0)
    {
        printf("Usage: ./autocomplete -s OR -i\n");
        return 5;
    }
    else if (strcmp(argv[1], "-s") == 0)
    {
        //create root of trie (all letters, no apostrophes)
        for (int i = 'a'; i <= 'z'; i++)
        {
            if (! create(i - 'a'))
            {
                printf("Memory allocation failed\n");
                return 1;
            }
        }

        //add words from dictionary into trie
        //open dictionary file to read
        FILE *file = fopen("dict.txt", "r");
        if (file == NULL)
        {
            printf("Could not open dictionary\n");
            return 2;
        }

        //read strings into buffer word
        char entry[LENGTH + 1];
        while (fscanf(file, "%s", entry) != EOF)
        {
            //add dictionary to trie, free structure if a malloc fails
            if (!add(entry))
            {
                printf("Memory allocation failed\n");
                delete_trie();
                return 3;
            }
        }
        fclose(file);
        //welcome message
        printf(RED"Welcome to Autocomplete! Type '.quit' at any time to exit"RESET"\n");
        //ask for user input
        while (true)
        {
            char *input = get_string();
            int empty = strcmp(input, ".quit");
            if (empty == 0)
            {
                delete_trie();
                free(input);
                break;
            }
            else if (input != NULL)
            {
                if (!gibberish(input) && word_exists(input))
                {
                    printf(GREEN"Results for words starting with '%s':"RESET"\n", input);
                    autocomplete(input);
                    printf(RED"End of results for '%s'. Type '.quit' at any time to exit"RESET"\n", input);
                    free(input);
                }
                else
                {
                    printf("Input does not exist in dictionary\n");
                    free(input);
                }
            }
            else
            {
                delete_trie();
                printf("Memory allocation failed\n");
                return 4;
            }
        }
    }
    else if (strcmp(argv[1], "-i") == 0)
    {
        FILE *f = fopen("dict.txt", "r");
        if (f == NULL)
        {
            printf("Could not open dictionary\n");
            return 2;
        }
        //welcome message
        printf(RED"Welcome to Autocomplete! Type '.quit' at any time to exit"RESET"\n");
        //ask for user input
        while (true)
        {
            char word[LENGTH];
            char *fragment = get_string();
            int empty = strcmp(fragment, ".quit");
            //quit if '.quit' is given
            if (empty == 0)
            {
                fclose(f);
                free(fragment);
                break;
            }
            else if (fragment == NULL)
            {
                printf("Memory allocation failed\n");
                fclose(f);
                free(fragment);
                return 6;
            }
            else if (!gibberish(fragment))
            {
                printf(GREEN"Results for words including '%s':"RESET"\n", fragment);
                char check[LENGTH] = {'a'};
                while (fscanf(f,"%s", word) != EOF)
                {
                    for (int j = 0; j < strlen(word); j++)
                    {
                        for (int i = 0; i < strlen(fragment); i++)
                        {
                            check[i] = word[i + j];
                        }
                        if (strcasecmp(check, fragment) == 0)
                        {
                            printf("%s\n", word);
                        }
                    }
                }
                printf(RED"End of results for '%s'. Type '.quit' at any time to exit"RESET"\n", fragment);
                free(fragment);
                fseek(f, 0 ,SEEK_SET);
            }
            else
            {
                printf("Input does not exist in dictionary\n");
                free(fragment);
            }

        }
    }
    return 0;
}


bool create(int k)
{
    node *n = malloc(sizeof(node));
    //prevent memory leak if some malloc fails
    if (n == NULL)
    {
        delete_trie();
        return false;
    }
    for (int i = 0; i < SIZE; i++)
    {
        n->word = false;
        n->next[i] = NULL;
    }
    root[k] = n;
    return true;
}


node *insert()
{
    node *new = malloc(sizeof(node));
    //return NULL if malloc fails, this ensures add function returns false
    if (new == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < SIZE; i++)
    {
        new->word = false;
        new->next[i] = NULL;
    }
    return new;
}


bool add(char *word)
{
    //keep last letter for updating word bool in struct
    int last = strlen(word) - 1;
    for (int j = 0; word[j] != '\0'; j++)
    {
        if (j == 0)
        {
            h1 = root[word[j] - 'a'];
            h2 = root[word[j] - 'a'];
        }
        //check if letter
        else if ((word[j] - 'a') >= 0)
        {
            if ((h1->next[word[j] - 'a']) == NULL)  // if branch doesn' t exist,  create it
            {
                h2 = insert();
                if (h2 == NULL)
                {
                    return false;
                }
                h1->next[word[j] - 'a'] = h2;
                h1 = h2;
            }
            else                                    // if branch exists, move on
            {
                h2 = h2->next[word[j] - 'a'];
                h1 = h2;
            }
        }
        //ignore space
        else if (word[j] == ' ')
        {
            continue;
        }
        //case for apostrophe character
        else
        {
            //make use of extra element in array
            h2 = insert();
            if (h2 == NULL)
            {
                return false;
            }
            h1->next[SIZE - 1] = h2;
            h1 = h2;
        }
        //if last letter, update word bool to true
        if (j == last)
        {
            h1->word = true;
        }
    }
    return true;
}


bool word_exists(char *word)
{
    //set traversal pointer to first letter (will never be null)
    node *ptr = root[tolower(word[0]) - 'a'];
    for (int i = 1; word[i] != '\0'; i++)
    {
        if ((word[i] - 'a') >= 0)
        {
            ptr = ptr->next[tolower(word[i]) - 'a'];
        }
        //ignore spaces
        else if (word[i] == ' ')
        {
            continue;
        }
        else if (word[i] == 0x27)
            //case where character is apostrophe
        {
            ptr = ptr->next[SIZE - 1];
        }
        if (ptr == NULL)
        {
            return false;
        }
    }
    //if all nodes exist
    return true;
}


void unload(node *ptr)
{
    //free a branch
    for (int i = 0; i < SIZE; i++)
    {
        if ((ptr->next[i]) != NULL)
        {
            unload(ptr->next[i]);
            free(ptr->next[i]);
        }
    }
}


void delete_trie()
{
    //free all branches
    for (int k = 'a'; k <= 'z'; k++)
    {
        node *ptr = root[k - 'a'];
        if (ptr != NULL)
        {
            unload(ptr);
        }
    }
    //free root array
    for (int j = 0; j < SIZE - 1; j++)
    {
        node *ptr2 = root[j];
        if (ptr2 != NULL)
        {
            free(ptr2);
        }
    }
}


void autocomplete(char *input)
{
    //set traversal to starting letter
    node *ptr = root[tolower(input[0]) - 'a'];
    for (int i = 1; input[i] != '\0'; i++)
    {
        //traverse structure until  the end of the input
        ptr = ptr->next[tolower(input[i]) - 'a'];
    }
    //if reached an actual word, print it
    if (ptr->word)
    {
        printf("%s\n", input);
    }
    //get inside all branches and print every word that completes the input
    for (int i = 0; i < SIZE; i++)
    {
        if (ptr->next[i] != NULL)
        {
            char c = i + 'a';
            char word[LENGTH];
            strcpy(word, input);
            int l = strlen(input);
            word[l] = c;
            word[l + 1] = '\0';
            autocomplete(word);
        }
    }
}


char *get_string(void)
{
    char *guess;
    char c;
    int size = 2;
    guess = malloc(size);
    if (guess == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }
    int k = 0;
    while ((c = getchar()) != '\n')
    {
        guess[k] = tolower(c);
        guess[k + 1] = '\0';
        size++;
        k++;
        char *newMem = realloc(guess, size);
        if (newMem == NULL)
        {
            printf("Memory allocation failed\n");
            return NULL;
        }
        guess = newMem;
    }
    return guess;
}


bool gibberish(char *input)
{
    for (int i = 0; input[i] != '\0'; i++)
    {
        if (!isalpha(input[i]))
        {
            return true;
        }
    }
    return false;
}
