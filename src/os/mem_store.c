#include "mem_store.h"

#include <string.h>

#include "common/assert.h"
#include "common/errno.h"
#include "common/log.h"
#include "common/rbtree.h"
#include "common/types.h"

#include "os/mem_store_coll_node.h"
#include "os/mem_store_object_node.h"

cceph_os_funcs* cceph_mem_store_get_funcs() {
    cceph_os_funcs *os_funcs = (cceph_os_funcs*)malloc(sizeof(cceph_os_funcs));
    bzero(os_funcs, sizeof(cceph_os_funcs));

    os_funcs->mount                 = cceph_mem_store_mount;
    os_funcs->submit_transaction    = cceph_mem_store_submit_transaction;
    os_funcs->read                  = cceph_mem_store_read_object;

    return os_funcs;
}

int cceph_mem_store_new(
        cceph_mem_store** store,
        int64_t           log_id) {

    assert(log_id, store  != NULL);
    assert(log_id, *store == NULL);

    *store = (cceph_mem_store*)malloc(sizeof(cceph_mem_store));
    if (*store == NULL) {
        return CCEPH_ERR_NO_ENOUGH_MEM;
    }

    (*store)->colls = CCEPH_RB_ROOT;
    pthread_mutex_init(&((*store)->lock), NULL);
    return CCEPH_OK;
}

int cceph_mem_store_mount(
        cceph_object_store* os,
        int64_t             log_id) {
    //MemStore don't need mount
    assert(log_id, os != NULL);
    return CCEPH_OK;
}

int cceph_mem_store_do_op_write(
        cceph_mem_store*         os,
        cceph_os_transaction_op* op,
        int64_t                  log_id) {

    assert(log_id, os != NULL);
    assert(log_id, op != NULL);
    assert(log_id, op->oid != NULL);
    assert(log_id, op->offset >= 0);
    assert(log_id, op->length >= 0);
    if (op->length > 0) {
        //Touch may not have a data
        assert(log_id, op->data != NULL);
    }

    LOG(LL_INFO, log_id, "Execute write op, cid %d, oid %s, offset %ld, length %ld, log_id %ld.",
            op->cid, op->oid, op->offset, op->length, op->log_id);

    log_id = op->log_id; //We will use op's log_id from here

    cceph_mem_store_coll_node *cnode = cceph_mem_store_coll_node_search(
            &os->colls, op->cid);
    if (cnode == NULL) {
        LOG(LL_ERROR, log_id, "Execute write op failed, since cid %d not existed.", op->cid);
        return CCEPH_ERR_COLL_NOT_EXIST;
    }

    cceph_mem_store_object_node *onode = cceph_mem_store_object_node_search(
            &cnode->objects, op->oid);
    if (onode == NULL) {
        int ret = cceph_mem_store_object_node_new(&onode, op->oid, log_id);
        if (ret != CCEPH_OK) {
            return ret;
        }
        ret = cceph_mem_store_object_node_insert(&cnode->objects, onode);
        if (ret != CCEPH_OK) {
            cceph_mem_store_object_node_free(&onode, log_id);
            return ret;
        }
    }

    //Is a touch operation?
    if (op->length == 0) {
        return CCEPH_OK;
    }

    assert(log_id, strcmp(op->oid, onode->oid) == 0);
    if (onode->length < op->offset + op->length) {
        //We don't have enough space, we should expend the data
        int64_t new_length = op->offset + op->length;
        char*   new_data   = (char*)malloc(sizeof(char) * new_length);
        if (new_data == NULL) {
            return CCEPH_ERR_NO_ENOUGH_MEM;
        }

        //cp old data to new data;
        bzero(new_data, new_length);
        memcpy(new_data, onode->data, onode->length);

        char* old_data = onode->data;
        onode->data    = new_data;
        onode->length  = new_length;

        free(old_data);
    }

    //Write op
    memcpy(onode->data + op->offset, op->data, op->length);

    return CCEPH_OK;
}

int cceph_mem_store_do_op(
        cceph_mem_store*         os,
        cceph_os_transaction_op* op,
        int64_t                  log_id) {

    assert(log_id, os != NULL);
    assert(log_id, op != NULL);

    int ret = CCEPH_OK;
    switch(op->op) {
        case CCEPH_OS_OP_NOOP:
            ret = CCEPH_OK;
            break;
        case CCEPH_OS_OP_TOUCH:
        case CCEPH_OS_OP_WRITE:
            ret = cceph_mem_store_do_op_write(os, op, log_id);
            break;
        default:
            ret = CCEPH_ERR_UNKNOWN_OS_OP;
    }

    if (ret == CCEPH_OK) {
        LOG(LL_INFO, log_id, "do_op %s(%d) success.",
                op->op, cceph_os_op_to_str(op->op));
    } else {
        LOG(LL_ERROR, log_id, "do_op %s(%d) failed, errno %d(%s).",
                op->op, cceph_os_op_to_str(op->op),
                ret, cceph_errno_str(ret));
    }

    return ret;
}

int cceph_mem_store_submit_transaction(
        cceph_object_store*   os,
        cceph_os_transaction* tran,
        int64_t               log_id) {

    assert(log_id, os != NULL);
    assert(log_id, tran != NULL);

    cceph_mem_store* mem_store = (cceph_mem_store*)os;
    int op_count = cceph_os_tran_get_op_count(tran, log_id);

    LOG(LL_INFO, log_id, "Submit transaction with %d ops.", op_count);
    pthread_mutex_lock(&mem_store->lock);

    int ret = CCEPH_OK;
    int i = 0;
    for (i = 0; i < op_count; i++) {
        cceph_os_transaction_op* op = cceph_os_tran_get_op(tran, i, log_id);
        ret = cceph_mem_store_do_op(mem_store, op, log_id);
        if (ret != CCEPH_OK) {
            break;
        }
    }

    pthread_mutex_unlock(&mem_store->lock);
    LOG(LL_INFO, log_id, "Transaction executed with %d/%d ops done.", i + 1, op_count);
    return ret;
}

int cceph_mem_store_read_object(
        cceph_object_store* os,
        const char*         oid,
        int64_t             offset,
        int64_t             length,
        char*               data,
        int64_t             log_id) {
    return CCEPH_OK;
}
