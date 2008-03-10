/* -*- mode: c; c-basic-offset: 8; indent-tabs-mode: nil; -*-
 * vim:expandtab:shiftwidth=8:tabstop=8:
 */

#ifndef _MGS_INTERNAL_H
#define _MGS_INTERNAL_H

#ifdef __KERNEL__
# include <linux/fs.h>
#endif
#include <libcfs/kp30.h>
#include <lustre/lustre_idl.h>
#include <lustre_lib.h>
#include <lustre_dlm.h>
#include <lustre_log.h>
#include <lustre_export.h>

/* in ms */
#define MGS_SERVICE_WATCHDOG_TIMEOUT (obd_timeout * 1000)

/* mgs_llog.c */
int class_dentry_readdir(struct obd_device *obd, struct dentry *dir,
                         struct vfsmount *inmnt, 
                         struct list_head *dentry_list);
 
struct mgs_tgt_srpc_conf {
        struct mgs_tgt_srpc_conf  *mtsc_next;
        char                      *mtsc_tgt;
        struct sptlrpc_rule_set    mtsc_rset;
};

#define INDEX_MAP_SIZE  8192     /* covers indicies to FFFF */
#define FSDB_LOG_EMPTY  0x0001  /* missing client log */
#define FSDB_OLDLOG14   0x0002  /* log starts in old (1.4) style */


struct fs_db {
        char              fsdb_name[9];
        struct list_head  fsdb_list;           /* list of databases */
        struct semaphore  fsdb_sem;
        void             *fsdb_ost_index_map;  /* bitmap of used indicies */
        void             *fsdb_mdt_index_map;  /* bitmap of used indicies */
        /* COMPAT_146 these items must be recorded out of the old client log */
        char             *fsdb_clilov;         /* COMPAT_146 client lov name */
        char             *fsdb_clilmv;
        char             *fsdb_mdtlov;         /* COMPAT_146 mds lov name */
        char             *fsdb_mdtlmv;
        char             *fsdb_mdc;            /* COMPAT_146 mdc name */
        /* end COMPAT_146 */
        __u32             fsdb_flags;
        __u32             fsdb_gen;

        /* in-memory copy of the srpc rules, guarded by fsdb_sem */
        struct sptlrpc_rule_set   fsdb_srpc_gen;
        struct mgs_tgt_srpc_conf *fsdb_srpc_tgt;
        unsigned int              fsdb_srpc_fl_udesc:1;
};

int mgs_init_fsdb_list(struct obd_device *obd);
int mgs_cleanup_fsdb_list(struct obd_device *obd);
int mgs_check_index(struct obd_device *obd, struct mgs_target_info *mti);
int mgs_check_failnid(struct obd_device *obd, struct mgs_target_info *mti);
int mgs_write_log_target(struct obd_device *obd, struct mgs_target_info *mti);
int mgs_upgrade_sv_14(struct obd_device *obd, struct mgs_target_info *mti);
int mgs_erase_log(struct obd_device *obd, char *name);
int mgs_erase_logs(struct obd_device *obd, char *fsname);
int mgs_setparam(struct obd_device *obd, struct lustre_cfg *lcfg, char *fsname);

/* mgs_fs.c */
int mgs_fs_setup(struct obd_device *obd, struct vfsmount *mnt);
int mgs_fs_cleanup(struct obd_device *obddev);

#define strsuf(buf, suffix) (strcmp((buf)+strlen(buf)-strlen(suffix), (suffix)))
#ifdef LPROCFS
int lproc_mgs_setup(struct obd_device *dev);
int lproc_mgs_cleanup(struct obd_device *obd);
int lproc_mgs_add_live(struct obd_device *obd, struct fs_db *fsdb);
int lproc_mgs_del_live(struct obd_device *obd, struct fs_db *fsdb);
void lprocfs_mgs_init_vars(struct lprocfs_static_vars *lvars);
#else
static inline int lproc_mgs_setup(struct obd_device *dev) 
{return 0;}
static inline int lproc_mgs_cleanup(struct obd_device *obd)
{return 0;}
static inline int lproc_mgs_add_live(struct obd_device *obd, struct fs_db *fsdb)
{return 0;}
static inline int lproc_mgs_del_live(struct obd_device *obd, struct fs_db *fsdb)
{return 0;}
static void lprocfs_mgs_init_vars(struct lprocfs_static_vars *lvars)
{
        memset(lvars, 0, sizeof(*lvars));
}
#endif

/* mgs/lproc_mgs.c */
enum {
        LPROC_MGS_CONNECT = 0,
        LPROC_MGS_DISCONNECT,
        LPROC_MGS_EXCEPTION,
        LPROC_MGS_TARGET_REG,
        LPROC_MGS_TARGET_DEL,
        LPROC_MGS_LAST
};
void mgs_counter_incr(struct obd_export *exp, int opcode);
void mgs_stats_counter_init(struct lprocfs_stats *stats);

#endif /* _MGS_INTERNAL_H */
