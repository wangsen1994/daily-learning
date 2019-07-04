### linux系统调用open

函数原型，其实现为调用系统函数`sys_open`由于各种VFS实现的不同，系统函数实质是对这些调用进行了包装。

```c
static inline long open(const char * name, int mode, int flags)
{
         return sys_open(name, mode, flags);
 }
```

```c
asmlinkage long sys_open(const char * filename, int flags, int mode)
 {
         char * tmp;
         int fd, error;
 
 #if BITS_PER_LONG != 32
         flags |= O_LARGEFILE;
 #endif
         tmp = getname(filename);   /* 1 将filename从用户态拷贝到内核态 */
         fd = PTR_ERR(tmp);
         if (!IS_ERR(tmp)) {
                 fd = get_unused_fd();  /* 2 申请一个可用的文件描述符 */
                 if (fd >= 0) {
                         struct file *f = filp_open(tmp, flags, mode); /* 3 打开制定的文件 */
                         error = PTR_ERR(f);
                         if (IS_ERR(f))
                                goto out_error;
                         fd_install(fd, f);    /* 4 将打开的文件与文件描述符关联 */
                 }
 out:
                 putname(tmp);
         }
         return fd;
 
 out_error:
         put_unused_fd(fd);
        fd = error;
        goto out;
}

```

```c
/* 第一步 */
char * getname(const char * filename)
 {
         char *tmp, *result;
 
         result = ERR_PTR(-ENOMEM);
         tmp = __getname();     /* 分配内核空间 在内核的slab空间分配*/
         /* #define __getname()     kmem_cache_alloc(names_cachep, SLAB_KERNEL)*/
         if (tmp)  {
                int retval = do_getname(filename, tmp);  /* 其实就是将filename拷贝到tmp中 */
 
                 result = tmp;
                 if (retval < 0) {
                         putname(tmp);
                         result = ERR_PTR(retval);
                 }
         }
         return result;
 }

static inline int do_getname(const char *filename, char *page)
 {
         int retval;
         unsigned long len = PATH_MAX;
 
        if ((unsigned long) filename >= TASK_SIZE) {
                 if (!segment_eq(get_fs(), KERNEL_DS))
                         return -EFAULT;
        } else if (TASK_SIZE - (unsigned long) filename < PATH_MAX)
                 len = TASK_SIZE - (unsigned long) filename;
 
         retval = strncpy_from_user((char *)page, filename, len);/* 核心的一个步骤，其实就是将filename拷贝到刚刚在内核中分配的空间中 */
         if (retval > 0) {
                 if (retval < len)
                         return 0;
                 return -ENAMETOOLONG;
         } else if (!retval)
                 retval = -ENOENT;
         return retval;
 }
```

