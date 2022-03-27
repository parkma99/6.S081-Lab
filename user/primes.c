#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
void primes(int p[2]){
    char prime ;
    if(read(p[0],&prime,1) == 0){
        close(p[0]);
        exit(0);
    }
    fprintf(2,"prime %d\n",prime);
    int cp[2];
    pipe(cp);
    if (fork()!= 0){
        close(cp[0]);
        char buf;
        while(read(p[0],&buf,1)!=0){
            if (buf%prime!=0){
                write(cp[1],&buf,1);
            }
        }
        close(p[0]);
        close(cp[1]);
        wait(0);
    }else{
        close(cp[1]);
        close(p[0]);
        primes(cp);
    }
    exit(0);
}
int main(int argc, char **argv){
    if (argc != 1){
        exit(1);
    }
    int p[2];
    pipe(p);

    if (fork()!=0){// parent process
        close(p[0]);
        for (char i = 2;i<35;i++){
            write(p[1],&i,1);
        }
        close(p[1]);
        wait((int*)0);
    }else{
        close(p[1]);
        primes(p);
    }
    wait(0);
    exit(0);
}
