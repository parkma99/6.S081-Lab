#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]){
    fprintf(2,"%d\n",uptime());
    exit(0);
}