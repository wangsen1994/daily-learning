### Redis内存管理

文件`zmalloc.c`主要包含`zmalloc`、`zfree`、`zcalloc` 、`zrelloc`、`zstrdup`，内存的管理需要考虑字节对齐，有助于提高CPU的运行速度。

- `zmalloc`函数，辅助数：`malloc`、`zmalloc_oom_handler`、`	malloc_default_oom`、`update_zmalloc_stat_alloc`宏函数、`update_zmalloc_stat_add`宏函数

  ```c
  void *zmalloc(size_t size) {
      void *ptr = malloc(size+PREFIX_SIZE);
   
      if (!ptr) zmalloc_oom_handler(size);
  #ifdef HAVE_MALLOC_SIZE
      update_zmalloc_stat_alloc(zmalloc_size(ptr));
      return ptr;
  #else
      *((size_t*)ptr) = size;  /* 头8个字节用于存储size */
    
    	/* 用于更新全局变量used_memory以分配内存的值*/
      update_zmalloc_stat_alloc(size+PREFIX_SIZE); 
    	/* 更新返回的内存地址 */
      return (char*)ptr+PREFIX_SIZE;
  #endif
  }
  ```

  size为需要分配的大小，PREFIX_SIZE是一个条件编译的宏，不同平台有不同的结果，在linux中其值为sizeof(size_t)，多分配8个字节的目的在于存储size的值。若分配失败，则调用`zmalloc_oom_handler`打印错误信息

  ``` c
  static void zmalloc_default_oom(size_t size) {
      fprintf(stderr, "zmalloc: Out of memory trying to allocate %zu bytes\n",
          size);
      fflush(stderr);
      abort();
  }
  ```

  `update_zmalloc_stat_alloc` 源码（条件宏编译定义）

  ```c
  #define update_zmalloc_stat_alloc(__n) 
  do { 
      size_t _n = (__n); 
    	/* 判断申请的size是否为sizeof（long）的整数倍，否则补齐；由于malloc能够保证倍数问题，其实质更新				used_memory其为全局变量统计信息*/
      if (_n&(sizeof(long)-1)) _n += sizeof(long)-(_n&(sizeof(long)-1)); 
      if (zmalloc_thread_safe) { 
          update_zmalloc_stat_add(_n); 
      } else { 
          used_memory += _n; 
      } 
  } while(0)
  ```

  `update_zmalloc_stat_add` 源码（条件宏编译定义）

  ```c
  #define update_zmalloc_stat_add(__n) 
  do { 
      pthread_mutex_lock(&used_memory_mutex); /* 对应于多线程 */
      used_memory += (__n); 
      pthread_mutex_unlock(&used_memory_mutex); 
  } while(0)
  ```



- `zfree`函数，辅助函数：`free`、`update_zmalloc_free`宏函数、`update_zmalloc_sub`宏函数、`zmalloc_size`

  `zfree`源码

  ```c
  void zfree(void *ptr) {
  #ifndef HAVE_MALLOC_SIZE
      void *realptr;
      size_t oldsize;
  #endif
   
      if (ptr == NULL) return;
  #ifdef HAVE_MALLOC_SIZE
      update_zmalloc_stat_free(zmalloc_size(ptr));
      free(ptr);
  #else
      realptr = (char*)ptr-PREFIX_SIZE; /* 向前偏移PREFIX_SIZE单位 */
      oldsize = *((size_t*)realptr);    /* 获得原始size大小 */
      update_zmalloc_stat_free(oldsize+PREFIX_SIZE); /* 更新used_memory统计信息 */
      free(realptr);  /* 回收地址 */
  #endif
  }
  ```

  `update_zmalloc_free` 源码（条件宏编译定义）

  ```c
  #define update_zmalloc_stat_free(__n) do { 
      size_t _n = (__n); 
      if (_n&(sizeof(long)-1)) _n += sizeof(long)-(_n&(sizeof(long)-1)); 
      if (zmalloc_thread_safe) { 
          update_zmalloc_stat_sub(_n); 
      } else { 
          used_memory -= _n; 
      } 
  } while(0)
    
  #define update_zmalloc_stat_sub(__n) do { 
      pthread_mutex_lock(&used_memory_mutex); 
      used_memory -= (__n); 
      pthread_mutex_unlock(&used_memory_mutex); 
  } while(0)
  ```

  

- `zcalloc`基于`calloc`函数实现，但接口不一样

  `zcalloc`源码

  ```c
  void *calloc(size_t nmemb, size_t size); /* 申请 nmemb*size 空间，并将地址全部初始化为0 */
  void *zcalloc(size_t size); /* 其实现基本与zmalloc基本一致，除了进行了初始化 */
  
  void *zcalloc(size_t size) {
      void *ptr = calloc(1, size+PREFIX_SIZE);
   
      if (!ptr) zmalloc_oom_handler(size);
  #ifdef HAVE_MALLOC_SIZE
      update_zmalloc_stat_alloc(zmalloc_size(ptr));
      return ptr;
  #else
      *((size_t*)ptr) = size;
      update_zmalloc_stat_alloc(size+PREFIX_SIZE);
      return (char*)ptr+PREFIX_SIZE;
  #endif
  }
  ```

  

- `zrealloc`函数，其主要为给定首地址的内存空间，重新分配大小，如果失败，则在其他位置新建一块大小为size字节的空间，将原来的数据复制到新的内存空间，并返回该段地址内存的首地址（原内存会被系统自动释放）（在上网看到的源码分析没有发现，如果内存空间不够时，上述的处理）。

  `zrealloc`源码

  ``` c
  void *realloc (void *ptr, size_t size);
  void *zrealloc(void *ptr, size_t size);
  
  void *zrealloc(void *ptr, size_t size) {
  #ifndef HAVE_MALLOC_SIZE
      void *realptr;
  #endif
      size_t oldsize;
      void *newptr;
   
      if (ptr == NULL) return zmalloc(size);
  #ifdef HAVE_MALLOC_SIZE
      oldsize = zmalloc_size(ptr);
      newptr = realloc(ptr,size);  /* c函数库 */
      if (!newptr) zmalloc_oom_handler(size);
   
      update_zmalloc_stat_free(oldsize);   /* 宏函数 */
      update_zmalloc_stat_alloc(zmalloc_size(newptr)); /* 宏函数 */
      return newptr;
  #else
      realptr = (char*)ptr-PREFIX_SIZE;
      oldsize = *((size_t*)realptr);
      newptr = realloc(realptr,size+PREFIX_SIZE);
      if (!newptr) zmalloc_oom_handler(size);
   
      *((size_t*)newptr) = size;
      update_zmalloc_stat_free(oldsize); /* 先减去原来内存 */
      update_zmalloc_stat_alloc(size);   /* 再加上现在分配 */
      return (char*)newptr+PREFIX_SIZE;
  #endif
  }
  /* 其释放内存函数调用zfree */
  ```

  

- `zstrdup`函数，字符串复制

  ``` c
  char *zstrdup(const char *s) {
      size_t l = strlen(s)+1; 
      char *p = zmalloc(l); // 开辟一段新内存
  
      memcpy(p,s,l); // 调用字符串复制函数
      return p;
  }
  ```

  

redis内存分配的函数基本都是基于系统提供的内存分配函数，但其进行了内存块大小与内存使用的统计。