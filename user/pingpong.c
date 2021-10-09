#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
    int pid;
    int pipe_c2f[2];
    int pipe_f2c[2];
    char buf = 'B';
    
    pipe(pipe_c2f);
    pipe(pipe_f2c);
    pid = fork();
    if (pid < 0)
    {
        fprintf(2, "fork error..\n");
        close(pipe_f2c[0]);
        close(pipe_f2c[1]);
        close(pipe_c2f[0]);
        close(pipe_c2f[1]);
        exit(1);
    }
    else if (pid == 0)
    {
        close(pipe_c2f[0]);
        close(pipe_f2c[1]);

        read(pipe_f2c[0], &buf, sizeof(char));
        fprintf(1, "%d: received ping\n", getpid());
        write(pipe_c2f[1], &buf, sizeof(char));
        close(pipe_c2f[1]);
        close(pipe_f2c[0]);

        exit(0);
    }
    else
    {
        close(pipe_c2f[1]);
        close(pipe_f2c[0]);

        write(pipe_f2c[1], &buf, sizeof(char));
        read(pipe_c2f[0], &buf, sizeof(char));
        fprintf(1, "%d: received pong\n", getpid());
        close(pipe_c2f[0]);
        close(pipe_f2c[1]);

        exit(0);
    }
}