```c
/* 第三步 */
struct nameidata {
         struct dentry *dentry;   /* 当前目录项对象 */
         struct vfsmount *mnt;    /* 已安装的文件系统挂载点 */
         struct qstr last;        /* 路径名称最后一部分 */
         unsigned int flags;      /* 查询标识 */
         int last_type;           /* 路径名称最后一部分类型 */
 };
struct file *filp_open(const char * filename, int flags, int mode)
 {
         int namei_flags, error;
         struct nameidata nd;
 
         namei_flags = flags;
         if ((namei_flags+1) & O_ACCMODE)
                 namei_flags++;
         if (namei_flags & O_TRUNC)
                 namei_flags |= 2;
         /* 根据文件名打开文件 */
         error = open_namei(filename, namei_flags, mode, &nd);
         if (!error)   /* 下面打开这个文件，这个函数返回的是file结构体指针！！！ */
                 return dentry_open(nd.dentry, nd.mnt, flags);
 
         return ERR_PTR(error);
 }

struct file *dentry_open(struct dentry *dentry, struct vfsmount *mnt, int flags)
 {
         struct file * f;
         struct inode *inode;
         static LIST_HEAD(kill_list);
         int error;
 
         error = -ENFILE;
         f = get_empty_filp();    /* 得到一个空的file结构体，如果出错或者内存不足，返回1error */
         if (!f)
                goto cleanup_dentry;
         f->f_flags = flags;   /* 一些赋值操作 */
         f->f_mode = (flags+1) & O_ACCMODE;
         inode = dentry->d_inode; /* 获得文件inode */
         if (f->f_mode & FMODE_WRITE) {
                 error = get_write_access(inode);
                 if (error)
                         goto cleanup_file;
         }
         /* 一些赋值操作 */
         f->f_dentry = dentry; /* 目录项 */
         f->f_vfsmnt = mnt;    /* 挂载点 */
         f->f_pos = 0;         /* 文件相对开头偏移 */
         f->f_reada = 0;       /* 预读标志 */
         f->f_op = fops_get(inode->i_fop);   /* 文件操作函数 */
  			 file_move(f, &inode->i_sb->s_files);/* 将新建的file链接进入inode对应的超级块的file链表中 */ 
         /* preallocate kiobuf for O_DIRECT */
         f->f_iobuf = NULL;
         f->f_iobuf_lock = 0;
         if (f->f_flags & O_DIRECT) {
                 error = alloc_kiovec(1, &f->f_iobuf); /* 分配io buffer */
                 if (error)
                         goto cleanup_all;
         }
 <span style="white-space:pre">	</span>    /* 下面尝试打开文件，保证能够正常打开这个文件 */
         if (f->f_op && f->f_op->open) { 
                error = f->f_op->open(inode,f);
                 if (error)
                         goto cleanup_all;
         }
         f->f_flags &= ~(O_CREAT | O_EXCL | O_NOCTTY | O_TRUNC);
 
         return f;  /* 返回创建好的file */
         /* 下面都是出错处理 */
 cleanup_all:
         if (f->f_iobuf)
                 free_kiovec(1, &f->f_iobuf);
         fops_put(f->f_op);
         if (f->f_mode & FMODE_WRITE)
                 put_write_access(inode);
         file_move(f, &kill_list); /* out of the way.. */
         f->f_dentry = NULL;
         f->f_vfsmnt = NULL;
 cleanup_file:
         put_filp(f);
 cleanup_dentry:
         dput(dentry);
         mntput(mnt);
         return ERR_PTR(error);
 }
```

```c
/* 第四步 */
 struct file * get_empty_filp(void)
  {
         static int old_max = 0;
         struct file * f;
 
         file_list_lock();
         if (files_stat.nr_free_files > NR_RESERVED_FILES) {  /* 如果允许打开的数量已经超过系统允许的 */
         used_one:
                 f = list_entry(free_list.next, struct file, f_list); /* 在free_list中删除一个，留下了给新的file使用 */
                 list_del(&f->f_list);
                 files_stat.nr_free_files--;
         new_one: /* 下面创建一个新的file结构体 */
                 memset(f, 0, sizeof(*f));
                 atomic_set(&f->f_count,1);
                 f->f_version = ++event;
                 f->f_uid = current->fsuid;
                 f->f_gid = current->fsgid;
                f->f_maxcount = INT_MAX;
                 list_add(&f->f_list, &anon_list);
                 file_list_unlock();
                 return f;   /* 返回file */
        }
         /*
          * Use a reserved one if we're the superuser
          */
         if (files_stat.nr_free_files && !current->euid)
                 goto used_one;
         /*
         * Allocate a new one if we're below the limit.  如果还可以创建file结构体，那么创建一个新的就OK
          */
         if (files_stat.nr_files < files_stat.max_files) {
                 file_list_unlock();
                f = kmem_cache_alloc(filp_cachep, SLAB_KERNEL);  /* 在slab中分配一个新的file缓存 */
                 file_list_lock();
                 if (f) {
                         files_stat.nr_files++; /* 数量++ */
                         goto new_one;          /* 初始化这个新的值 */
                 }
                 /* Big problems... */
                 printk(KERN_WARNING "VFS: filp allocation failed\n");
  
         } else if (files_stat.max_files > old_max) {
                 printk(KERN_INFO "VFS: file-max limit %d reached\n", files_stat.max_files);
                 old_max = files_stat.max_files;
         }
         file_list_unlock();
         return NULL;
 }

void fd_install(unsigned int fd, struct file * file)
 {
         struct files_struct *files = current->files;  /* 获得当前进程文件打开表 */
           
         write_lock(&files->file_lock);
         if (files->fd[fd])   /* 如果这个fd下已经存在文件了，那么error！ */
                BUG(); 
         files->fd[fd] = file;/* 关联这个fd和新打开的文件 */
         write_unlock(&files->file_lock);
 }
```

