
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>

#define BUFLEN 512

int main(int argc, char *argv[])
{
    struct sockaddr_in si_other;
    int sock, slen=sizeof(si_other);
    char buf[BUFLEN];
    int port, tries, delay, ii;
    char *server;
    
    port = 3333;
    tries = 1;
    delay = 1000;
    server = NULL;
    switch (argc) {
        default:
        case 5:
            delay = atoi(argv[4]);
        case 4:
            tries = atoi(argv[3]);
        case 3:
            port = atoi(argv[2]);
        case 2:
            server = argv[1];
            break;
        case 1:
        case 0:
            fprintf(stderr, "Usage: %s server_ip <port=%d> <tries=%d> <delay=%d ms>\n",
                    argv[0], port, tries, delay);
            return 1;
    }
    
    printf("Info: %s server=%s port=%d tries=%d delay=%d ms\n",
                argv[0], server, port, tries, delay);

    if ( (sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        perror("socket() failed");
        return 1;
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port);

    if (inet_aton(server , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        return 1;
    }

    ii = 0;
    printf("Progres: ");
    fflush(stdout);
    while (1)
    {
        snprintf(buf, sizeof(buf), "test-msg-%d\n", ii);

        // send data with '\0' included.
        if (sendto(sock, buf, strlen(buf)+1, 0, (struct sockaddr *) &si_other, slen)==-1)
        {
            fprintf(stderr, "sendto() failed\n");
            return 1;
        }
        printf("*");
        fflush(stdout);
        if (++ii >= tries)
            break;
        usleep(delay*1000);
    }
    printf("\n");

    close(sock);
    return 0;
}
