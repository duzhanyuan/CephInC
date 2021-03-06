#ifndef CCEPH_OSD_TYPES_H
#define CCEPH_OSD_TYPES_H

#include <pthread.h>

#include "common/types.h"
#include "common/osdmap.h"

#include "message/messenger.h"
#include "message/server_messenger.h"
#include "message/msg_header.h"

#include "os/object_store.h"

#define CCEPH_OSD_META_COLL_ID               0
#define CCEPH_OSD_META_ATTR_OSD_ID           "osd_id"
#define CCEPH_OSD_META_ATTR_OSDMAP_MAX_EPOCH "osdmap_max_epoch"

#define CCEPH_PG_STATE_UNKNOWN    0
#define CCEPH_PG_STATE_INITIAL    1
#define CCEPH_PG_STATE_STARTING   2
#define CCEPH_PG_STATE_ACTIVE     3
#define CCEPH_PG_STATE_DISUSE     4
#define CCEPH_PG_STATE_INCOMPLETE 5

#endif
