
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#define LINELEN (80)
#define LINELEN_SMALL (2)

char *mygets(char *buf, int len);
int mygeti();

int main()
{
    // fd[0] - read
    // fd[1] - write
    char buf [LINELEN_SMALL];
    char *cmpstr[] = {"lexcmp", "lencmp"};
    int veclen = sizeof(cmpstr) / sizeof(char *);
    char str1[LINELEN + 1];
    char str2[LINELEN + 1];
    int index;
    int fd1[2];
    int fd2[2];
    int fd3[2];
    if (pipe(fd1) == -1 || pipe(fd2) == -1 || pipe(fd3) == -1)
    {
        perror("pipe");
        return -2;
    }
    switch (fork())
    { // lexcmp {
    case -1:
        perror("fork");
        return -2;
    case 0:

        if (close(fd3[1]) == -1 || close(fd3[0]) == -1 || close(fd1[1]) || close(fd2[0]))
            exit(-2);
        if (fd1[0] != STDIN_FILENO)
        { 
            if (dup2(fd1[0], STDIN_FILENO) == -1)
                perror("dup2 -first child - STDIN_FILENO");
            if(close(fd1[0])==-1)
            	exit(-2);  
        }
        if (fd2[1] != STDOUT_FILENO)
        { 
            if (dup2(fd2[1] , STDOUT_FILENO ) == -1)
                perror("dup2 -first child - STDOUT_FILENO");
	     if(close(fd2[1])==-1)
            	exit(-2);
        }
        
        execlp("./loopcmp","loopcmp", "lexcmp", NULL);
        perror("execlp");
        return -2;

    default: /* Parent falls through */
        break;
    }
    switch (fork())
    { // lencmp {
    case -1:
        perror("fork");
        return -2;
    case 0:

        if (close(fd1[1]) == -1 || close(fd1[0]) == -1 || close(fd2[0]) == -1 || close (fd3[1]))
            perror("close other pipes");
        
	 if (fd2[1] != STDOUT_FILENO)
        { /* Defensive check */
            if (dup2(fd2[1] , STDOUT_FILENO) == -1)
                perror("dup2 -second child - STDOUT_FILENO");
            if(close(fd2[1])==-1)
            	exit(-2);
            
        }
        if (fd3[0] != STDIN_FILENO)
        { /* Defensive check */
            if (dup2(fd3[0], STDIN_FILENO) == -1)
                perror("dup2 -second child - STDIN_FILENO");
            if(close(fd3[0])==-1)
            	exit(-2);
        }
        execlp("./loopcmp","loopcmp", "lencmp", NULL);
        perror("execlp");
        return -2;

    default: /* Parent falls through */
        break;
    }
    /* Parent  */
    if (close(fd1[0]) || close(fd2[1]) || close(fd3[0]))
		exit(-2);
    while (1)
    {
        
        printf("Please enter first string:\n");
        if (mygets(str1, LINELEN) == NULL)
            break;

        printf("Please enter second string:\n");

        if (mygets(str2, LINELEN) == NULL)
            break;
        do
        {
            printf("Please choose:\n");
            for (int i = 0; i < veclen; i++)
                printf("%d - %s\n", i, cmpstr[i]);
                index = mygeti();
        } while ((index < 0) || (index >= 2));
        
        str1[strlen(str1)] = '\n';
	    str2[strlen(str2)] = '\n';
        if (index == 0) // lexcmp
        {
            
            if (write(fd1[1], str1, strlen(str1)) == -1)
            {
                printf("can't write str1\n");
                break;
            }

            if (write(fd1[1], str2, strlen(str2)) == -1)
            {
                printf("can't write str2\n");
                break;
            }
        }
        if (index == 1) // lencmp
        {
           
            if (write(fd3[1], str1, strlen(str1)) == -1)
            {
                printf("can't write str1\n");
                exit(-2);
            }
            if (write(fd3[1], str2, strlen(str2)) == -1)
            {
                printf("can't write str2\n");
                 exit(-2);
            }
        }
        if (read(fd2[0] , buf , 2*sizeof(char))==-1)
            exit(-2);
		str1[strlen(str1) - 1] = '\0';
		str2[strlen(str2) - 1] = '\0';
		printf("%s(%s, %s) == %d\n", cmpstr[index], str1, str2, atoi(buf));
        fflush(stdout);
    }
    exit(-2);
    return 0;
}

char *mygets(char *buf, int len)
{
    char *retval;

    retval = fgets(buf, len, stdin);
    buf[len] = '\0';
    if (buf[strlen(buf) - 1] == 10) /* trim \r */
        buf[strlen(buf) - 1] = '\0';
    else if (retval)
        while (getchar() != '\n')
            ; /* get to eol */
    fflush(stdin);
    return retval;
}

int mygeti()
{
    int ch;
    int retval = 0;

    while (isspace(ch = getchar()))
        ;
    while (isdigit(ch))
    {
        retval = retval * 10 + ch - '0';
        ch = getchar();
    }
    while (ch != '\n')
        ch = getchar();
    return retval;
}
