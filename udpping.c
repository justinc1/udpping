
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>

#define BUFLEN 512

int hostname_to_ip(const char * hostname , char* ip, size_t ip_len)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ((he = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        fprintf(stderr, "gethostbyname failed\n");
        return 1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;
    for (i = 0; addr_list[i] != NULL; i++)
    {
        // Return the first one;
        strncpy(ip, inet_ntoa(*addr_list[i]), ip_len);
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    struct sockaddr_in si_other;
    int sock, slen=sizeof(si_other);
    char buf[BUFLEN];
    int port, tries, delay, ii;
    char *server_name;
    int ret;
    char server_ip[100];
    
    port = 3333;
    tries = 1;
    delay = 1000;
    server_name = NULL;
    switch (argc) {
        default:
        case 5:
            delay = atoi(argv[4]);
        case 4:
            tries = atoi(argv[3]);
        case 3:
            port = atoi(argv[2]);
        case 2:
            server_name = argv[1];
            break;
        case 1:
        case 0:
            fprintf(stderr, "Usage: %s server_ip <port=%d> <tries=%d> <delay=%d ms>\n",
                    argv[0], port, tries, delay);
            return 1;
    }
    
    printf("Info: %s server=%s port=%d tries=%d delay=%d ms\n",
                argv[0], server_name, port, tries, delay);

    if ( (sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        perror("socket() failed");
        return 1;
    }

    ret = hostname_to_ip(server_name, server_ip, sizeof(server_ip));
    if (ret != 0) {
        return ret;
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port);

    if (inet_aton(server_ip , &si_other.sin_addr) == 0)
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
