#include <stdint.h>

#ifndef __SYSCALL
#define __SYSCALL(x, y)
#endif

// TODO: remove syscalls I don't think I'll need
//
// FIXME: how to get from syscall NR to syscall function pointer?

#define __NR_io_setup               0
#define __NR_io_destroy             1
#define __NR_io_submit              2
#define __NR_io_cancel              3
#define __NR_io_getevents           4
/* fs/xattr.c */
#define __NR_setxattr               5
#define __NR_lsetxattr              6
#define __NR_fsetxattr              7
#define __NR_getxattr               8
#define __NR_lgetxattr              9
#define __NR_fgetxattr              10
#define __NR_listxattr              11
#define __NR_llistxattr             12
#define __NR_flistxattr             13
#define __NR_removexattr            14
#define __NR_lremovexattr           15
#define __NR_fremovexattr           16
/* fs/dcache.c */
#define __NR_getcwd                 17
/* fs/cookies.c */
#define __NR_lookup_dcookie         18
/* fs/eventfd.c */
#define __NR_eventfd2               19
/* fs/eventpoll.c */
#define __NR_epoll_create1          20
#define __NR_epoll_ctl              21
#define __NR_epoll_pwait            22
/* fs/fcntl.c */
#define __NR_dup                    23
#define __NR_dup3                   24
#define __NR_fcntl                  25
/* fs/inotify_user.c */
#define __NR_inotify_init1          26
#define __NR_inotify_add_watch      27
#define __NR_inotify_rm_watch       28
/* fs/ioctl.c */
#define __NR_ioctl                  29
/* fs/ioprio.c */
#define __NR_ioprio_set             30
#define __NR_ioprio_get             31
/* fs/locks.c */
#define __NR_flock                  32
/* fs/namei.c */
#define __NR_mknodat                33
#define __NR_mkdirat                34
#define __NR_unlinkat               35
#define __NR_symlinkat              36
#define __NR_linkat                 37
#define __NR_renameat               38
/* fs/namespace.c */
#define __NR_umount2                39
#define __NR_mount                  40
#define __NR_pivot_root             41
/* fs/nfsctl.c */
#define __NR_nfsservctl             42
/* fs/open.c */
#define __NR_statfs                 43
#define __NR_fstatfs                44
#define __NR_truncate               45
#define __NR_ftruncate              46
#define __NR_fallocate              47
#define __NR_faccessat              48
#define __NR_chdir                  49
#define __NR_fchdir                 50
#define __NR_chroot                 51
#define __NR_fchmod                 52
#define __NR_fchmodat               53
#define __NR_fchownat               54
#define __NR_fchown                 55
#define __NR_openat                 56
#define __NR_close                  57
#define __NR_vhangup                58
/* fs/pipe.c */
#define __NR_pipe                   59
/* fs/quota.c */
#define __NR_quotactl               60
/* fs/readdir.c */
#define __NR_getdents64             61
/* fs/read_write.c */
#define __NR_lseek                  62
#define __NR_read                   63
#define __NR_write                  64
#define __NR_readv                  65
#define __NR_writev                 66
#define __NR_pread64                67
#define __NR_pwrite64               68
#define __NR_preadv                 69
#define __NR_pwritev                70
/* fs/sendfile.c */
#define __NR_sendfile               71
/* fs/select.c */
#define __NR_pselect6               72
#define __NR_ppoll                  73
/* fs/signalfd.c */
#define __NR_signalfd4              74
/* fs/splice.c */
#define __NR_vmsplice               75
#define __NR_splice                 76
#define __NR_tee                    77
/* fs/stat.c */
#define __NR_readlinkat             78
#define __NR_fstatat                79
#define __NR_fstat                  80
/* fs/sync.c */
#define __NR_sync                   81
#define __NR_fsync                  82
#define __NR_fdatasync              83
#define __NR_sync_file_range        84
/* fs/timerfd.c */
#define __NR_timerfd_create         85
#define __NR_timerfd_settime        86
#define __NR_timerfd_gettime        87
/* fs/utimes.c */
#define __NR_utimensat              88
/* kernel/acct.c */
#define __NR_acct                   89
/* kernel/capability.c */
#define __NR_capget                 90
#define __NR_capset                 91
/* kernel/exec_domain.c */
#define __NR_personality            92
/* kernel/exit.c */
#define __NR_exit                   93
#define __NR_exit_group             94
#define __NR_waitid                 95
/* kernel/fork.c */
#define __NR_set_tid_address        96
#define __NR_unshare                97
/* kernel/futex.c */
#define __NR_futex                  98
#define __NR_set_robust_list        99
#define __NR_get_robust_list        100
/* kernel/hrtimer.c */
#define __NR_nanosleep              101
/* kernel/itimer.c */
#define __NR_getitimer              102
#define __NR_setitimer              103
/* kernel/kexec.c */
#define __NR_kexec_load             104
/* kernel/module.c */
#define __NR_init_module            105
#define __NR_delete_module          106
/* kernel/posix-timers.c */
#define __NR_timer_create           107
#define __NR_timer_gettime          108
#define __NR_timer_getoverrun       109
#define __NR_timer_settime          110
#define __NR_timer_delete           111
#define __NR_clock_settime          112
#define __NR_clock_gettime          113
#define __NR_clock_getres           114
#define __NR_clock_nanosleep        115
/* kernel/printk.c */
#define __NR_syslog                 116
/* kernel/ptrace.c */
#define __NR_ptrace                 117
/* kernel/sched/core.c */
#define __NR_sched_setparam         118
#define __NR_sched_setscheduler     119
#define __NR_sched_getscheduler     120
#define __NR_sched_getparam         121
#define __NR_sched_setaffinity      122
#define __NR_sched_getaffinity      123
#define __NR_sched_yield            124
#define __NR_sched_get_priority_max 125
#define __NR_sched_get_priority_min 126
#define __NR_sched_rr_get_interval  127
/* kernel/signal.c */
#define __NR_restart_syscall        128
#define __NR_kill                   129
#define __NR_tkill                  130
#define __NR_tgkill                 131
#define __NR_sigaltstack            132
#define __NR_rt_sigsuspend          133
#define __NR_rt_sigaction           134
#define __NR_rt_sigprocmask         135
#define __NR_rt_sigpending          136
#define __NR_rt_sigtimedwait        137
#define __NR_rt_sigqueueinfo        138
#define __NR_rt_sigreturn           139
/* kernel/sys.c */
#define __NR_setpriority            140
#define __NR_getpriority            141
#define __NR_reboot                 142
#define __NR_setregid               143
#define __NR_setgid                 144
#define __NR_setreuid               145
#define __NR_setuid                 146
#define __NR_setresuid              147
#define __NR_getresuid              148
#define __NR_setresgid              149
#define __NR_getresgid              150
#define __NR_setfsuid               151
#define __NR_setfsgid               152
#define __NR_times                  153
#define __NR_setpgid                154
#define __NR_getpgid                155
#define __NR_getsid                 156
#define __NR_setsid                 157
#define __NR_getgroups              158
#define __NR_setgroups              159
#define __NR_uname                  160
#define __NR_sethostname            161
#define __NR_setdomainname          162
#define __NR_getrlimit              163
#define __NR_setrlimit              164
#define __NR_getrusage              165
#define __NR_umask                  166
#define __NR_prctl                  167
#define __NR_getcpu                 168
/* kernel/time.c */
#define __NR_gettimeofday           169
#define __NR_settimeofday           170
#define __NR_adjtimex               171
/* kernel/timer.c */
#define __NR_getpid                 172
#define __NR_getppid                173
#define __NR_getuid                 174
#define __NR_geteuid                175
#define __NR_getgid                 176
#define __NR_getegid                177
#define __NR_gettid                 178
#define __NR_sysinfo                179
/* ipc/mqueue.c */
#define __NR_mq_open                180
#define __NR_mq_unlink              181
#define __NR_mq_timedsend           182
#define __NR_mq_timedreceive        183
#define __NR_mq_notify              184
#define __NR_mq_getsetattr          185
/* ipc/msg.c */
#define __NR_msgget                 186
#define __NR_msgctl                 187
#define __NR_msgrcv                 188
#define __NR_msgsnd                 189
/* ipc/sem.c */
#define __NR_semget                 190
#define __NR_semctl                 191
#define __NR_semtimedop             192
#define __NR_semop                  193
/* ipc/shm.c */
#define __NR_shmget                 194
#define __NR_shmctl                 195
#define __NR_shmat                  196
#define __NR_shmdt                  197
/* net/socket.c */
#define __NR_socket                 198
#define __NR_socketpair             199
#define __NR_bind                   200
#define __NR_listen                 201
#define __NR_accept                 202
#define __NR_connect                203
#define __NR_getsockname            204
#define __NR_getpeername            205
#define __NR_sendto                 206
#define __NR_recvfrom               207
#define __NR_setsockopt             208
#define __NR_getsockopt             209
#define __NR_shutdown               210
#define __NR_sendmsg                211
#define __NR_recvmsg                212
/* mm/filemap.c */
#define __NR_readahead              213
/* mm/nommu.c, also with MMU */
#define __NR_brk                    214
#define __NR_munmap                 215
#define __NR_mremap                 216
/* security/keys/keyctl.c */
#define __NR_add_key                217
#define __NR_request_key            218
#define __NR_keyctl                 219
/* arch/example/kernel/sys_example.c */
#define __NR_clone                  220
#define __NR_execve                 221
#define __NR_mmap                   222
/* mm/fadvise.c */
#define __NR_fadvise64              223
/* mm/, CONFIG_MMU only */
#define __NR_swapon                 224
#define __NR_swapoff                225
#define __NR_mprotect               226
#define __NR_msync                  227
#define __NR_mlock                  228
#define __NR_munlock                229
#define __NR_mlockall               230
#define __NR_munlockall             231
#define __NR_mincore                232
#define __NR_madvise                233
#define __NR_remap_file_pages       234
#define __NR_mbind                  235
#define __NR_get_mempolicy          236
#define __NR_set_mempolicy          237
#define __NR_migrate_pages          238
#define __NR_move_pages             239
#define __NR_rt_tgsigqueueinfo      240
#define __NR_perf_event_open        241
#define __NR_accept4                242
#define __NR_recvmmsg               243
#define __NR_wait4                  260
#define __NR_prlimit64              261
#define __NR_fanotify_init          262
#define __NR_fanotify_mark          263
#define __NR_name_to_handle_at      264
#define __NR_open_by_handle_at      265
#define __NR_clock_adjtime          266
#define __NR_syncfs                 267
#define __NR_setns                  268
#define __NR_sendmmsg               269
#define __NR_process_vm_readv       270
#define __NR_process_vm_writev      271
#define __NR_kcmp                   272
#define __NR_finit_module           273
#define __NR_sched_setattr          274
#define __NR_sched_getattr          275
#define __NR_renameat2              276
#define __NR_seccomp                277
#define __NR_getrandom              278
#define __NR_memfd_create           279
#define __NR_bpf                    280
#define __NR_execveat               281
#define __NR_userfaultfd            282
#define __NR_membarrier             283
#define __NR_mlock2                 284
#define __NR_copy_file_range        285
#define __NR_preadv2                286
#define __NR_pwritev2               287
#define __NR_pkey_mprotect          288
#define __NR_pkey_alloc             289
#define __NR_pkey_free              290
#define __NR_statx                  291

