#include <stdint.h>

// TODO: remove syscalls I don't think I'll need

#ifndef __SYSCALL
#define __SYSCALL(_nr, _sys)
#endif

/* The syscall numbers */
#define __NR_read 0
#define __NR_write 1
#define __NR_open 2
#define __NR_close 3
#define __NR_stat 4
#define __NR_fstat 5
#define __NR_lstat 6
#define __NR_poll 7
#define __NR_lseek 8
#define __NR_mmap 9
#define __NR_mprotect 10
#define __NR_munmap 11
#define __NR_brk 12
#define __NR_rt_sigaction 13
#define __NR_rt_sigprocmask 14
#define __NR_rt_sigreturn 15
#define __NR_ioctl 16
#define __NR_pread64 17
#define __NR_pwrite64 18
#define __NR_readv 19
#define __NR_writev 20
#define __NR_access 21
#define __NR_pipe 22
#define __NR_select 23
#define __NR_sched_yield 24
#define __NR_mremap 25
#define __NR_msync 26
#define __NR_mincore 27
#define __NR_madvise 28
#define __NR_shmget 29
#define __NR_shmat 30
#define __NR_shmctl 31
#define __NR_dup 32
#define __NR_dup2 33
#define __NR_pause 34
#define __NR_nanosleep 35
#define __NR_getitimer 36
#define __NR_alarm 37
#define __NR_setitimer 38
#define __NR_getpid 39
#define __NR_sendfile 40
#define __NR_socket 41
#define __NR_connect 42
#define __NR_accept 43
#define __NR_sendto 44
#define __NR_recvfrom 45
#define __NR_sendmsg 46
#define __NR_recvmsg 47
#define __NR_shutdown 48
#define __NR_bind 49
#define __NR_listen 50
#define __NR_getsockname 51
#define __NR_getpeername 52
#define __NR_socketpair 53
#define __NR_setsockopt 54
#define __NR_getsockopt 55
#define __NR_clone 56
#define __NR_fork 57
#define __NR_vfork 58
#define __NR_execve 59
#define __NR_exit 60
#define __NR_wait4 61
#define __NR_kill 62
#define __NR_uname 63
#define __NR_semget 64
#define __NR_semop 65
#define __NR_semctl 66
#define __NR_shmdt 67
#define __NR_msgget 68
#define __NR_msgsnd 69
#define __NR_msgrcv 70
#define __NR_msgctl 71
#define __NR_fcntl 72
#define __NR_flock 73
#define __NR_fsync 74
#define __NR_fdatasync 75
#define __NR_truncate 76
#define __NR_ftruncate 77
#define __NR_getdents 78
#define __NR_getcwd 79
#define __NR_chdir 80
#define __NR_fchdir 81
#define __NR_rename 82
#define __NR_mkdir 83
#define __NR_rmdir 84
#define __NR_creat 85
#define __NR_link 86
#define __NR_unlink 87
#define __NR_symlink 88
#define __NR_readlink 89
#define __NR_chmod 90
#define __NR_fchmod 91
#define __NR_chown 92
#define __NR_fchown 93
#define __NR_lchown 94
#define __NR_umask 95
#define __NR_gettimeofday 96
#define __NR_getrlimit 97
#define __NR_getrusage 98
#define __NR_sysinfo 99
#define __NR_times 100
#define __NR_ptrace 101
#define __NR_getuid 102
#define __NR_syslog 103
#define __NR_getgid 104
#define __NR_setuid 105
#define __NR_setgid 106
#define __NR_geteuid 107
#define __NR_getegid 108
#define __NR_setpgid 109
#define __NR_getppid 110
#define __NR_getpgrp 111
#define __NR_setsid 112
#define __NR_setreuid 113
#define __NR_setregid 114
#define __NR_getgroups 115
#define __NR_setgroups 116
#define __NR_setresuid 117
#define __NR_getresuid 118
#define __NR_setresgid 119
#define __NR_getresgid 120
#define __NR_getpgid 121
#define __NR_setfsuid 122
#define __NR_setfsgid 123
#define __NR_getsid 124
#define __NR_capget 125
#define __NR_capset 126
#define __NR_rt_sigpending 127
#define __NR_rt_sigtimedwait 128
#define __NR_rt_sigqueueinfo 129
#define __NR_rt_sigsuspend 130
#define __NR_sigaltstack 131
#define __NR_utime 132
#define __NR_mknod 133
#define __NR_uselib 134
#define __NR_personality 135
#define __NR_ustat 136
#define __NR_statfs 137
#define __NR_fstatfs 138
#define __NR_sysfs 139
#define __NR_getpriority 140
#define __NR_setpriority 141
#define __NR_sched_setparam 142
#define __NR_sched_getparam 143
#define __NR_sched_setscheduler 144
#define __NR_sched_getscheduler 145
#define __NR_sched_get_priority_max 146
#define __NR_sched_get_priority_min 147
#define __NR_sched_rr_get_interval 148
#define __NR_mlock 149
#define __NR_munlock 150
#define __NR_mlockall 151
#define __NR_munlockall 152
#define __NR_vhangup 153
#define __NR_modify_ldt 154
#define __NR_pivot_root 155
#define __NR__sysctl 156
#define __NR_prctl 157
#define __NR_arch_prctl 158
#define __NR_adjtimex 159
#define __NR_setrlimit 160
#define __NR_chroot 161
#define __NR_sync 162
#define __NR_acct 163
#define __NR_settimeofday 164
#define __NR_mount 165
#define __NR_umount2 166
#define __NR_swapon 167
#define __NR_swapoff 168
#define __NR_reboot 169
#define __NR_sethostname 170
#define __NR_setdomainname 171
#define __NR_iopl 172
#define __NR_ioperm 173
#define __NR_create_module 174
#define __NR_init_module 175
#define __NR_delete_module 176
#define __NR_get_kernel_syms 177
#define __NR_query_module 178
#define __NR_quotactl 179
#define __NR_nfsservctl 180
#define __NR_getpmsg 181
#define __NR_putpmsg 182
#define __NR_afs_syscall 183
#define __NR_tuxcall 184
#define __NR_security 185
#define __NR_gettid 186
#define __NR_readahead 187
#define __NR_setxattr 188
#define __NR_lsetxattr 189
#define __NR_fsetxattr 190
#define __NR_getxattr 191
#define __NR_lgetxattr 192
#define __NR_fgetxattr 193
#define __NR_listxattr 194
#define __NR_llistxattr 195
#define __NR_flistxattr 196
#define __NR_removexattr 197
#define __NR_lremovexattr 198
#define __NR_fremovexattr 199
#define __NR_tkill 200
#define __NR_time 201
#define __NR_futex 202
#define __NR_sched_setaffinity 203
#define __NR_sched_getaffinity 204
#define __NR_set_thread_area 205
#define __NR_io_setup 206
#define __NR_io_destroy 207
#define __NR_io_getevents 208
#define __NR_io_submit 209
#define __NR_io_cancel 210
#define __NR_get_thread_area 211
#define __NR_lookup_dcookie 212
#define __NR_epoll_create 213
#define __NR_epoll_ctl_old 214
#define __NR_epoll_wait_old 215
#define __NR_remap_file_pages 216
#define __NR_getdents64 217
#define __NR_set_tid_address 218
#define __NR_restart_syscall 219
#define __NR_semtimedop 220
#define __NR_fadvise64 221
#define __NR_timer_create 222
#define __NR_timer_settime 223
#define __NR_timer_gettime 224
#define __NR_timer_getoverrun 225
#define __NR_timer_delete 226
#define __NR_clock_settime 227
#define __NR_clock_gettime 228
#define __NR_clock_getres 229
#define __NR_clock_nanosleep 230
#define __NR_exit_group 231
#define __NR_epoll_wait 232
#define __NR_epoll_ctl 233
#define __NR_tgkill 234
#define __NR_utimes 235
#define __NR_vserver 236
#define __NR_mbind 237
#define __NR_set_mempolicy 238
#define __NR_get_mempolicy 239
#define __NR_mq_open 240
#define __NR_mq_unlink 241
#define __NR_mq_timedsend 242
#define __NR_mq_timedreceive 243
#define __NR_mq_notify 244
#define __NR_mq_getsetattr 245
#define __NR_kexec_load 246
#define __NR_waitid 247
#define __NR_add_key 248
#define __NR_request_key 249
#define __NR_keyctl 250
#define __NR_ioprio_set 251
#define __NR_ioprio_get 252
#define __NR_inotify_init 253
#define __NR_inotify_add_watch 254
#define __NR_inotify_rm_watch 255
#define __NR_migrate_pages 256
#define __NR_openat 257
#define __NR_mkdirat 258
#define __NR_mknodat 259
#define __NR_fchownat 260
#define __NR_futimesat 261
#define __NR_newfstatat 262
#define __NR_unlinkat 263
#define __NR_renameat 264
#define __NR_linkat 265
#define __NR_symlinkat 266
#define __NR_readlinkat 267
#define __NR_fchmodat 268
#define __NR_faccessat 269
#define __NR_pselect6 270
#define __NR_ppoll 271
#define __NR_unshare 272
#define __NR_set_robust_list 273
#define __NR_get_robust_list 274
#define __NR_splice 275
#define __NR_tee 276
#define __NR_sync_file_range 277
#define __NR_vmsplice 278
#define __NR_move_pages 279
#define __NR_utimensat 280
#define __NR_epoll_pwait 281
#define __NR_signalfd 282
#define __NR_timerfd_create 283
#define __NR_eventfd 284
#define __NR_fallocate 285
#define __NR_timerfd_settime 286
#define __NR_timerfd_gettime 287
#define __NR_accept4 288
#define __NR_signalfd4 289
#define __NR_eventfd2 290
#define __NR_epoll_create1 291
#define __NR_dup3 292
#define __NR_pipe2 293
#define __NR_inotify_init1 294
#define __NR_preadv 295
#define __NR_pwritev 296
#define __NR_rt_tgsigqueueinfo 297
#define __NR_perf_event_open 298
#define __NR_recvmmsg 299
#define __NR_fanotify_init 300
#define __NR_fanotify_mark 301
#define __NR_prlimit64 302
#define __NR_name_to_handle_at 303
#define __NR_open_by_handle_at 304
#define __NR_clock_adjtime 305
#define __NR_syncfs 306
#define __NR_sendmmsg 307
#define __NR_setns 308
#define __NR_getcpu 309
#define __NR_process_vm_readv 310
#define __NR_process_vm_writev 311
#define __NR_kcmp 312
#define __NR_finit_module 313
#define __NR_sched_setattr 314
#define __NR_sched_getattr 315
#define __NR_renameat2 316
#define __NR_seccomp 317
#define __NR_getrandom 318
#define __NR_memfd_create 319
#define __NR_kexec_file_load 320
#define __NR_bpf 321
#define __NR_execveat 322
#define __NR_userfaultfd 323
#define __NR_membarrier 324
#define __NR_mlock2 325
#define __NR_copy_file_range 326
#define __NR_preadv2 327
#define __NR_pwritev2 328
#define __NR_pkey_mprotect 329
#define __NR_pkey_alloc 330
#define __NR_pkey_free 331
#define __NR_statx 332
#define __NR_io_pgetevents 333
#define __NR_rseq 334
#define __NR_pidfd_send_signal 424
#define __NR_io_uring_setup 425
#define __NR_io_uring_enter 426
#define __NR_io_uring_register 427
#define __NR_open_tree 428
#define __NR_move_mount 429
#define __NR_fsopen 430
#define __NR_fsconfig 431
#define __NR_fsmount 432
#define __NR_fspick 433
#define __NR_pidfd_open 434
#define __NR_clone3 435
#define __NR_close_range 436
#define __NR_openat2 437
#define __NR_pidfd_getfd 438
#define __NR_faccessat2 439
#define __NR_process_madvise 440
#define __NR_epoll_pwait2 441
#define __NR_mount_setattr 442

