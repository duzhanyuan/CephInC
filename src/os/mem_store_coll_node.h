#ifndef CCEPH_OS_MEM_STORE_COLL_NODE_H
#define CCEPH_OS_MEM_STORE_COLL_NODE_H

#include "common/rbtree.h"
#include "os/types.h"

typedef struct {
    cceph_os_coll_id_t cid;
    cceph_rb_root objects;
    cceph_rb_root map;

    cceph_rb_node node;
} cceph_mem_store_coll_node;

extern int cceph_mem_store_coll_node_new(
        cceph_os_coll_id_t          cid,
        cceph_mem_store_coll_node** node,
        int64_t                     log_id);

//This will free all objects in the collection
extern int cceph_mem_store_coll_node_free(
        cceph_mem_store_coll_node** node,
        int64_t                     log_id);

CCEPH_DEFINE_MAP(mem_store_coll_node, cceph_os_coll_id_t, cid);

#endif
