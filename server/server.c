#include <stdio.h>
#include <netinet/in.h> 
#include <unistd.h> 
#include <wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <dirent.h>

#define DATA_BUFFER 5000
#define MAX_CONNECTIONS 10 
#define SUCCESS_MESSAGE "Your message delivered successfully"

void ForkWaitFunction(char bash[], char *arg[]){
    int status;
    pid_t child;
    child = fork();
    if(child == 0){
        execv(bash, arg);
    }
    else{
        ((wait(&status))>0);
    }
}

char pwd(){
    char cwd[PATH_MAX];
    // char *cwdString;
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return 0;
    }
    // strcpy(cwdString,cwd);
    return cwd[PATH_MAX];
}

void to(char *arg){
    // ke arg
    chdir(arg);
}

void down(){
    // ke bawah
    chdir("..");
}

void createDatabase(char *dbname){
    char *database = dbname;
    int check;
    check = mkdir(database,0777);
    if(!check){
        printf("Dir database created.\n");
    }else{
        printf("Dir database exist.\n"); 
    }
}

void createTable(char *tName){
    pwd();
    printf("%s\n",tName);
    char fileName[DATA_BUFFER];
    strcpy(fileName,tName);
    strcat(fileName,".csv");
    char *arg[] = {"touch",fileName,NULL};
    ForkWaitFunction("/usr/bin/touch",arg);
    memset(fileName,0,(strlen(fileName))*sizeof(char));
}

void setGrant(char* argGrant){
    int i = 0;
    char setter[80];
    char *token;
    char *arg[DATA_BUFFER];
    const char s[80] = " ";

    strcpy(setter,argGrant);

    /* get the first token */
    token = strtok(setter, s);
   
    /* walk through other tokens */
    while( token != NULL ) {
        arg[i]=token;
        // printf( "%s\n", token );    
        token = strtok(NULL, s);
        i++;
    }    
    sprintf(setter,"%s,%s\n",arg[4],arg[2]);
    to("dbAkun");
    FILE*fp;
    fp=fopen("akses.csv","a");
    if(strstr(arg[4],"sudo")){
        fprintf(fp,"%s",setter);        
    }else{
        fprintf(fp,"%s,%s\n%s","sudo",arg[2],setter);
    }
    fclose(fp);
    down();
    free(token);
    free(arg);
    strcpy(setter,"");
}

int getGrant(){
    return 1;
}

int auth(char *argAuth) {
    to("dbAkun");
    FILE *fp = fopen("akun.csv", "r");
    int is_auth = 0;
    char buffer[1024];
    while (fgets(buffer, 1024, fp) != NULL && is_auth == 0) {
        char f_username[1024], f_password[1024];
        char *token = strtok(buffer, ".");
        strcpy(f_username, token);
        token = strtok(NULL, "\n");
        strcpy(f_password, token);
        if (strcmp(username, f_username) == 0 && strcmp(password, f_password) == 0)
            is_auth = 1;
    }
    fclose(fp);
    return is_auth;
}

void setReg(char* argReg){
    int i = 0;
    char setter[80];
    char *token;
    char *arg[DATA_BUFFER];
    const char s[80] = " ";

    strcpy(setter,argReg);

    /* get the first token */
    token = strtok(setter, s);
   
    /* walk through other tokens */
    while( token != NULL ) {
        arg[i]=token;
        // printf( "%s\n", token );    
        token = strtok(NULL, s);
        i++;
    }    
    sprintf(setter,"%s,%s\n",arg[2],arg[5]);
    to("dbAkun");
    FILE*fp;
    fp=fopen("akses.csv","a");
    fprintf(fp,"%s",setter);      
    fclose(fp);
    down();
    free(token);
    free(arg);
    strcpy(setter,"");
}

void ddl(){

}

