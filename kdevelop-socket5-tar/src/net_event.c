#include "net_event.h"
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>
#include "rb_tree.h"

// buffer
static ne_buffer_t *ne_buffer_from_buff(char *buff, size_t size){
    ne_buffer_t *ret = (ne_buffer_t*)malloc(sizeof(ne_buffer_t));
    ret->buff = buff;
    ret->size = size;
    ret->pos = 0;
    ret->next = NULL;
    return ret;
}

static void ne_buffer_free(ne_buffer_t *buffer){
    if(buffer != NULL){
        ne_buffer_free(buffer->next);
        free(buffer->buff);
        free(buffer);
    }
}

// node
static rb_node_net_event_t *rb_node_net_event_from_x(int fd, uint32_t epoll_flag, event_hander_f *handler, void *extra){
    rb_node_net_event_t *ret = (rb_node_net_event_t*)malloc(sizeof(rb_node_net_event_t));
    ret->fd = fd;
    ret->epoll_flag = epoll_flag;
    ret->hander = handler;
    ret->extra = extra;
    ret->write_buffer = NULL;
    ret->write_buffer_back = NULL;
    return ret;
}

static void rb_node_net_event_delete(rb_node_net_event_t *rb_node_net_event){
    if(rb_node_net_event->extra != NULL){
        printf("extra info free by default: free(%p)\n", rb_node_net_event->extra);
        free(rb_node_net_event->extra);
    }
    ne_buffer_free(rb_node_net_event->write_buffer);
    free(rb_node_net_event);
}

static void rb_node_net_event_write_push_back(rb_node_net_event_t *node, char *buff, size_t size){
    ne_buffer_t *item = ne_buffer_from_buff(buff, size);
    if(node->write_buffer == NULL){
        node->write_buffer = item;
        node->write_buffer_back = item;
    }
    else{
        node->write_buffer_back->next = item;
    }
}

static void rb_node_net_event_write_buffer_pop_frount(rb_node_net_event_t *node){
    ne_buffer_t *item = node->write_buffer;
    node->write_buffer = node->write_buffer->next;
    if(node->write_buffer == NULL){
        node->write_buffer_back = NULL;
    }
    ne_buffer_free(item);
}

// rb_tree
static void rbt_insert_event(rb_node_net_event_t **proot, int fd, uint32_t epoll_flag, event_hander_f *handler, void *extra){
    rb_node_net_event_t *parent = NULL;
    rb_node_net_event_t *cur_root = *proot;
    rb_node_net_event_t *n = rb_node_net_event_from_x(fd, epoll_flag, handler, extra);
    
    while(cur_root != NULL){
        if(n->fd == cur_root->fd){
            printf("[waring] fd insert %d but we has (%d %p %p)\n", n->fd, cur_root->fd, cur_root->hander, cur_root->extra);
            return;
        }
        parent = cur_root;
        cur_root = (rb_node_net_event_t*)((n->fd < cur_root->fd)? cur_root->base.left: cur_root->base.right);
    }
    n->base.parent = (rb_node*)parent;
    if(parent != NULL){
        *((n->fd < parent->fd)? &parent->base.left: &parent->base.right) = (rb_node*)n;
    }
    rbt_after_insert((rb_node**)proot, (rb_node*)n);
}

static rb_node_net_event_t *rbt_find_event(rb_node_net_event_t *root, int fd){
    while(root != NULL){
        if(fd == root->fd){
            return root;
        }
        root = (rb_node_net_event_t*)((fd < root->fd)? root->base.left: root->base.right);
    }
    return NULL;
}

static void rbt_print_event(rb_node_net_event_t *root){
    rb_node_net_event_t *iter;
    size_t count = 0;
    printf("{");
    for(iter=(rb_node_net_event_t*)rbt_min((rb_node*)root); iter!=NULL; iter=(rb_node_net_event_t*)rbt_next((rb_node*)iter)){
        if(count%4==0){
            printf("\n");
        }
        printf("(%d %p %p), ", iter->fd, iter->hander, iter->extra);
    }
    printf("}\n");
}


//
void set_non_blocking(int fd){
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags|O_NONBLOCK);
}



