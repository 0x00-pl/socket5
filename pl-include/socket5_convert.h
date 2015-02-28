#ifndef _SOCKET5_CONVERT_H_
#define _SOCKET5_CONVERT_H_

#include <stdlib.h>


#define S5_VERSION 0x05
int convert_s5_mothed_c(char *data, char *ver, char *nmethods, char *methods, int reverse);

#define S5_METHOD_NO_AUTH  0x00
#define S5_METHOD_GSSAPI   0x01
#define S5_METHOD_PASSWORD 0x02
int convert_s5_mothed_r(char *data, char *ver, char *method, int reverse);


int convert_s5_auth_pass_c(char *data, char *ver, char *ulen, char *uname, char *plen, char *passwd, int reverse);

#define S5_METHOD_PASSWORD_SUCCESS  0x00
int convert_s5_auth_pass_r(char *data, char *ver, char *status, int reverse);

#define S5_ATYPE_IP4        0x01
#define S5_ATYPE_DOMAINNAME 0x03
#define S5_ATYPE_IP6        0x04
int convert_s5_atyp_addr_port(char *data, char *atyp, char *addr, char *port, size_t *header_size, int reverse);

#define S5_REQUEST_CMD_CONNECT 0x01
#define S5_REQUEST_CMD_BIND    0x02
#define S5_REQUEST_CMD_UDP     0x03
int convert_s5_request(char *data, char *ver, char *cmd, char *rsv, char *atyp, char *dst_addr, char *port, int reverse);

#define S5_REPLIE_REP_SUCCEED                 0x00
#define S5_REPLIE_REP_S5_SERVER_FAIL          0x01
#define S5_REPLIE_REP_CONNECTION_NOT_ALLOWED  0x02
#define S5_REPLIE_REP_NETWORK_UNREACHABLE     0x03
#define S5_REPLIE_REP_HOST_UNRECHABLE         0x04
#define S5_REPLIE_REP_CONNECTION_REFUSED      0x05
#define S5_REPLIE_REP_TTL_EXPIRED             0x06
#define S5_REPLIE_REP_CMD_NOT_SUPPORTED       0x07
#define S5_REPLIE_REP_ADDR_TYPE_NOT_SUPPORTED 0x08
int convert_s5_replie(char *data, char *ver, char *rep, char *rsv, char *atyp, char *bnd_addr, char *port, int reverse);

int convert_s5_udp_header(char *data, char *rsv, char *frag, char *atyp, char *dst_addr, char *port, size_t *header_size, int reverse);



#endif