__SYSCALL(__NR_io_setup, sys_io_setup)
__SYSCALL(__NR_io_destroy, sys_io_destroy)
__SYSCALL(__NR_io_submit, sys_io_submit)
__SYSCALL(__NR_io_cancel, sys_io_cancel)
__SYSCALL(__NR_io_getevents, sys_io_getevents)
__SYSCALL(__NR_setxattr, sys_setxattr)
__SYSCALL(__NR_lsetxattr, sys_lsetxattr)
__SYSCALL(__NR_fsetxattr, sys_fsetxattr)
__SYSCALL(__NR_getxattr, sys_getxattr)
__SYSCALL(__NR_lgetxattr, sys_lgetxattr)
__SYSCALL(__NR_fgetxattr, sys_fgetxattr)
__SYSCALL(__NR_listxattr, sys_listxattr)
__SYSCALL(__NR_llistxattr, sys_llistxattr)
__SYSCALL(__NR_flistxattr, sys_flistxattr)
__SYSCALL(__NR_removexattr, sys_removexattr)
__SYSCALL(__NR_lremovexattr, sys_lremovexattr)
__SYSCALL(__NR_fremovexattr, sys_fremovexattr)
__SYSCALL(__NR_getcwd, sys_getcwd)
__SYSCALL(__NR_lookup_dcookie, sys_lookup_dcookie)
__SYSCALL(__NR_eventfd2, sys_eventfd2)
__SYSCALL(__NR_epoll_create1, sys_epoll_create1)
__SYSCALL(__NR_epoll_ctl, sys_epoll_ctl)
__SYSCALL(__NR_epoll_pwait, sys_epoll_pwait)
__SYSCALL(__NR_dup, sys_dup)
__SYSCALL(__NR_dup3, sys_dup3)
__SYSCALL(__NR_fcntl, sys_fcntl)
__SYSCALL(__NR_inotify_init1, sys_inotify_init1)
__SYSCALL(__NR_inotify_add_watch, sys_inotify_add_watch)
__SYSCALL(__NR_inotify_rm_watch, sys_inotify_rm_watch)
__SYSCALL(__NR_ioctl, sys_ioctl)
__SYSCALL(__NR_ioprio_set, sys_ioprio_set)
__SYSCALL(__NR_ioprio_get, sys_ioprio_get)
__SYSCALL(__NR_flock, sys_flock)
__SYSCALL(__NR_mknodat, sys_mknodat)
__SYSCALL(__NR_mkdirat, sys_mkdirat)
__SYSCALL(__NR_unlinkat, sys_unlinkat)
__SYSCALL(__NR_symlinkat, sys_symlinkat)
__SYSCALL(__NR_linkat, sys_linkat)
__SYSCALL(__NR_renameat, sys_renameat)
__SYSCALL(__NR_umount2, sys_umount)
__SYSCALL(__NR_mount, sys_mount)
__SYSCALL(__NR_pivot_root, sys_pivot_root)
__SYSCALL(__NR_nfsservctl, sys_ni_syscall)
__SYSCALL(__NR_statfs, sys_statfs)
__SYSCALL(__NR_fstatfs, sys_fstatfs)
__SYSCALL(__NR_truncate, sys_truncate)
__SYSCALL(__NR_ftruncate, sys_ftruncate)
__SYSCALL(__NR_fallocate, sys_fallocate)
__SYSCALL(__NR_faccessat, sys_faccessat)
__SYSCALL(__NR_chdir, sys_chdir)
__SYSCALL(__NR_fchdir, sys_fchdir)
__SYSCALL(__NR_chroot, sys_chroot)
__SYSCALL(__NR_fchmod, sys_fchmod)
__SYSCALL(__NR_fchmodat, sys_fchmodat)
__SYSCALL(__NR_fchownat, sys_fchownat)
__SYSCALL(__NR_fchown, sys_fchown)
__SYSCALL(__NR_openat, sys_openat)
__SYSCALL(__NR_close, sys_close)
__SYSCALL(__NR_vhangup, sys_vhangup)
__SYSCALL(__NR_pipe, sys_pipe)
__SYSCALL(__NR_quotactl, sys_quotactl)
__SYSCALL(__NR_getdents64, sys_getdents64)
__SYSCALL(__NR_lseek, sys_llseek)
__SYSCALL(__NR_read, sys_read)
__SYSCALL(__NR_write, sys_write)
__SYSCALL(__NR_readv, sys_readv)
__SYSCALL(__NR_writev, sys_writev)
__SYSCALL(__NR_pread64, sys_pread64)
__SYSCALL(__NR_pwrite64, sys_pwrite64)
__SYSCALL(__NR_preadv, sys_preadv)
__SYSCALL(__NR_pwritev, sys_pwritev)
__SYSCALL(__NR_sendfile, sys_sendfile64)
__SYSCALL(__NR_pselect6, sys_pselect6)
__SYSCALL(__NR_ppoll, sys_ppoll)
__SYSCALL(__NR_signalfd4, sys_signalfd4)
__SYSCALL(__NR_vmsplice, sys_vmsplice)
__SYSCALL(__NR_splice, sys_splice)
__SYSCALL(__NR_tee, sys_tee)
__SYSCALL(__NR_readlinkat, sys_readlinkat)
__SYSCALL(__NR_fstatat, sys_fstatat64)
__SYSCALL(__NR_fstat, sys_fstat64)
__SYSCALL(__NR_sync, sys_sync)
__SYSCALL(__NR_fsync, sys_fsync)
__SYSCALL(__NR_fdatasync, sys_fdatasync)
__SYSCALL(__NR_sync_file_range2, sys_sync_file_range2)
__SYSCALL(__NR_sync_file_range, sys_sync_file_range)
__SYSCALL(__NR_timerfd_create, sys_timerfd_create)
__SYSCALL(__NR_timerfd_settime, sys_timerfd_settime)
__SYSCALL(__NR_timerfd_gettime, sys_timerfd_gettime)
__SYSCALL(__NR_utimensat, sys_utimensat)
__SYSCALL(__NR_acct, sys_acct)
__SYSCALL(__NR_capget, sys_capget)
__SYSCALL(__NR_capset, sys_capset)
__SYSCALL(__NR_personality, sys_personality)
__SYSCALL(__NR_exit, sys_exit)
__SYSCALL(__NR_exit_group, sys_exit_group)
__SYSCALL(__NR_waitid, sys_waitid)
__SYSCALL(__NR_set_tid_address, sys_set_tid_address)
__SYSCALL(__NR_unshare, sys_unshare)
__SYSCALL(__NR_futex, sys_futex)
__SYSCALL(__NR_set_robust_list, sys_set_robust_list)
__SYSCALL(__NR_get_robust_list, sys_get_robust_list)
__SYSCALL(__NR_nanosleep, sys_nanosleep)
__SYSCALL(__NR_getitimer, sys_getitimer)
__SYSCALL(__NR_setitimer, sys_setitimer)
__SYSCALL(__NR_kexec_load, sys_kexec_load)
__SYSCALL(__NR_init_module, sys_init_module)
__SYSCALL(__NR_delete_module, sys_delete_module)
__SYSCALL(__NR_timer_create, sys_timer_create)
__SYSCALL(__NR_timer_gettime, sys_timer_gettime)
__SYSCALL(__NR_timer_getoverrun, sys_timer_getoverrun)
__SYSCALL(__NR_timer_settime, sys_timer_settime)
__SYSCALL(__NR_timer_delete, sys_timer_delete)
__SYSCALL(__NR_clock_settime, sys_clock_settime)
__SYSCALL(__NR_clock_gettime, sys_clock_gettime)
__SYSCALL(__NR_clock_getres, sys_clock_getres)
__SYSCALL(__NR_clock_nanosleep, sys_clock_nanosleep)
__SYSCALL(__NR_syslog, sys_syslog)
__SYSCALL(__NR_ptrace, sys_ptrace)
__SYSCALL(__NR_sched_setparam, sys_sched_setparam)
__SYSCALL(__NR_sched_setscheduler, sys_sched_setscheduler)
__SYSCALL(__NR_sched_getscheduler, sys_sched_getscheduler)
__SYSCALL(__NR_sched_getparam, sys_sched_getparam)
__SYSCALL(__NR_sched_setaffinity, sys_sched_setaffinity)
__SYSCALL(__NR_sched_getaffinity, sys_sched_getaffinity)
__SYSCALL(__NR_sched_yield, sys_sched_yield)
__SYSCALL(__NR_sched_get_priority_max, sys_sched_get_priority_max)
__SYSCALL(__NR_sched_get_priority_min, sys_sched_get_priority_min)
__SYSCALL(__NR_sched_rr_get_interval, sys_sched_rr_get_interval)
__SYSCALL(__NR_restart_syscall, sys_restart_syscall)
__SYSCALL(__NR_kill, sys_kill)
__SYSCALL(__NR_tkill, sys_tkill)
__SYSCALL(__NR_tgkill, sys_tgkill)
__SYSCALL(__NR_sigaltstack, sys_sigaltstack)
__SYSCALL(__NR_rt_sigsuspend, sys_rt_sigsuspend)
__SYSCALL(__NR_rt_sigaction, sys_rt_sigaction)
__SYSCALL(__NR_rt_sigprocmask, sys_rt_sigprocmask)
__SYSCALL(__NR_rt_sigpending, sys_rt_sigpending)
__SYSCALL(__NR_rt_sigtimedwait, sys_rt_sigtimedwait)
__SYSCALL(__NR_rt_sigqueueinfo, sys_rt_sigqueueinfo)
__SYSCALL(__NR_rt_sigreturn, sys_rt_sigreturn)
__SYSCALL(__NR_setpriority, sys_setpriority)
__SYSCALL(__NR_getpriority, sys_getpriority)
__SYSCALL(__NR_reboot, sys_reboot)
__SYSCALL(__NR_setregid, sys_setregid)
__SYSCALL(__NR_setgid, sys_setgid)
__SYSCALL(__NR_setreuid, sys_setreuid)
__SYSCALL(__NR_setuid, sys_setuid)
__SYSCALL(__NR_setresuid, sys_setresuid)
__SYSCALL(__NR_getresuid, sys_getresuid)
__SYSCALL(__NR_setresgid, sys_setresgid)
__SYSCALL(__NR_getresgid, sys_getresgid)
__SYSCALL(__NR_setfsuid, sys_setfsuid)
__SYSCALL(__NR_setfsgid, sys_setfsgid)
__SYSCALL(__NR_times, sys_times)
__SYSCALL(__NR_setpgid, sys_setpgid)
__SYSCALL(__NR_getpgid, sys_getpgid)
__SYSCALL(__NR_getsid, sys_getsid)
__SYSCALL(__NR_setsid, sys_setsid)
__SYSCALL(__NR_getgroups, sys_getgroups)
__SYSCALL(__NR_setgroups, sys_setgroups)
__SYSCALL(__NR_uname, sys_newuname)
__SYSCALL(__NR_sethostname, sys_sethostname)
__SYSCALL(__NR_setdomainname, sys_setdomainname)
__SYSCALL(__NR_getrlimit, sys_getrlimit)
__SYSCALL(__NR_setrlimit, sys_setrlimit)
__SYSCALL(__NR_getrusage, sys_getrusage)
__SYSCALL(__NR_umask, sys_umask)
__SYSCALL(__NR_prctl, sys_prctl)
__SYSCALL(__NR_getcpu, sys_getcpu)
__SYSCALL(__NR_gettimeofday, sys_gettimeofday)
__SYSCALL(__NR_settimeofday, sys_settimeofday)
__SYSCALL(__NR_adjtimex, sys_adjtimex)
__SYSCALL(__NR_getpid, sys_getpid)
__SYSCALL(__NR_getppid, sys_getppid)
__SYSCALL(__NR_getuid, sys_getuid)
__SYSCALL(__NR_geteuid, sys_geteuid)
__SYSCALL(__NR_getgid, sys_getgid)
__SYSCALL(__NR_getegid, sys_getegid)
__SYSCALL(__NR_gettid, sys_gettid)
__SYSCALL(__NR_sysinfo, sys_sysinfo)
__SYSCALL(__NR_mq_open, sys_mq_open)
__SYSCALL(__NR_mq_unlink, sys_mq_unlink)
__SYSCALL(__NR_mq_timedsend, sys_mq_timedsend)
__SYSCALL(__NR_mq_timedreceive, sys_mq_timedreceive)
__SYSCALL(__NR_mq_notify, sys_mq_notify)
__SYSCALL(__NR_mq_getsetattr, sys_mq_getsetattr)
__SYSCALL(__NR_msgget, sys_msgget)
__SYSCALL(__NR_msgctl, sys_msgctl)
__SYSCALL(__NR_msgrcv, sys_msgrcv)
__SYSCALL(__NR_msgsnd, sys_msgsnd)
__SYSCALL(__NR_semget, sys_semget)
__SYSCALL(__NR_semctl, sys_semctl)
__SYSCALL(__NR_semtimedop, sys_semtimedop)
__SYSCALL(__NR_semop, sys_semop)
__SYSCALL(__NR_shmget, sys_shmget)
__SYSCALL(__NR_shmctl, sys_shmctl)
__SYSCALL(__NR_shmat, sys_shmat)
__SYSCALL(__NR_shmdt, sys_shmdt)
__SYSCALL(__NR_socket, sys_socket)
__SYSCALL(__NR_socketpair, sys_socketpair)
__SYSCALL(__NR_bind, sys_bind)
__SYSCALL(__NR_listen, sys_listen)
__SYSCALL(__NR_accept, sys_accept)
__SYSCALL(__NR_connect, sys_connect)
__SYSCALL(__NR_getsockname, sys_getsockname)
__SYSCALL(__NR_getpeername, sys_getpeername)
__SYSCALL(__NR_sendto, sys_sendto)
__SYSCALL(__NR_recvfrom, sys_recvfrom)
__SYSCALL(__NR_setsockopt, sys_setsockopt)
__SYSCALL(__NR_getsockopt, sys_getsockopt)
__SYSCALL(__NR_shutdown, sys_shutdown)
__SYSCALL(__NR_sendmsg, sys_sendmsg)
__SYSCALL(__NR_recvmsg, sys_recvmsg)
__SYSCALL(__NR_readahead, sys_readahead)
__SYSCALL(__NR_brk, sys_brk)
__SYSCALL(__NR_munmap, sys_munmap)
__SYSCALL(__NR_mremap, sys_mremap)
__SYSCALL(__NR_add_key, sys_add_key)
__SYSCALL(__NR_request_key, sys_request_key)
__SYSCALL(__NR_keyctl, sys_keyctl)
__SYSCALL(__NR_clone, sys_clone)
__SYSCALL(__NR_execve, sys_execve)
__SYSCALL(__NR_mmap, sys_mmap)
__SYSCALL(__NR_fadvise64, sys_fadvise64_64)
__SYSCALL(__NR_swapon, sys_swapon)
__SYSCALL(__NR_swapoff, sys_swapoff)
__SYSCALL(__NR_mprotect, sys_mprotect)
__SYSCALL(__NR_msync, sys_msync)
__SYSCALL(__NR_mlock, sys_mlock)
__SYSCALL(__NR_munlock, sys_munlock)
__SYSCALL(__NR_mlockall, sys_mlockall)
__SYSCALL(__NR_munlockall, sys_munlockall)
__SYSCALL(__NR_mincore, sys_mincore)
__SYSCALL(__NR_madvise, sys_madvise)
__SYSCALL(__NR_remap_file_pages, sys_remap_file_pages)
__SYSCALL(__NR_mbind, sys_mbind)
__SYSCALL(__NR_get_mempolicy, sys_get_mempolicy)
__SYSCALL(__NR_set_mempolicy, sys_set_mempolicy)
__SYSCALL(__NR_migrate_pages, sys_migrate_pages)
__SYSCALL(__NR_move_pages, sys_move_pages)
__SYSCALL(__NR_rt_tgsigqueueinfo, sys_rt_tgsigqueueinfo)
__SYSCALL(__NR_perf_event_open, sys_perf_event_open)
__SYSCALL(__NR_accept4, sys_accept4)
__SYSCALL(__NR_recvmmsg, sys_recvmmsg)
__SYSCALL(__NR_wait4, sys_wait4)
__SYSCALL(__NR_prlimit64, sys_prlimit64)
__SYSCALL(__NR_fanotify_init, sys_fanotify_init)
__SYSCALL(__NR_fanotify_mark, sys_fanotify_mark)
__SYSCALL(__NR_name_to_handle_at, sys_name_to_handle_at)
__SYSCALL(__NR_open_by_handle_at, sys_open_by_handle_at)
__SYSCALL(__NR_clock_adjtime, sys_clock_adjtime)
__SYSCALL(__NR_syncfs, sys_syncfs)
__SYSCALL(__NR_setns, sys_setns)
__SYSCALL(__NR_sendmmsg, sys_sendmmsg)
__SYSCALL(__NR_process_vm_readv, sys_process_vm_readv)
__SYSCALL(__NR_process_vm_writev, sys_process_vm_writev)
__SYSCALL(__NR_kcmp, sys_kcmp)
__SYSCALL(__NR_finit_module, sys_finit_module)
__SYSCALL(__NR_sched_setattr, sys_sched_setattr)
__SYSCALL(__NR_sched_getattr, sys_sched_getattr)
__SYSCALL(__NR_renameat2, sys_renameat2)
__SYSCALL(__NR_seccomp, sys_seccomp)
__SYSCALL(__NR_getrandom, sys_getrandom)
__SYSCALL(__NR_memfd_create, sys_memfd_create)
__SYSCALL(__NR_bpf, sys_bpf)
__SYSCALL(__NR_execveat, sys_execveat)
__SYSCALL(__NR_userfaultfd, sys_userfaultfd)
__SYSCALL(__NR_membarrier, sys_membarrier)
__SYSCALL(__NR_mlock2, sys_mlock2)
__SYSCALL(__NR_copy_file_range, sys_copy_file_range)
__SYSCALL(__NR_preadv2, sys_preadv2)
__SYSCALL(__NR_pwritev2, sys_pwritev2)
__SYSCALL(__NR_pkey_mprotect, sys_pkey_mprotect)
__SYSCALL(__NR_pkey_alloc, sys_pkey_alloc)
__SYSCALL(__NR_pkey_free, sys_pkey_free)
__SYSCALL(__NR_statx, sys_statx)
