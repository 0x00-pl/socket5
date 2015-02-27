#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "net_event.h"

int is_exit = 0;

void test_cat_tcp(){
    struct sockaddr_in local;
    struct sockaddr ru;
    bzero(&local, sizeof(struct sockaddr_in));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(8889);
    int localfd = socket(AF_INET, SOCK_STREAM, 0);
    if(bind(localfd, (struct sockaddr*)&local, sizeof(struct sockaddr_in)) == -1){
        printf("bind local fail\n");
        exit(-1);
    }
//     set_non_blocking(localfd);
    listen(localfd, SOMAXCONN);
    socklen_t len = sizeof(struct sockaddr_in);
    int ru_fd = accept(localfd, &ru, &len);
    
    
//     printf("accept ok!\r\nServer start get connect from %#x : %#x\r\n",ntohl(ru.sin_addr.s_addr),ntohs(ru.sin_port));
    
    char buff[17];
    ssize_t recv_len;
    while(1){
        recv_len = read(ru_fd, buff, 16);
        if(recv_len<=0){
            break;
        }
        buff[recv_len]='\0';
        printf("%s", buff);
    }
}

typedef struct global_value_decl{
    ne_manager_t *ne_manager;
    int is_exit;
} global_value_t;

global_value_t *global_get(){
    static global_value_t *g = NULL;
    if(g == NULL){
        g = (global_value_t*)malloc(sizeof(global_value_t));
        g->ne_manager = ne_manager_new();
        g->is_exit = 0;
    }
    return g;
}

int create_tcp_entry(uint16_t port){
    struct sockaddr_in local;
    bzero(&local, sizeof(struct sockaddr_in));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(port);
    int localfd = socket(AF_INET, SOCK_STREAM, 0);
    if(bind(localfd, (struct sockaddr*)&local, sizeof(struct sockaddr_in)) == -1){
        printf("bind local fail\n");
        exit(-1);
    }
    listen(localfd, SOMAXCONN);
    
    return localfd;
}

// void handle_echo(int fd, uint32_t events, void *extra){
//     (void)events;(void)extra;
//     char buff[16];
//     ssize_t length;
//     
//     length = read(fd, buff, 16);
//     if(length <= 0){
//         ne_epoll_unreg(global_proot_get(), global_epoll_fd_get(), fd);
//         close(fd);
//     }
//     buff[length]='\0';
//     printf("%s", buff);
//     write(fd, buff, (size_t)length);
// }


void handle_read(rb_node_net_event_t *node, uint32_t event, struct ne_manager_decl *ne_manager){
    ssize_t length;
    char *buff = (char*)malloc(17);
    length = read(node->fd, buff, 16);
    if(length <= 0 || (event & EPOLLERR)){
        printf("[waring] link break.\n");
        ne_manager_fd_remove(ne_manager, node->fd);
        close(node->fd);
        return;
    }
    
    ne_manager_node_write(ne_manager, node, buff, (size_t)length);
    printf("revc[%d]: %s", (int)length, buff);
    if(strcmp(buff,"exit")==0){
        is_exit = 1;
    }
}
// void handle_write(int fd, uint32_t events, void *extra){
//     (void)events;(void)extra;
//     ne_buffer_t *buffer = (ne_buffer_t*)extra;
//     ssize_t length;
//     length = write(fd, buffer->buff+buffer->pos, buffer->size-buffer->pos);
//     if(length <= 0){
//         ne_epoll_unreg(global_proot_get(), global_epoll_fd_get(), fd);
//         close(fd);
//         return;
//     }
//     buffer->pos += (size_t)length;
//     if(buffer->pos >= buffer->size){
//         free(buffer->buff);
//         free(buffer);
//         ne_epoll_modify(global_proot_get(), global_epoll_fd_get(), EPOLLIN, fd, &handle_read, NULL);
//     }
//     else{
//         // state no change
//     }
// }

void handle_listen(rb_node_net_event_t *node, uint32_t event, struct ne_manager_decl *ne_manager){
    struct sockaddr ru;
    socklen_t len = sizeof(struct sockaddr_in);
    int ru_fd = accept(node->fd, &ru, &len);
    if(ru_fd < 0 || (event & EPOLLERR)){
        printf("not accept\n");
        return;
    }
    ne_manager_node_add(ne_manager, EPOLLIN, ru_fd, &handle_read, 0);
//     ne_epoll_reg(global_proot_get(), global_epoll_fd_get(), EPOLLIN, ru_fd, &handle_read, 0);
}

void test_epoll_echo(){
    ne_manager_t *ne_manager = global_get()->ne_manager;
    int entry_fd = create_tcp_entry(8888);
    ne_manager_node_add(ne_manager, EPOLLIN, entry_fd, &handle_listen, 0);
    
    while(!global_get()->is_exit){
        ne_manager_poll(ne_manager, 1);
//         ne_epoll_poll(global_proot_get(), global_epoll_fd_get(), 1);
    }
    
    ne_manager_delete(ne_manager);
//     ne_rbt_fd_close(*global_proot_get());
//     ne_epoll_delete(global_epoll_fd_get());
}

int main(int argc, char **argv){
    (void)argc;(void)argv;
    
    test_epoll_echo();
    
    return 0;
}