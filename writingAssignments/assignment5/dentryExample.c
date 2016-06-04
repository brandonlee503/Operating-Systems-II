struct dentry {
    atomic_t                    d_count;                        /* usage count */
    unsigned int                d_flags;                        /* dentry flags */
    spinlock_t                  d_lock;                         /* per-dentry lock */
    int                         d_mounted;                      /* is this a mount point? */
    struct inode                *d_inode;                       /* associated inode */
    struct hlist_node           d_hash;                         /* list of hash table entries */
    struct dentry               *d_parent;                      /* dentry object of parent */
    struct qstr                 d_name;                         /* dentry name */
    struct list_head            d_lru;                          /* unused list */
    union {
        struct list_head        d_child;                        /* list of dentries within */
        struct rcu_head         d_rcu;                          /* RCU locking */
    } d_u;
    struct list_head            d_subdirs;                      /* subdirectories */
    struct list_head            d_alias;                        /* list of alias inodes */
    unsigned long               d_time;                         /* revalidate time */
    struct dentry_operations    *d_op;                          /* dentry operations table */
    struct super_block          *d_sb;                          /* superblock of file */
    void                        *d_fsdata;                      /* filesystem-specific data */
    unsigned char               d_iname[DNAME_INLINE_LEN_MIN];  /* short name */
};
