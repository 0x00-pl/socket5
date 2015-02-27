#include <stdlib.h>

int convert_char(char *a, char *b, int reverse){
    if(!reverse){
        *a = *b;
    }
    else{
        *b = *a;
    }
    return 0;
}

int convert_n_char(char *a, char *b, size_t n, int reverse){
    while(n --> 0){
        convert_char(a+n, b+n, reverse);
    }
    return 0;
}

#define S5_VERSION 0x05
int convert_s5_mothed_c(char *data, char *ver, char *nmethods, char *methods, int reverse){
    convert_char(ver,       data, reverse);
    convert_char(nmethods,  data+1, reverse);
    convert_n_char(methods, data+2, (size_t)*nmethods, reverse);
    return 0;
}

#define S5_METHOD_NO_AUTH  0x00
#define S5_METHOD_GSSAPI   0x01
#define S5_METHOD_PASSWORD 0x02
int convert_s5_mothed_r(char *data, char *ver, char *method, int reverse){
    convert_char(ver, data, reverse);
    convert_char(method, data+1, reverse);
    return 0;
}


int convert_s5_auth_pass_c(char *data, char *ver, char *ulen, char *uname, char *plen, char *passwd, int reverse){
    convert_char(ver,      data, reverse);
    convert_char(ulen,     data+1, reverse);
    convert_n_char(uname,  data+2, (size_t)*ulen, reverse);
    convert_char(plen,     data+2+(size_t)*ulen, reverse);
    convert_n_char(passwd, data+2+(size_t)*ulen+1, (size_t)*plen, reverse);
    return 0;
}

#define S5_METHOD_PASSWORD_SUCCESS  0x00
int convert_s5_auth_pass_r(char *data, char *ver, char *status, int reverse){
    convert_char(ver, data, reverse);
    convert_char(status, data+1, reverse);
    return 0;
}

#define S5_ATYPE_IP4        0x01
#define S5_ATYPE_DOMAINNAME 0x03
#define S5_ATYPE_IP6        0x04
int convert_s5_atyp_addr_port(char *data, char *atyp, char *addr, char *port, size_t *header_size, int reverse){
    convert_char(atyp, data, reverse);
    switch(*atyp){
        case S5_ATYPE_IP4:
            convert_n_char(addr, data+1, 4, reverse);
            convert_n_char(port, data+5, 2, reverse);
            if(header_size != NULL){*header_size = 7;}
            break;
        case S5_ATYPE_DOMAINNAME:
            convert_char(addr, data+1, reverse);
            convert_n_char(addr+1, data+2, (size_t)*addr, reverse);
            convert_n_char(addr+1, data+2+(size_t)*addr, 2, reverse);
            if(header_size != NULL){*header_size = 4+(size_t)*addr;}
            break;
        case S5_ATYPE_IP6:
            convert_n_char(addr, data+1, 16, reverse);
            convert_n_char(port, data+17, 2, reverse);
            if(header_size != NULL){*header_size = 19;}
            break;
        default:
            return -1;
    }
    return 0;
}

#define S5_REQUEST_CMD_CONNECT 0x01
#define S5_REQUEST_CMD_BIND    0x02
#define S5_REQUEST_CMD_UDP     0x03
int convert_s5_request(char *data, char *ver, char *cmd, char *rsv, char *atyp, char *dst_addr, char *port, int reverse){
    convert_char(ver, data, reverse);
    convert_char(cmd, data+1, reverse);
    convert_char(rsv, data+2, reverse);
    if(*rsv!=0x00){return -1;}
    return convert_s5_atyp_addr_port(data+3, atyp, dst_addr, port, NULL, reverse);
}

#define S5_REPLIE_REP_SUCCEED                 0x00
#define S5_REPLIE_REP_S5_SERVER_FAIL          0x01
#define S5_REPLIE_REP_CONNECTION_NOT_ALLOWED  0x02
#define S5_REPLIE_REP_NETWORK_UNREACHABLE     0x03
#define S5_REPLIE_REP_HOST_UNRECHABLE         0x04
#define S5_REPLIE_REP_CONNECTION_REFUSED      0x05
#define S5_REPLIE_REP_TTL_EXPIRED             0x06
#define S5_REPLIE_REP_CMD_NOT_SUPPORTED       0x07
#define S5_REPLIE_REP_ADDR_TYPE_NOT_SUPPORTED 0x08
int convert_s5_replie(char *data, char *ver, char *rep, char *rsv, char *atyp, char *bnd_addr, char *port, int reverse){
    convert_char(ver, data, reverse);
    convert_char(rep, data+1, reverse);
    convert_char(rsv, data+2, reverse);
    if(*rsv!=0x00){return -1;}
    return convert_s5_atyp_addr_port(data+3, atyp, bnd_addr, port, NULL, reverse);
}

int convert_s5_udp_header(char *data, char *rsv, char *frag, char *atyp, char *dst_addr, char *port, size_t *header_size, int reverse){
    convert_n_char(rsv, data, 2, reverse);
    if(rsv[0]!=0x00 || rsv[1]!=0x00){return -1;}
    convert_char(frag, data+2, reverse);
    if(convert_s5_atyp_addr_port(data+3, atyp, dst_addr, port, header_size, reverse) != 0){return -1;}
    if(header_size != NULL){*header_size += 3;}
    return 0;
}





