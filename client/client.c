#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h> 
#include <netdb.h> 

#define DATA_BUFFER 5000

int main (int argc, char*argv[]) {
    int sudo = getuid();
    int fd, ret_val;
    struct sockaddr_in saddr;
    struct hostent *local_host; /* need netdb.h for this */
    char message[50];
    char auth[50];

    strcpy(auth,"");

    sprintf(auth,"%s %s %s %s %s",argv[0],argv[1],argv[2],argv[3],argv[4]); 

    /* Step1: create a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        return -1;
    }
    
    printf("Created a socket with fd: %d %s\n", fd, auth);

    /* Let us initialize the server address structure */
    saddr.sin_family = AF_INET;         
    saddr.sin_port = htons(7000);     
    local_host = gethostbyname("127.0.0.1");
    saddr.sin_addr = *((struct in_addr *)local_host->h_addr);

    /* Step2: connect to the TCP server socket */
    ret_val = connect(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val == -1) {
        fprintf(stderr, "connect failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }

    if(sudo == 0){
        puts("run as sudo");
        strcpy(message,"0");
        ret_val = send(fd,message, sizeof(message), 0);
        // sudo privilage
    }else{
        puts("not as sudo");
        ret_val = send(fd,auth, sizeof(auth), 0);
        // non-sudo privilage
    }

    while (1)
    {
        /* Next step: send some data */
        gets(message);
        ret_val = send(fd,message, sizeof(message), 0);
        if(strstr(message,"exit")){
            puts("putus");
            close(fd);
            break;
        }
        printf("Successfully sent data (len %d bytes): %s\n", 
                    ret_val, message);
        ret_val = recv(fd, message, sizeof(message), 0);
        if(ret_val>0)
            puts(message);
    }

    /* Last step: close the socket */
    close(fd);
    return 0;
}