__SYSCALL(__NR_read, sys_ni_syscall) // 0
__SYSCALL(__NR_write, sys_ni_syscall) // 1
__SYSCALL(__NR_open, sys_ni_syscall) // 2
__SYSCALL(__NR_close, sys_ni_syscall) // 3
__SYSCALL(__NR_stat, sys_ni_syscall) // 4
__SYSCALL(__NR_fstat, sys_ni_syscall) // 5
__SYSCALL(__NR_lstat, sys_ni_syscall) // 6
__SYSCALL(__NR_poll, sys_ni_syscall) // 7
__SYSCALL(__NR_lseek, sys_ni_syscall) // 8
__SYSCALL(__NR_mmap, sys_ni_syscall) // 9
__SYSCALL(__NR_mprotect, sys_ni_syscall) // 10
__SYSCALL(__NR_munmap, sys_ni_syscall) // 11
__SYSCALL(__NR_brk, sys_ni_syscall) // 12
__SYSCALL(__NR_rt_sigaction, sys_ni_syscall) // 13
__SYSCALL(__NR_rt_sigprocmask, sys_ni_syscall) // 14
__SYSCALL(__NR_rt_sigreturn, sys_ni_syscall) // 15
__SYSCALL(__NR_ioctl, sys_ni_syscall) // 16
__SYSCALL(__NR_pread64, sys_ni_syscall) // 17
__SYSCALL(__NR_pwrite64, sys_ni_syscall) // 18
__SYSCALL(__NR_readv, sys_ni_syscall) // 19
__SYSCALL(__NR_writev, sys_ni_syscall) // 20
__SYSCALL(__NR_access, sys_ni_syscall) // 21
__SYSCALL(__NR_pipe, sys_ni_syscall) // 22
__SYSCALL(__NR_select, sys_ni_syscall) // 23
__SYSCALL(__NR_sched_yield, sys_ni_syscall) // 24
__SYSCALL(__NR_mremap, sys_ni_syscall) // 25
__SYSCALL(__NR_msync, sys_ni_syscall) // 26
__SYSCALL(__NR_mincore, sys_ni_syscall) // 27
__SYSCALL(__NR_madvise, sys_ni_syscall) // 28
__SYSCALL(__NR_shmget, sys_ni_syscall) // 29
__SYSCALL(__NR_shmat, sys_ni_syscall) // 30
__SYSCALL(__NR_shmctl, sys_ni_syscall) // 31
__SYSCALL(__NR_dup, sys_ni_syscall) // 32
__SYSCALL(__NR_dup2, sys_ni_syscall) // 33
__SYSCALL(__NR_pause, sys_ni_syscall) // 34
__SYSCALL(__NR_nanosleep, sys_ni_syscall) // 35
__SYSCALL(__NR_getitimer, sys_ni_syscall) // 36
__SYSCALL(__NR_alarm, sys_ni_syscall) // 37
__SYSCALL(__NR_setitimer, sys_ni_syscall) // 38
__SYSCALL(__NR_getpid, sys_ni_syscall) // 39
__SYSCALL(__NR_sendfile, sys_ni_syscall) // 40
__SYSCALL(__NR_socket, sys_ni_syscall) // 41
__SYSCALL(__NR_connect, sys_ni_syscall) // 42
__SYSCALL(__NR_accept, sys_ni_syscall) // 43
__SYSCALL(__NR_sendto, sys_ni_syscall) // 44
__SYSCALL(__NR_recvfrom, sys_ni_syscall) // 45
__SYSCALL(__NR_sendmsg, sys_ni_syscall) // 46
__SYSCALL(__NR_recvmsg, sys_ni_syscall) // 47
__SYSCALL(__NR_shutdown, sys_ni_syscall) // 48
__SYSCALL(__NR_bind, sys_ni_syscall) // 49
__SYSCALL(__NR_listen, sys_ni_syscall) // 50
__SYSCALL(__NR_getsockname, sys_ni_syscall) // 51
__SYSCALL(__NR_getpeername, sys_ni_syscall) // 52
__SYSCALL(__NR_socketpair, sys_ni_syscall) // 53
__SYSCALL(__NR_setsockopt, sys_ni_syscall) // 54
__SYSCALL(__NR_getsockopt, sys_ni_syscall) // 55
__SYSCALL(__NR_clone, sys_ni_syscall) // 56
__SYSCALL(__NR_fork, sys_ni_syscall) // 57
__SYSCALL(__NR_vfork, sys_ni_syscall) // 58
__SYSCALL(__NR_execve, sys_ni_syscall) // 59
__SYSCALL(__NR_exit, sys_ni_syscall) // 60
__SYSCALL(__NR_wait4, sys_ni_syscall) // 61
__SYSCALL(__NR_kill, sys_ni_syscall) // 62
__SYSCALL(__NR_uname, sys_ni_syscall) // 63
__SYSCALL(__NR_semget, sys_ni_syscall) // 64
__SYSCALL(__NR_semop, sys_ni_syscall) // 65
__SYSCALL(__NR_semctl, sys_ni_syscall) // 66
__SYSCALL(__NR_shmdt, sys_ni_syscall) // 67
__SYSCALL(__NR_msgget, sys_ni_syscall) // 68
__SYSCALL(__NR_msgsnd, sys_ni_syscall) // 69
__SYSCALL(__NR_msgrcv, sys_ni_syscall) // 70
__SYSCALL(__NR_msgctl, sys_ni_syscall) // 71
__SYSCALL(__NR_fcntl, sys_ni_syscall) // 72
__SYSCALL(__NR_flock, sys_ni_syscall) // 73
__SYSCALL(__NR_fsync, sys_ni_syscall) // 74
__SYSCALL(__NR_fdatasync, sys_ni_syscall) // 75
__SYSCALL(__NR_truncate, sys_ni_syscall) // 76
__SYSCALL(__NR_ftruncate, sys_ni_syscall) // 77
__SYSCALL(__NR_getdents, sys_ni_syscall) // 78
__SYSCALL(__NR_getcwd, sys_ni_syscall) // 79
__SYSCALL(__NR_chdir, sys_ni_syscall) // 80
__SYSCALL(__NR_fchdir, sys_ni_syscall) // 81
__SYSCALL(__NR_rename, sys_ni_syscall) // 82
__SYSCALL(__NR_mkdir, sys_ni_syscall) // 83
__SYSCALL(__NR_rmdir, sys_ni_syscall) // 84
__SYSCALL(__NR_creat, sys_ni_syscall) // 85
__SYSCALL(__NR_link, sys_ni_syscall) // 86
__SYSCALL(__NR_unlink, sys_ni_syscall) // 87
__SYSCALL(__NR_symlink, sys_ni_syscall) // 88
__SYSCALL(__NR_readlink, sys_ni_syscall) // 89
__SYSCALL(__NR_chmod, sys_ni_syscall) // 90
__SYSCALL(__NR_fchmod, sys_ni_syscall) // 91
__SYSCALL(__NR_chown, sys_ni_syscall) // 92
__SYSCALL(__NR_fchown, sys_ni_syscall) // 93
__SYSCALL(__NR_lchown, sys_ni_syscall) // 94
__SYSCALL(__NR_umask, sys_ni_syscall) // 95
__SYSCALL(__NR_gettimeofday, sys_ni_syscall) // 96
__SYSCALL(__NR_getrlimit, sys_ni_syscall) // 97
__SYSCALL(__NR_getrusage, sys_ni_syscall) // 98
__SYSCALL(__NR_sysinfo, sys_ni_syscall) // 99
__SYSCALL(__NR_times, sys_ni_syscall) // 100
__SYSCALL(__NR_ptrace, sys_ni_syscall) // 101
__SYSCALL(__NR_getuid, sys_ni_syscall) // 102
__SYSCALL(__NR_syslog, sys_ni_syscall) // 103
__SYSCALL(__NR_getgid, sys_ni_syscall) // 104
__SYSCALL(__NR_setuid, sys_ni_syscall) // 105
__SYSCALL(__NR_setgid, sys_ni_syscall) // 106
__SYSCALL(__NR_geteuid, sys_ni_syscall) // 107
__SYSCALL(__NR_getegid, sys_ni_syscall) // 108
__SYSCALL(__NR_setpgid, sys_ni_syscall) // 109
__SYSCALL(__NR_getppid, sys_ni_syscall) // 110
__SYSCALL(__NR_getpgrp, sys_ni_syscall) // 111
__SYSCALL(__NR_setsid, sys_ni_syscall) // 112
__SYSCALL(__NR_setreuid, sys_ni_syscall) // 113
__SYSCALL(__NR_setregid, sys_ni_syscall) // 114
__SYSCALL(__NR_getgroups, sys_ni_syscall) // 115
__SYSCALL(__NR_setgroups, sys_ni_syscall) // 116
__SYSCALL(__NR_setresuid, sys_ni_syscall) // 117
__SYSCALL(__NR_getresuid, sys_ni_syscall) // 118
__SYSCALL(__NR_setresgid, sys_ni_syscall) // 119
__SYSCALL(__NR_getresgid, sys_ni_syscall) // 120
__SYSCALL(__NR_getpgid, sys_ni_syscall) // 121
__SYSCALL(__NR_setfsuid, sys_ni_syscall) // 122
__SYSCALL(__NR_setfsgid, sys_ni_syscall) // 123
__SYSCALL(__NR_getsid, sys_ni_syscall) // 124
__SYSCALL(__NR_capget, sys_ni_syscall) // 125
__SYSCALL(__NR_capset, sys_ni_syscall) // 126
__SYSCALL(__NR_rt_sigpending, sys_ni_syscall) // 127
__SYSCALL(__NR_rt_sigtimedwait, sys_ni_syscall) // 128
__SYSCALL(__NR_rt_sigqueueinfo, sys_ni_syscall) // 129
__SYSCALL(__NR_rt_sigsuspend, sys_ni_syscall) // 130
__SYSCALL(__NR_sigaltstack, sys_ni_syscall) // 131
__SYSCALL(__NR_utime, sys_ni_syscall) // 132
__SYSCALL(__NR_mknod, sys_ni_syscall) // 133
__SYSCALL(__NR_uselib, sys_ni_syscall) // 134
__SYSCALL(__NR_personality, sys_ni_syscall) // 135
__SYSCALL(__NR_ustat, sys_ni_syscall) // 136
__SYSCALL(__NR_statfs, sys_ni_syscall) // 137
__SYSCALL(__NR_fstatfs, sys_ni_syscall) // 138
__SYSCALL(__NR_sysfs, sys_ni_syscall) // 139
__SYSCALL(__NR_getpriority, sys_ni_syscall) // 140
__SYSCALL(__NR_setpriority, sys_ni_syscall) // 141
__SYSCALL(__NR_sched_setparam, sys_ni_syscall) // 142
__SYSCALL(__NR_sched_getparam, sys_ni_syscall) // 143
__SYSCALL(__NR_sched_setscheduler, sys_ni_syscall) // 144
__SYSCALL(__NR_sched_getscheduler, sys_ni_syscall) // 145
__SYSCALL(__NR_sched_get_priority_max, sys_ni_syscall) // 146
__SYSCALL(__NR_sched_get_priority_min, sys_ni_syscall) // 147
__SYSCALL(__NR_sched_rr_get_interval, sys_ni_syscall) // 148
__SYSCALL(__NR_mlock, sys_ni_syscall) // 149
__SYSCALL(__NR_munlock, sys_ni_syscall) // 150
__SYSCALL(__NR_mlockall, sys_ni_syscall) // 151
__SYSCALL(__NR_munlockall, sys_ni_syscall) // 152
__SYSCALL(__NR_vhangup, sys_ni_syscall) // 153
__SYSCALL(__NR_modify_ldt, sys_ni_syscall) // 154
__SYSCALL(__NR_pivot_root, sys_ni_syscall) // 155
__SYSCALL(__NR__sysctl, sys_ni_syscall) // 156
__SYSCALL(__NR_prctl, sys_ni_syscall) // 157
__SYSCALL(__NR_arch_prctl, sys_ni_syscall) // 158
__SYSCALL(__NR_adjtimex, sys_ni_syscall) // 159
__SYSCALL(__NR_setrlimit, sys_ni_syscall) // 160
__SYSCALL(__NR_chroot, sys_ni_syscall) // 161
__SYSCALL(__NR_sync, sys_ni_syscall) // 162
__SYSCALL(__NR_acct, sys_ni_syscall) // 163
__SYSCALL(__NR_settimeofday, sys_ni_syscall) // 164
__SYSCALL(__NR_mount, sys_ni_syscall) // 165
__SYSCALL(__NR_umount2, sys_ni_syscall) // 166
__SYSCALL(__NR_swapon, sys_ni_syscall) // 167
__SYSCALL(__NR_swapoff, sys_ni_syscall) // 168
__SYSCALL(__NR_reboot, sys_ni_syscall) // 169
__SYSCALL(__NR_sethostname, sys_ni_syscall) // 170
__SYSCALL(__NR_setdomainname, sys_ni_syscall) // 171
__SYSCALL(__NR_iopl, sys_ni_syscall) // 172
__SYSCALL(__NR_ioperm, sys_ni_syscall) // 173
__SYSCALL(__NR_create_module, sys_ni_syscall) // 174
__SYSCALL(__NR_init_module, sys_ni_syscall) // 175
__SYSCALL(__NR_delete_module, sys_ni_syscall) // 176
__SYSCALL(__NR_get_kernel_syms, sys_ni_syscall) // 177
__SYSCALL(__NR_query_module, sys_ni_syscall) // 178
__SYSCALL(__NR_quotactl, sys_ni_syscall) // 179
__SYSCALL(__NR_nfsservctl, sys_ni_syscall) // 180
__SYSCALL(__NR_getpmsg, sys_ni_syscall) // 181
__SYSCALL(__NR_putpmsg, sys_ni_syscall) // 182
__SYSCALL(__NR_afs_syscall, sys_ni_syscall) // 183
__SYSCALL(__NR_tuxcall, sys_ni_syscall) // 184
__SYSCALL(__NR_security, sys_ni_syscall) // 185
__SYSCALL(__NR_gettid, sys_ni_syscall) // 186
__SYSCALL(__NR_readahead, sys_ni_syscall) // 187
__SYSCALL(__NR_setxattr, sys_ni_syscall) // 188
__SYSCALL(__NR_lsetxattr, sys_ni_syscall) // 189
__SYSCALL(__NR_fsetxattr, sys_ni_syscall) // 190
__SYSCALL(__NR_getxattr, sys_ni_syscall) // 191
__SYSCALL(__NR_lgetxattr, sys_ni_syscall) // 192
__SYSCALL(__NR_fgetxattr, sys_ni_syscall) // 193
__SYSCALL(__NR_listxattr, sys_ni_syscall) // 194
__SYSCALL(__NR_llistxattr, sys_ni_syscall) // 195
__SYSCALL(__NR_flistxattr, sys_ni_syscall) // 196
__SYSCALL(__NR_removexattr, sys_ni_syscall) // 197
__SYSCALL(__NR_lremovexattr, sys_ni_syscall) // 198
__SYSCALL(__NR_fremovexattr, sys_ni_syscall) // 199
__SYSCALL(__NR_tkill, sys_ni_syscall) // 200
__SYSCALL(__NR_time, sys_ni_syscall) // 201
__SYSCALL(__NR_futex, sys_ni_syscall) // 202
__SYSCALL(__NR_sched_setaffinity, sys_ni_syscall) // 203
__SYSCALL(__NR_sched_getaffinity, sys_ni_syscall) // 204
__SYSCALL(__NR_set_thread_area, sys_ni_syscall) // 205
__SYSCALL(__NR_io_setup, sys_ni_syscall) // 206
__SYSCALL(__NR_io_destroy, sys_ni_syscall) // 207
__SYSCALL(__NR_io_getevents, sys_ni_syscall) // 208
__SYSCALL(__NR_io_submit, sys_ni_syscall) // 209
__SYSCALL(__NR_io_cancel, sys_ni_syscall) // 210
__SYSCALL(__NR_get_thread_area, sys_ni_syscall) // 211
__SYSCALL(__NR_lookup_dcookie, sys_ni_syscall) // 212
__SYSCALL(__NR_epoll_create, sys_ni_syscall) // 213
__SYSCALL(__NR_epoll_ctl_old, sys_ni_syscall) // 214
__SYSCALL(__NR_epoll_wait_old, sys_ni_syscall) // 215
__SYSCALL(__NR_remap_file_pages, sys_ni_syscall) // 216
__SYSCALL(__NR_getdents64, sys_ni_syscall) // 217
__SYSCALL(__NR_set_tid_address, sys_ni_syscall) // 218
__SYSCALL(__NR_restart_syscall, sys_ni_syscall) // 219
__SYSCALL(__NR_semtimedop, sys_ni_syscall) // 220
__SYSCALL(__NR_fadvise64, sys_ni_syscall) // 221
__SYSCALL(__NR_timer_create, sys_ni_syscall) // 222
__SYSCALL(__NR_timer_settime, sys_ni_syscall) // 223
__SYSCALL(__NR_timer_gettime, sys_ni_syscall) // 224
__SYSCALL(__NR_timer_getoverrun, sys_ni_syscall) // 225
__SYSCALL(__NR_timer_delete, sys_ni_syscall) // 226
__SYSCALL(__NR_clock_settime, sys_ni_syscall) // 227
__SYSCALL(__NR_clock_gettime, sys_ni_syscall) // 228
__SYSCALL(__NR_clock_getres, sys_ni_syscall) // 229
__SYSCALL(__NR_clock_nanosleep, sys_ni_syscall) // 230
__SYSCALL(__NR_exit_group, sys_ni_syscall) // 231
__SYSCALL(__NR_epoll_wait, sys_ni_syscall) // 232
__SYSCALL(__NR_epoll_ctl, sys_ni_syscall) // 233
__SYSCALL(__NR_tgkill, sys_ni_syscall) // 234
__SYSCALL(__NR_utimes, sys_ni_syscall) // 235
__SYSCALL(__NR_vserver, sys_ni_syscall) // 236
__SYSCALL(__NR_mbind, sys_ni_syscall) // 237
__SYSCALL(__NR_set_mempolicy, sys_ni_syscall) // 238
__SYSCALL(__NR_get_mempolicy, sys_ni_syscall) // 239
__SYSCALL(__NR_mq_open, sys_ni_syscall) // 240
__SYSCALL(__NR_mq_unlink, sys_ni_syscall) // 241
__SYSCALL(__NR_mq_timedsend, sys_ni_syscall) // 242
__SYSCALL(__NR_mq_timedreceive, sys_ni_syscall) // 243
__SYSCALL(__NR_mq_notify, sys_ni_syscall) // 244
__SYSCALL(__NR_mq_getsetattr, sys_ni_syscall) // 245
__SYSCALL(__NR_kexec_load, sys_ni_syscall) // 246
__SYSCALL(__NR_waitid, sys_ni_syscall) // 247
__SYSCALL(__NR_add_key, sys_ni_syscall) // 248
__SYSCALL(__NR_request_key, sys_ni_syscall) // 249
__SYSCALL(__NR_keyctl, sys_ni_syscall) // 250
__SYSCALL(__NR_ioprio_set, sys_ni_syscall) // 251
__SYSCALL(__NR_ioprio_get, sys_ni_syscall) // 252
__SYSCALL(__NR_inotify_init, sys_ni_syscall) // 253
__SYSCALL(__NR_inotify_add_watch, sys_ni_syscall) // 254
__SYSCALL(__NR_inotify_rm_watch, sys_ni_syscall) // 255
__SYSCALL(__NR_migrate_pages, sys_ni_syscall) // 256
__SYSCALL(__NR_openat, sys_ni_syscall) // 257
__SYSCALL(__NR_mkdirat, sys_ni_syscall) // 258
__SYSCALL(__NR_mknodat, sys_ni_syscall) // 259
__SYSCALL(__NR_fchownat, sys_ni_syscall) // 260
__SYSCALL(__NR_futimesat, sys_ni_syscall) // 261
__SYSCALL(__NR_newfstatat, sys_ni_syscall) // 262
__SYSCALL(__NR_unlinkat, sys_ni_syscall) // 263
__SYSCALL(__NR_renameat, sys_ni_syscall) // 264
__SYSCALL(__NR_linkat, sys_ni_syscall) // 265
__SYSCALL(__NR_symlinkat, sys_ni_syscall) // 266
__SYSCALL(__NR_readlinkat, sys_ni_syscall) // 267
__SYSCALL(__NR_fchmodat, sys_ni_syscall) // 268
__SYSCALL(__NR_faccessat, sys_ni_syscall) // 269
__SYSCALL(__NR_pselect6, sys_ni_syscall) // 270
__SYSCALL(__NR_ppoll, sys_ni_syscall) // 271
__SYSCALL(__NR_unshare, sys_ni_syscall) // 272
__SYSCALL(__NR_set_robust_list, sys_ni_syscall) // 273
__SYSCALL(__NR_get_robust_list, sys_ni_syscall) // 274
__SYSCALL(__NR_splice, sys_ni_syscall) // 275
__SYSCALL(__NR_tee, sys_ni_syscall) // 276
__SYSCALL(__NR_sync_file_range, sys_ni_syscall) // 277
__SYSCALL(__NR_vmsplice, sys_ni_syscall) // 278
__SYSCALL(__NR_move_pages, sys_ni_syscall) // 279
__SYSCALL(__NR_utimensat, sys_ni_syscall) // 280
__SYSCALL(__NR_epoll_pwait, sys_ni_syscall) // 281
__SYSCALL(__NR_signalfd, sys_ni_syscall) // 282
__SYSCALL(__NR_timerfd_create, sys_ni_syscall) // 283
__SYSCALL(__NR_eventfd, sys_ni_syscall) // 284
__SYSCALL(__NR_fallocate, sys_ni_syscall) // 285
__SYSCALL(__NR_timerfd_settime, sys_ni_syscall) // 286
__SYSCALL(__NR_timerfd_gettime, sys_ni_syscall) // 287
__SYSCALL(__NR_accept4, sys_ni_syscall) // 288
__SYSCALL(__NR_signalfd4, sys_ni_syscall) // 289
__SYSCALL(__NR_eventfd2, sys_ni_syscall) // 290
__SYSCALL(__NR_epoll_create1, sys_ni_syscall) // 291
__SYSCALL(__NR_dup3, sys_ni_syscall) // 292
__SYSCALL(__NR_pipe2, sys_ni_syscall) // 293
__SYSCALL(__NR_inotify_init1, sys_ni_syscall) // 294
__SYSCALL(__NR_preadv, sys_ni_syscall) // 295
__SYSCALL(__NR_pwritev, sys_ni_syscall) // 296
__SYSCALL(__NR_rt_tgsigqueueinfo, sys_ni_syscall) // 297
__SYSCALL(__NR_perf_event_open, sys_ni_syscall) // 298
__SYSCALL(__NR_recvmmsg, sys_ni_syscall) // 299
__SYSCALL(__NR_fanotify_init, sys_ni_syscall) // 300
__SYSCALL(__NR_fanotify_mark, sys_ni_syscall) // 301
__SYSCALL(__NR_prlimit64, sys_ni_syscall) // 302
__SYSCALL(__NR_name_to_handle_at, sys_ni_syscall) // 303
__SYSCALL(__NR_open_by_handle_at, sys_ni_syscall) // 304
__SYSCALL(__NR_clock_adjtime, sys_ni_syscall) // 305
__SYSCALL(__NR_syncfs, sys_ni_syscall) // 306
__SYSCALL(__NR_sendmmsg, sys_ni_syscall) // 307
__SYSCALL(__NR_setns, sys_ni_syscall) // 308
__SYSCALL(__NR_getcpu, sys_ni_syscall) // 309
__SYSCALL(__NR_process_vm_readv, sys_ni_syscall) // 310
__SYSCALL(__NR_process_vm_writev, sys_ni_syscall) // 311
__SYSCALL(__NR_kcmp, sys_ni_syscall) // 312
__SYSCALL(__NR_finit_module, sys_ni_syscall) // 313
__SYSCALL(__NR_sched_setattr, sys_ni_syscall) // 314
__SYSCALL(__NR_sched_getattr, sys_ni_syscall) // 315
__SYSCALL(__NR_renameat2, sys_ni_syscall) // 316
__SYSCALL(__NR_seccomp, sys_ni_syscall) // 317
__SYSCALL(__NR_getrandom, sys_ni_syscall) // 318
__SYSCALL(__NR_memfd_create, sys_ni_syscall) // 319
__SYSCALL(__NR_kexec_file_load, sys_ni_syscall) // 320
__SYSCALL(__NR_bpf, sys_ni_syscall) // 321
__SYSCALL(__NR_execveat, sys_ni_syscall) // 322
__SYSCALL(__NR_userfaultfd, sys_ni_syscall) // 323
__SYSCALL(__NR_membarrier, sys_ni_syscall) // 324
__SYSCALL(__NR_mlock2, sys_ni_syscall) // 325
__SYSCALL(__NR_copy_file_range, sys_ni_syscall) // 326
__SYSCALL(__NR_preadv2, sys_ni_syscall) // 327
__SYSCALL(__NR_pwritev2, sys_ni_syscall) // 328
__SYSCALL(__NR_pkey_mprotect, sys_ni_syscall) // 329
__SYSCALL(__NR_pkey_alloc, sys_ni_syscall) // 330
__SYSCALL(__NR_pkey_free, sys_ni_syscall) // 331
__SYSCALL(__NR_statx, sys_ni_syscall) // 332
__SYSCALL(__NR_io_pgetevents, sys_ni_syscall) // 333
__SYSCALL(__NR_rseq, sys_ni_syscall) // 334
__SYSCALL(335, sys_ni_syscall) // 335
__SYSCALL(336, sys_ni_syscall) // 336
__SYSCALL(337, sys_ni_syscall) // 337
__SYSCALL(338, sys_ni_syscall) // 338
__SYSCALL(339, sys_ni_syscall) // 339
__SYSCALL(340, sys_ni_syscall) // 340
__SYSCALL(341, sys_ni_syscall) // 341
__SYSCALL(342, sys_ni_syscall) // 342
__SYSCALL(343, sys_ni_syscall) // 343
__SYSCALL(344, sys_ni_syscall) // 344
__SYSCALL(345, sys_ni_syscall) // 345
__SYSCALL(346, sys_ni_syscall) // 346
__SYSCALL(347, sys_ni_syscall) // 347
__SYSCALL(348, sys_ni_syscall) // 348
__SYSCALL(349, sys_ni_syscall) // 349
__SYSCALL(350, sys_ni_syscall) // 350
__SYSCALL(351, sys_ni_syscall) // 351
__SYSCALL(352, sys_ni_syscall) // 352
__SYSCALL(353, sys_ni_syscall) // 353
__SYSCALL(354, sys_ni_syscall) // 354
__SYSCALL(355, sys_ni_syscall) // 355
__SYSCALL(356, sys_ni_syscall) // 356
__SYSCALL(357, sys_ni_syscall) // 357
__SYSCALL(358, sys_ni_syscall) // 358
__SYSCALL(359, sys_ni_syscall) // 359
__SYSCALL(360, sys_ni_syscall) // 360
__SYSCALL(361, sys_ni_syscall) // 361
__SYSCALL(362, sys_ni_syscall) // 362
__SYSCALL(363, sys_ni_syscall) // 363
__SYSCALL(364, sys_ni_syscall) // 364
__SYSCALL(365, sys_ni_syscall) // 365
__SYSCALL(366, sys_ni_syscall) // 366
__SYSCALL(367, sys_ni_syscall) // 367
__SYSCALL(368, sys_ni_syscall) // 368
__SYSCALL(369, sys_ni_syscall) // 369
__SYSCALL(370, sys_ni_syscall) // 370
__SYSCALL(371, sys_ni_syscall) // 371
__SYSCALL(372, sys_ni_syscall) // 372
__SYSCALL(373, sys_ni_syscall) // 373
__SYSCALL(374, sys_ni_syscall) // 374
__SYSCALL(375, sys_ni_syscall) // 375
__SYSCALL(376, sys_ni_syscall) // 376
__SYSCALL(377, sys_ni_syscall) // 377
__SYSCALL(378, sys_ni_syscall) // 378
__SYSCALL(379, sys_ni_syscall) // 379
__SYSCALL(380, sys_ni_syscall) // 380
__SYSCALL(381, sys_ni_syscall) // 381
__SYSCALL(382, sys_ni_syscall) // 382
__SYSCALL(383, sys_ni_syscall) // 383
__SYSCALL(384, sys_ni_syscall) // 384
__SYSCALL(385, sys_ni_syscall) // 385
__SYSCALL(386, sys_ni_syscall) // 386
__SYSCALL(387, sys_ni_syscall) // 387
__SYSCALL(388, sys_ni_syscall) // 388
__SYSCALL(389, sys_ni_syscall) // 389
__SYSCALL(390, sys_ni_syscall) // 390
__SYSCALL(391, sys_ni_syscall) // 391
__SYSCALL(392, sys_ni_syscall) // 392
__SYSCALL(393, sys_ni_syscall) // 393
__SYSCALL(394, sys_ni_syscall) // 394
__SYSCALL(395, sys_ni_syscall) // 395
__SYSCALL(396, sys_ni_syscall) // 396
__SYSCALL(397, sys_ni_syscall) // 397
__SYSCALL(398, sys_ni_syscall) // 398
__SYSCALL(399, sys_ni_syscall) // 399
__SYSCALL(400, sys_ni_syscall) // 400
__SYSCALL(401, sys_ni_syscall) // 401
__SYSCALL(402, sys_ni_syscall) // 402
__SYSCALL(403, sys_ni_syscall) // 403
__SYSCALL(404, sys_ni_syscall) // 404
__SYSCALL(405, sys_ni_syscall) // 405
__SYSCALL(406, sys_ni_syscall) // 406
__SYSCALL(407, sys_ni_syscall) // 407
__SYSCALL(408, sys_ni_syscall) // 408
__SYSCALL(409, sys_ni_syscall) // 409
__SYSCALL(410, sys_ni_syscall) // 410
__SYSCALL(411, sys_ni_syscall) // 411
__SYSCALL(412, sys_ni_syscall) // 412
__SYSCALL(413, sys_ni_syscall) // 413
__SYSCALL(414, sys_ni_syscall) // 414
__SYSCALL(415, sys_ni_syscall) // 415
__SYSCALL(416, sys_ni_syscall) // 416
__SYSCALL(417, sys_ni_syscall) // 417
__SYSCALL(418, sys_ni_syscall) // 418
__SYSCALL(419, sys_ni_syscall) // 419
__SYSCALL(420, sys_ni_syscall) // 420
__SYSCALL(421, sys_ni_syscall) // 421
__SYSCALL(422, sys_ni_syscall) // 422
__SYSCALL(423, sys_ni_syscall) // 423
__SYSCALL(__NR_pidfd_send_signal, sys_ni_syscall) // 424
__SYSCALL(__NR_io_uring_setup, sys_ni_syscall) // 425
__SYSCALL(__NR_io_uring_enter, sys_ni_syscall) // 426
__SYSCALL(__NR_io_uring_register, sys_ni_syscall) // 427
__SYSCALL(__NR_open_tree, sys_ni_syscall) // 428
__SYSCALL(__NR_move_mount, sys_ni_syscall) // 429
__SYSCALL(__NR_fsopen, sys_ni_syscall) // 430
__SYSCALL(__NR_fsconfig, sys_ni_syscall) // 431
__SYSCALL(__NR_fsmount, sys_ni_syscall) // 432
__SYSCALL(__NR_fspick, sys_ni_syscall) // 433
__SYSCALL(__NR_pidfd_open, sys_ni_syscall) // 434
__SYSCALL(__NR_clone3, sys_ni_syscall) // 435
__SYSCALL(__NR_close_range, sys_ni_syscall) // 436
__SYSCALL(__NR_openat2, sys_ni_syscall) // 437
__SYSCALL(__NR_pidfd_getfd, sys_ni_syscall) // 438
__SYSCALL(__NR_faccessat2, sys_ni_syscall) // 439
__SYSCALL(__NR_process_madvise, sys_ni_syscall) // 440
__SYSCALL(__NR_epoll_pwait2, sys_ni_syscall) // 441
__SYSCALL(__NR_mount_setattr, sys_ni_syscall) // 442
