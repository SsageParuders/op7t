/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_PID_NS_H
#define _LINUX_PID_NS_H

#include <linux/sched.h>
#include <linux/bug.h>
#include <linux/mm.h>
#include <linux/workqueue.h>
#include <linux/threads.h>
#include <linux/nsproxy.h>
#include <linux/kref.h>
#include <linux/ns_common.h>

struct pidmap {
       atomic_t nr_free;
       void *page;
};

#define BITS_PER_PAGE		(PAGE_SIZE * 8)
#define BITS_PER_PAGE_MASK	(BITS_PER_PAGE-1)
#define PIDMAP_ENTRIES		((PID_MAX_LIMIT+BITS_PER_PAGE-1)/BITS_PER_PAGE)

struct fs_pin;

enum { /* definitions for pid_namespace's hide_pid field */
	HIDEPID_OFF	  = 0,
	HIDEPID_NO_ACCESS = 1,
	HIDEPID_INVISIBLE = 2,
};

struct pid_namespace {
	struct kref kref; /* kref 表示指向 pid_namespace 的个数 */
	struct pidmap pidmap[PIDMAP_ENTRIES]; /* pidmap 结构体表示分配pid的位图，pidmap[PIDMAP_ENTRIES] 域存储了该 pid_namespace 下 pid 已分配情况 */
	struct rcu_head rcu;
	int last_pid; /* 最后一个已分配的 pid */
	unsigned int nr_hashed; /* nr_hashed 统计该命名空间已分配PID个数 */
	struct task_struct *child_reaper; /* child_reaper指向的是一个进程。 该进程的作用是当子进程结束时为其收尸（回收空间）。global namespace 中child_reaper 指向 init_task。*/
	struct kmem_cache *pid_cachep; /* 指向分配 pid 的 slab 的地址 */
	unsigned int level; /* 该命名空间所处层级 */
	struct pid_namespace *parent; /* 指向该命名空间的父命名空间 */
#ifdef CONFIG_PROC_FS
	struct vfsmount *proc_mnt;
	struct dentry *proc_self;
	struct dentry *proc_thread_self;
#endif
#ifdef CONFIG_BSD_PROCESS_ACCT
	struct fs_pin *bacct;
#endif
	struct user_namespace *user_ns;
	struct ucounts *ucounts;
	struct work_struct proc_work;
	kgid_t pid_gid;
	int hide_pid;
	int reboot;	/* group exit code if this pidns was rebooted */
	struct ns_common ns;
} __randomize_layout;

extern struct pid_namespace init_pid_ns;

#define PIDNS_HASH_ADDING (1U << 31)

#ifdef CONFIG_PID_NS
static inline struct pid_namespace *get_pid_ns(struct pid_namespace *ns)
{
	if (ns != &init_pid_ns)
		kref_get(&ns->kref);
	return ns;
}

extern struct pid_namespace *copy_pid_ns(unsigned long flags,
	struct user_namespace *user_ns, struct pid_namespace *ns);
extern void zap_pid_ns_processes(struct pid_namespace *pid_ns);
extern int reboot_pid_ns(struct pid_namespace *pid_ns, int cmd);
extern void put_pid_ns(struct pid_namespace *ns);

#else /* !CONFIG_PID_NS */
#include <linux/err.h>

static inline struct pid_namespace *get_pid_ns(struct pid_namespace *ns)
{
	return ns;
}

static inline struct pid_namespace *copy_pid_ns(unsigned long flags,
	struct user_namespace *user_ns, struct pid_namespace *ns)
{
	if (flags & CLONE_NEWPID)
		ns = ERR_PTR(-EINVAL);
	return ns;
}

static inline void put_pid_ns(struct pid_namespace *ns)
{
}

static inline void zap_pid_ns_processes(struct pid_namespace *ns)
{
	BUG();
}

static inline int reboot_pid_ns(struct pid_namespace *pid_ns, int cmd)
{
	return 0;
}
#endif /* CONFIG_PID_NS */

extern struct pid_namespace *task_active_pid_ns(struct task_struct *tsk);
void pidhash_init(void);
void pidmap_init(void);

#endif /* _LINUX_PID_NS_H */
