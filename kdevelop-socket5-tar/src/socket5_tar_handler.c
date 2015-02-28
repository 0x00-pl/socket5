#include "socket5_tar_handler.h"
#include <socket5_convert.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdlib.h>

enum{
    stage_entry,
    stage_mothed_c,
    stage_mothed_c_1,
    stage_mothed_c_2,
    stage_request,
    stage_request_1,
    stage_request_2
};

void s5tar_error(rb_node_net_event_t* node, uint32_t event, ne_manager_t* ne_manager){
    (void)event;
    s5tar_args_t *args;
    printf("[%d]\t[waring] link break.\n", node->fd);
    
    args = (s5tar_args_t*)node->extra;
    free(args->base.buff);
    free(args);
    node->extra = NULL;
    ne_manager_fd_remove(ne_manager, node->fd);
}


void s5tar_mothed_c(rb_node_net_event_t *node, uint32_t event, ne_manager_t *ne_manager);
void s5tar_mothed_c_1(rb_node_net_event_t *node, uint32_t event, ne_manager_t *ne_manager);
void s5tar_auth_pass_c(rb_node_net_event_t *node, uint32_t event, ne_manager_t *ne_manager);
void s5tar_request(rb_node_net_event_t *node, uint32_t event, ne_manager_t *ne_manager);
void s5tar_udp_header(rb_node_net_event_t *node, uint32_t event, ne_manager_t *ne_manager);


void s5tar_entry(rb_node_net_event_t *node, ne_manager_t *ne_manager){
    s5tar_args_t *args;
    args = (s5tar_args_t*)malloc(sizeof(s5tar_args_t));
    args->base.buff = malloc(1024);
    args->base.readed = 0;
    args->stage = stage_mothed_c;
    node->extra = args;
    s5tar_mothed_c(node, 0, ne_manager);
}


void s5tar_mothed_c(rb_node_net_event_t *node, uint32_t event, ne_manager_t *ne_manager){
    size_t i;
    char *buff;
    s5tar_args_t *args;
    if(event & EPOLLERR){
        s5tar_error(node, event, ne_manager);
        return;
    }
    args = (s5tar_args_t*)node->extra;
    switch(args->stage){
        case stage_mothed_c:
            args->base.next_handler = &s5tar_mothed_c;
            // change stage
            args->stage = stage_mothed_c_1;
            args->base.need_to_read = 2;
            node->hander = &ne_event_handler_read_n_and_call;
            ne_manager_node_modify(ne_manager, node);
            break;
        case stage_mothed_c_1:
            args->nmotheds = args->base.buff[1];
            // change stage
            args->stage = stage_mothed_c_2;
            args->base.need_to_read += (size_t)args->nmotheds;
            node->hander = &ne_event_handler_read_n_and_call;
            ne_manager_node_modify(ne_manager, node);
            break;
        case stage_mothed_c_2:
            // support mothed check
            for(i=0; i<(size_t)args->nmotheds; i++){
                // TODO: look for support motheds
            }
            buff = (char*)malloc(2);
            buff[0] = 0x05;
            // TODO only support NO_AUTH now
            buff[1] = 0x00;
            ne_manager_node_write(ne_manager, node, buff, 2);
            
            // change func_stage
            args->stage = stage_request;
            s5tar_request(node, 0, ne_manager);
            break;
        default:
            s5tar_error(node, event, ne_manager);
    }
}


void s5tar_request(rb_node_net_event_t *node, uint32_t event, ne_manager_t *ne_manager){
    size_t i;
    char *buff;
    s5tar_args_t *args;
    if(event & EPOLLERR){
        s5tar_error(node, event, ne_manager);
        return;
    }
    args = (s5tar_args_t*)node->extra;
    switch(args->stage){
        case stage_request:
            args->base.next_handler = &s5tar_request;
            args->base.readed = 0;
            // change stage
            args->stage = stage_request_1;
            args->base.need_to_read = 5;
            node->hander = &ne_event_handler_read_n_and_call;
            ne_manager_node_modify(ne_manager, node);
            break;
        case stage_request_1:
            args->atyp = args->base.buff[3];
            switch(args->atyp){
                case S5_ATYPE_IP4:
                    args->base.need_to_read = 10;
                    break;
                case S5_ATYPE_DOMAINNAME:
                    args->base.need_to_read = 6+(size_t)args->base.buff[4];
                    break;
                case S5_ATYPE_IP6:
                    args->base.need_to_read = 22;
                    break;    
                default:
                s5tar_error(node, event, ne_manager);
            }
            
            // change stage
            args->stage = stage_request_2;
            node->hander = &ne_event_handler_read_n_and_call;
            ne_manager_node_modify(ne_manager, node);
            break;
        case stage_request_2:
            convert_s5_request(args->base.buff, &args->ver, &args->cmd, &args->rsv, &args->atyp, args->addr, args->port, 0);
            printf("request(%x, %x, %x, %x, %d.%d.%d.%d, %d)\n",
                   (int)args->ver, (int)args->cmd, (int)args->rsv, (int)args->atyp,
                   (uint8_t)args->addr[0], (uint8_t)args->addr[1], (uint8_t)args->addr[2], (uint8_t)args->addr[3],
                   (int)(((uint8_t)args->port[0]<<8)|((uint8_t)args->port[1])));
            // TODO send to network
            // reply to user
            buff = (char*)malloc(10);
            // replay no detal
            for(i=0; i<10; i++){
                buff[i] = 0x00;
            }
            buff[0] = 05; // socket version 5
            buff[3] = 01; // atyp: ip4
            ne_manager_node_write(ne_manager, node, buff, 10);
            // TODO reg cur_fd to pipe system
            // change func_stage
            s5tar_error(node, event, ne_manager);
            break;
        default:
            s5tar_error(node, event, ne_manager);
    }
}

