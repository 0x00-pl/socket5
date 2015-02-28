#ifndef _SOCKET5_TAR_HANDLER_H_
#define _SOCKET5_TAR_HANDLER_H_

#include "net_event.h"


typedef struct s5tar_args_decl{
    ne_read_n_and_call_args_t base;
    int stage;
    char nmotheds;
    char ver;
    char rsv;
    char cmd;
    char atyp;
    char addr[256];
    char port[2];
    int pipe_fd;
} s5tar_args_t;

void s5tar_error(rb_node_net_event_t* node, uint32_t event, ne_manager_t* ne_manager);

void s5tar_entry(rb_node_net_event_t *node, ne_manager_t *ne_manager);

void s5tar_mothed_c(rb_node_net_event_t *node, uint32_t event, ne_manager_t *ne_manager);

void s5tar_request(rb_node_net_event_t *node, uint32_t event, ne_manager_t *ne_manager);



#endif
