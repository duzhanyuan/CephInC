#ifndef CCEPH_ERRNO_H
#define CCEPH_ERRNO_H

#define CCEPH_OK                    0
#define CCEPH_ERR_UNKNOWN           (-1000)
#define CCEPH_ERR_CONN_NOT_FOUND    (-1001)
#define CCEPH_ERR_CONN_CLOSED       (-1002)
#define CCEPH_ERR_WRITE_CONN_ERR    (-1003)
#define CCEPH_ERR_UNKNOWN_OP        (-1004)
#define CCEPH_ERR_NOT_ENOUGH_SERVER (-1005)
#define CCEPH_ERR_WRONG_CLIENT_ID   (-1006)

extern const char* errno_str(int err_no);

#endif