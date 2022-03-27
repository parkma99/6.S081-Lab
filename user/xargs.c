#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{

    char *x_argv[MAXARG];
    int i;
    char buf[512];
    if (argc < 2)
    {
        fprintf(2, "usage: xargs your_command\n");
        exit(1);
    }
    if (argc + 1 > MAXARG)
    {
        fprintf(2, "too many args\n");
        exit(1);
    }
    for (i = 1; i < argc; i++)
    {
        x_argv[i - 1] = argv[i];
    }
    x_argv[argc] = 0;

    while (1)
    {
        i = 0;
        while (!(read(0, &buf[i], sizeof(char)) == 0 || buf[i] == '\n'))
        {
            i++;
        }
        if (i == 0) break;
        buf[i] = 0;
        x_argv[argc-1] = buf;
        if(fork()==0){
            exec(x_argv[0],x_argv);
            exit(0);
        }else{
            wait((int *)0);
        }
    }
    exit(0);
}