ne_manager_t *ne_manager_new(){
    ne_manager_t *ret = (ne_manager_t*)malloc(sizeof(ne_manager_t));
    ret->root = NULL;
    ret->epoll_fd = epoll_create1(0);
    return ret;
}

void ne_manager_delete(ne_manager_t *ne_manager){
    rb_node_net_event_t *iter;
    
    while(ne_manager->root != NULL){
        iter = (rb_node_net_event_t*)rbt_pop((rb_node**)&ne_manager->root, rbt_min((rb_node*)ne_manager->root));
        (*iter->hander)(iter, EPOLLERR, ne_manager);
        close(iter->fd);
        rb_node_net_event_delete(iter);
    }
    
    close(ne_manager->epoll_fd);
}

void ne_manager_node_write(ne_manager_t *ne_manager, rb_node_net_event_t *node, char *buff, size_t size){
    rb_node_net_event_write_push_back(node, buff, size);
    
    if((node->epoll_flag & EPOLLOUT) == 0){
        node->epoll_flag |= EPOLLOUT;
        ne_manager_node_modify(ne_manager, node);
    }
}

int ne_manager_node_add(ne_manager_t *ne_manager, uint32_t events_flag, int fd, event_hander_f *handler, void *extra){
    struct epoll_event epe;
    epe.data.fd = fd;
    epe.events = events_flag;
    set_non_blocking(fd);
    epoll_ctl(ne_manager->epoll_fd, EPOLL_CTL_ADD, fd, &epe);

    rbt_insert_event(&ne_manager->root, fd, events_flag, handler, extra);
    return 0;
}

int ne_manager_node_remove(ne_manager_t *ne_manager, rb_node_net_event_t *node){
    epoll_ctl(ne_manager->epoll_fd, EPOLL_CTL_DEL, node->fd, NULL);

    rbt_pop((rb_node**)&ne_manager->root, (rb_node*)node);
    (*node->hander)(node, EPOLLERR, ne_manager);
    close(node->fd);
    rb_node_net_event_delete(node);
    return 0;
}

int ne_manager_fd_remove(ne_manager_t *ne_manager, int fd){
    rb_node_net_event_t *node;
    node = rbt_find_event(ne_manager->root, fd);
    return ne_manager_node_remove(ne_manager, node);
}

int ne_manager_node_modify(ne_manager_t *ne_manager, rb_node_net_event_t *node){
    struct epoll_event epe;
    epe.data.fd = node->fd;
    epe.events = node->epoll_flag;
    epoll_ctl(ne_manager->epoll_fd, EPOLL_CTL_MOD, node->fd, &epe);
    return 0;
}

static void handler__(rb_node_net_event_t *node, uint32_t events, struct ne_manager_decl *ne_manager){
    ssize_t writed;
    ne_buffer_t *write_buffer;
    if(events & EPOLLOUT){
        if(node->write_buffer != NULL){
            write_buffer = node->write_buffer;
            writed = write(node->fd,
                        write_buffer->buff + write_buffer->pos,
                        write_buffer->size - write_buffer->pos);
            if(writed <= 0){
                printf("[error]: write error\n");
                // get error, ignore current buff to write
                write_buffer->pos = write_buffer->size;
            }
            write_buffer->pos += (size_t)writed;
                
            if(write_buffer->pos >= write_buffer->size){
                rb_node_net_event_write_buffer_pop_frount(node);

            }
        }
        if(node->write_buffer == NULL){
            // all write done
            node->epoll_flag = node->epoll_flag & (uint32_t)(~EPOLLOUT);
            ne_manager_node_modify(ne_manager, node);
        }
    }
    else{
        (*node->hander)(node, events & (uint32_t)(~EPOLLOUT), ne_manager);
    }
}

int ne_manager_poll(ne_manager_t *ne_manager, int timeout){
    struct epoll_event epe[512];
    rb_node_net_event_t *node;
    int length;
    int cur_fd;
    int i;
    length = epoll_wait(ne_manager->epoll_fd, epe, 512, timeout);
    for(i=0; i<length; i++){
        cur_fd = epe[i].data.fd;
        node = rbt_find_event(ne_manager->root, cur_fd);
        handler__(node, epe[i].events, ne_manager);
//         (*node->hander)(node, epe[i].events);
    }
    return 0;
}

