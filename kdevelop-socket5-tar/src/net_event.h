#ifndef _NET_EVENT_H_
#define _NET_EVENT_H_
#include <stdint.h>
#include "rb_tree.h"

struct rb_node_net_event_decl;
struct ne_manager_decl;
typedef void event_hander_f(struct rb_node_net_event_decl *node, uint32_t event, struct ne_manager_decl *ne_manager);

typedef struct ne_buffer_decl{
    size_t size;
    size_t pos;
    char *buff;
    struct ne_buffer_decl *next;
} ne_buffer_t;

typedef struct rb_node_net_event_decl{
    rb_node base;
    int fd;
    uint32_t epoll_flag;
    event_hander_f *hander;
    void *extra;
    ne_buffer_t *write_buffer;
    ne_buffer_t *write_buffer_back;
    
} rb_node_net_event_t;

typedef struct ne_manager_decl{
    rb_node_net_event_t *root;
    int epoll_fd;
} ne_manager_t;


void set_non_blocking(int fd);


ne_manager_t *ne_manager_new();
void ne_manager_delete(ne_manager_t *ne_manager);
void ne_manager_node_write(ne_manager_t *ne_manager, rb_node_net_event_t *node, char *buff, size_t size);
int ne_manager_node_add(ne_manager_t *ne_manager, uint32_t events_flag, int fd, event_hander_f *handler, void *extra);
int ne_manager_node_remove(ne_manager_t *ne_manager, rb_node_net_event_t *node);
int ne_manager_fd_remove(ne_manager_t *ne_manager, int fd);
int ne_manager_node_modify(ne_manager_t *ne_manager, rb_node_net_event_t *node);
int ne_manager_poll(ne_manager_t *ne_manager, int timeout);


// void event_hander_read_n_and_call(struct rb_node_net_event_decl *node, uint32_t event, struct ne_manager_decl *ne_manager);


#endif