int create_tcp_server_socket() {
    struct sockaddr_in saddr;
    int fd, ret_val;

    /* Step1: create a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        return -1;
    }
    printf("Created a socket with fd: %d\n", fd);

    /* Initialize the socket address structure */
    saddr.sin_family = AF_INET;         
    saddr.sin_port = htons(7000);     
    saddr.sin_addr.s_addr = INADDR_ANY; 

    /* Step2: bind the socket to port 7000 on the local host */
    ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val != 0) {
        fprintf(stderr, "bind failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }

    /* Step3: listen for incoming connections */
    ret_val = listen(fd, 5);
    if (ret_val != 0) {
        fprintf(stderr, "listen failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }

    return fd;
}

void setup_epoll_connection(int epfd, int fd, struct epoll_event * event) {
    event->events = EPOLLIN;
    event->data.fd = fd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, event);
}


void start(){
    char *databases = "databases";
    char *dbAkun = "dbAkun";
    char *tAkun = "akun";
    char *tAkses = "akses";
    char *sudoGrant = "GRANT PERMISSION dbAkun INTO sudo";
    int check;
    createDatabase(databases);
    to(databases); 
    pwd();   
    createDatabase(dbAkun);
    to(dbAkun);
    pwd();
    createTable(tAkun);
    createTable(tAkses);
    down();
    pwd();
    setGrant(sudoGrant);
}

// void *server_main_routine(void *arg){
//     char buf[DATA_BUFFER];
//     int ret_val,timeout_msecs = 100;
//     int indeks;

//     // sid[*(int *)arg] = *(int *)arg;

//     while(1){
//         ret_val = recv(*(int *)arg, buf, DATA_BUFFER, 0);
//         // printf("%d\n", *(int *)arg);
//         if (ret_val > 0) {
//             printf("Returned fd is %d\n", *(int *)arg);
//             printf("Received data (len %d bytes, fd: %d): %s\n", ret_val, *(int *)arg, buf);
//             ret_val = send(*(int *)arg, SUCCESS_MESSAGE, sizeof(SUCCESS_MESSAGE), 0);
//             if(strstr(buf,"exit")){
//                 close(*(int *)arg);
//                 printf("bye!\n");
//                 pthread_exit(&ret_val);
//             }   
//         }
//     }
// }

int main () {
    struct epoll_event all_connections[MAX_CONNECTIONS], epoll_temp;
    struct sockaddr_in new_addr;
    
    int server_fd, new_fd, ret_val, temp_fd, i, timeout_msecs = 100;
    int epfd = epoll_create(1);
    
    char buf[DATA_BUFFER];

    socklen_t addrlen;
    pthread_t tid[MAX_CONNECTIONS];

    

    /* Get the socket server fd */
    server_fd = create_tcp_server_socket(); 

    if (server_fd == -1) {
        fprintf(stderr, "Failed to create a server\n");
        return -1; 
    }   

    setup_epoll_connection(epfd, server_fd, &epoll_temp);

    // struct thread_info *tinfo = calloc(num_threads, sizeof(*tinfo));
    start();

    while (1) {
        ret_val = epoll_wait(epfd, all_connections, MAX_CONNECTIONS, timeout_msecs /*timeout*/ );

        for (int i = 0; i < ret_val; i++)
        {
            if (all_connections[i].data.fd == server_fd) { 
                new_fd = accept(server_fd, (struct sockaddr*)&new_addr, &addrlen);
                if (new_fd >= 0) {
                    setup_epoll_connection(epfd, new_fd, &epoll_temp);
                    printf("Accepted a new connection with  fd: %d\n", new_fd);
                    ret_val = recv(all_connections[i].data.fd, buf, DATA_BUFFER, 0);
                    
                    // Authentikasi

                    // Dump atau noDump

                } else {
                    fprintf(stderr, "accept failed [%s]\n", strerror(errno));
                }
            }else if (all_connections[i].events & EPOLLIN) {
                if ( (temp_fd = all_connections[i].data.fd) < 0) continue;

                ret_val = recv(all_connections[i].data.fd, buf, DATA_BUFFER, 0);

                if (ret_val > 0) {
                    printf("Returned fd is %d [index, i: %d]\n", all_connections[i].data.fd, i);
                    printf("Received data (len %d bytes, fd: %d): %s\n", ret_val, all_connections[i].data.fd, buf);
                    ret_val = send(all_connections[i].data.fd, SUCCESS_MESSAGE, sizeof(SUCCESS_MESSAGE), 0);
                    if(strstr(buf,"exit")){
                        close(all_connections[i].data.fd);
                    }
                }
            }
        }
    } /* while(1) */

    /* Last step: Close all the sockets */
    for (i=0;i < MAX_CONNECTIONS;i++) {
        if (all_connections[i].data.fd > 0) {
            close(all_connections[i].data.fd);
        }
    }
    return 0;
}