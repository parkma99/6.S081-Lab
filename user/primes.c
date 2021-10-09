#include "kernel/types.h"
#include "user/user.h"
void primes(int p[]);
int main(int argc, char const *argv[])
{
    int p[2];
    int i=2;
    pipe(p);
    if(fork()==0){
        primes(p);
    }else{
        close(p[0]);
        for(;i<36;i++){
            write(p[1],&i,sizeof(int));
        }
        close(p[1]);
        wait((int*)0);
    }
    exit(0);
}

void primes(int p[])
{
    close(p[1]);
    int num, prime;
    int cp[2];
    if (read(p[0], &num, sizeof(int)) == 0)
    {
        close(p[0]);
        exit(0);
    }
    else
    {
        prime = num;
        fprintf(1, "prime %d\n", prime);
    }
    pipe(cp);
    if (fork() == 0)
    { //right childer process
        close(p[0]);
        close(cp[1]);
        primes(cp);
    }
    else
    {
        close(cp[0]);
        while (read(p[0], &num, sizeof(int)))
        {
            if (num % prime != 0)
            {
                write(cp[1],&num,sizeof(int));
            }
        }
        close(p[0]);
        close(cp[1]);
        wait((int*)0);
    }
}