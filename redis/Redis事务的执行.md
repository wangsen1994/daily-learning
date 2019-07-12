### Redis事务的执行

redis事务机制将多个命令当作一个独立的单元运行，主要包括multi、exec、watch、unwatch、discard五个相关命令。

其主要包括三个阶段 1、声明事务（multi）2、命令入队 3、执行命令（exec）

redis也是通过状态机的方式对事务进行管理``REDIS_MULTI``表示事务状态、``REDIS_DIRTY_EXEC``无效状态，命令入队时出现错误、``REDIS_DIRTY_CAS``非安全状态，监视的key被修改。

####  Redis事务大致流程：

- 客户端redisClient中有一个名叫flags的成员，标识当前客户端的状态。
- 在声明事务之前，我们可以通过watch命令对一个或多个key进行监视。如果在事务执行之前这些被监视的key被其他命令修改，Redis将redisClient->flags设置为REDIS_DIRTY_CAS标识。
- 使用multi命令可以标识着一个事务的开始，此时redisClient进入事务状态，其flags字段被设置为REDIS_MULTI标识。
- 当客户端进入事务状态后，Redis服务器等待接收一个或多个命令，并把它们放入命令队列中等待执行。如果某条命令在入队过程中发生错误，Redis会将redisClient的flags字段置为REDIS_DIRTY_EXEC标识。
- 最后我们通过exec命令执行事务，该命令将会检查redisClient的flags标识，如果该标识为REDIS_DIRTY_CAS或REDIS_DIRTY_EXEC，则事务执行失败，否则Redis一次性执行事务中的多个命令，并将所有命令的结果集合到回复队列，再作为 exec 命令的结果返回给客户端。

#### watch实现

其实现主要包括两个数据结构：

- 每个redisDb数据库使用一个哈希表来维护key和所有监控该key的客户端列表的映射关系。这样当一个key被修改后，我们就可以对所有监控该key的客户端设置dirty标识。

  ```c
  /* Redis数据库结构体 */
  typedef struct redisDb {
      ...
      // 被watch命令监控的键和相应client
      dict *watched_keys; /* WATCHED keys for MULTI/EXEC CAS */
      int id;
      ...
  } redisDb;
  ```

  watched_keys为字典（哈希表）结构，键为被监视的key，值为所有监控该key的客户端列表（即list数据结构）。

- 客户端redisClient维护着一个保存所有被监控的key的列表，这样就可以方便地对key取消监视。这个列表就是redisClient中的watched_keys成员，该成员是一个双向链表list结构。

  ```c
  typedef struct watchedKey {
      // 被监控的key
      robj *key;
      // key所在的数据库
      redisDb *db;
  } watchedKey;
  ```

  redisDb-> watched_keys和 redisClient->watched_keys两者的结合就实现了watch/unwatch命令所需要的功能：通过redisDb-> watched_keys 哈希表， 如果某个程序需要检查某个键是否被监视， 那么它只要检查字典中是否存在这个键即可； 通过redisClient->watched_keys，如果某个程序要获得该客户端监视的所有key，那么它只要获得该链表即可.

```c
void watchForKey(redisClient *c, robj *key) {
    list *clients = NULL;
    listIter li;
    listNode *ln;
    watchedKey *wk;

    /* Check if we are already watching for this key */
    // 检查该key是否已经保存在client->watched_keys列表中

    // listRewind获取list的迭代器
    listRewind(c->watched_keys,&li);
    // 遍历查找，如果发现给定key已经存在直接返回
    while((ln = listNext(&li))) {
        wk = listNodeValue(ln);
        if (wk->db == c->db && equalStringObjects(key,wk->key))
            return; /* Key already watched */
    }

    /* This key is not already watched in this DB. Let's add it */
    // 检查redisDB->watched_keys是否保存了该key和客户端的映射关系，如果没有则添加之
    // 获取监控给定key的客户端列表
    clients = dictFetchValue(c->db->watched_keys,key);
    // 如果该列表为空，则创建一个
    if (!clients) {
        clients = listCreate();
        dictAdd(c->db->watched_keys,key,clients);
        incrRefCount(key);
    }
    // 并加入当前客户端
    listAddNodeTail(clients,c);

    /* Add the new key to the list of keys watched by this client */
    // 将一个新的watchedKey结构添加到client->watched_keys列表中
    wk = zmalloc(sizeof(*wk));
    wk->key = key;
    wk->db = c->db;
    incrRefCount(key);
    listAddNodeTail(c->watched_keys,wk);
}

void unwatchAllKeys(redisClient *c) {
    listIter li;
    listNode *ln;

    // 如果没有key被监控，直接返回
    if (listLength(c->watched_keys) == 0) return;
    // 获得c->watched_keys列表的迭代器
    listRewind(c->watched_keys,&li);
    // 遍历c->watched_keys列表，逐一删除被该客户端监视的key
    while((ln = listNext(&li))) {
        list *clients;
        watchedKey *wk;

        /* Lookup the watched key -> clients list and remove the client
         * from the list */
        wk = listNodeValue(ln);
        // 将当前客户端从db->watched_keys中删除
        clients = dictFetchValue(wk->db->watched_keys, wk->key);
        redisAssertWithInfo(c,NULL,clients != NULL);
        listDelNode(clients,listSearchKey(clients,c));

        /* Kill the entry at all if this was the only client */
        // 如果没有任何客户端监控该key，则将该key从db->watched_keys中删除
        if (listLength(clients) == 0)
            dictDelete(wk->db->watched_keys, wk->key);

        /* Remove this watched key from the client->watched list */
        // 将c->watched_keys删除该key
        listDelNode(c->watched_keys,ln);

        // 释放资源
        decrRefCount(wk->key);
        zfree(wk);
    }
}
```

#### multi/exec命令实现

#### 声明事务

Redis不支持嵌套事务，声明事务即是简单地将flags设置为REDIS_MULTI标识。随后redisClient进入事务状态，等待命令入队。

```c
/* 执行MULTI命令 */
void multiCommand(redisClient *c) {
    // 不支持嵌套事务，否则直接报错
    if (c->flags & REDIS_MULTI) {
        addReplyError(c,"MULTI calls can not be nested");
        return;
    }
    // 设置事务标识
    c->flags |= REDIS_MULTI;
    addReply(c,shared.ok);
}
```

#### 命令入队

在redisClient中存在multiState mstate字段用于保存一个事务

```c
/* 事务状态结构体 */
typedef struct multiState {
    // 命令数组，保存着该事务中的所有命令并按输入顺序排列
    multiCmd *commands;     /* Array of MULTI commands */
    // 命令数组长度，即命令的数量
    int count;              /* Total number of MULTI commands */
    int minreplicas;        /* MINREPLICAS for synchronous replication */
    time_t minreplicas_timeout; /* MINREPLICAS timeout as unixtime. */
} multiState;
```

multiState结构中的`multiCmd *commands`存放命令的命令队列，其实质是一个数组。multiCmd表示一条完整的输入命令，包含“要执行的命令”、“命令参数”、“参数个数”三个属性。定义如下：

```c
/* 事务命令结构体 */
typedef struct multiCmd {
    // 命令参数
    robj **argv;
    // 参数个数
    int argc;
    // 要执行的命令
    struct redisCommand *cmd;
} multiCmd;
```

命令入队函数：

```c
/* 将一个新命令添加到multi命令队列中 */
void queueMultiCommand(redisClient *c) {
    multiCmd *mc;
    int j;

    // 在原commands后面配置空间以存放新命令
    c->mstate.commands = zrealloc(c->mstate.commands,
            sizeof(multiCmd)*(c->mstate.count+1));
    // 执行新配置的空间
    mc = c->mstate.commands+c->mstate.count;
    // 设置各个属性（命令、命令参数个数以及具体的命令参数）
    mc->cmd = c->cmd;
    mc->argc = c->argc;
    // 分配空间以存放命令参数
    mc->argv = zmalloc(sizeof(robj*)*c->argc);
    memcpy(mc->argv,c->argv,sizeof(robj*)*c->argc);
    for (j = 0; j < c->argc; j++)
        incrRefCount(mc->argv[j]);
    // 命令队列中保存的命令个数加1
    c->mstate.count++;
}
```

#### 执行事务

Redis通过exec命令执行事务，该命令将会检查redisClient的flags标识，如果该标识为REDIS_DIRTY_CAS或REDIS_DIRTY_EXEC，则事务执行失败返回。如果客户端仍然处于事务状态， 那么当 exec 命令执行时，Redis会根据客户端所保存的事务队列， 以“先近先出”的策略执行事务队列中的命令，即最先入队的命令最先执行， 而最后入队的命令最后执行。

```c
/* 执行exec命令 */
void execCommand(redisClient *c) {
    int j;
    robj **orig_argv;
    int orig_argc;
    struct redisCommand *orig_cmd;
    // 是否需要将MULTI/EXEC命令传播到slave节点/AOF
    int must_propagate = 0; /* Need to propagate MULTI/EXEC to AOF / slaves? */

    // 如果客户端当前不处于事务状态，直接返回
    if (!(c->flags & REDIS_MULTI)) {
        addReplyError(c,"EXEC without MULTI");
        return;
    }

    /* Check if we need to abort the EXEC because:
     * 1) Some WATCHed key was touched.
     * 2) There was a previous error while queueing commands.
     * A failed EXEC in the first case returns a multi bulk nil object
     * (technically it is not an error but a special behavior), while
     * in the second an EXECABORT error is returned. */
    // 检查是否需要中断事务执行，因为：
    // （1）、有被监控的key被修改
    // （2）、命令入队的时候发生错误
    //  对于第一种情况，Redis返回多个nil空对象（准确地说这种情况并不是错误，应视为一种特殊的行为）
    //  对于第二种情况则返回一个EXECABORT错误
    if (c->flags & (REDIS_DIRTY_CAS|REDIS_DIRTY_EXEC)) {
        addReply(c, c->flags & REDIS_DIRTY_EXEC ? shared.execaborterr :
                                                  shared.nullmultibulk);
        // 取消事务
        discardTransaction(c);
        goto handle_monitor;
    }

    /* Exec all the queued commands */
    // 现在可以执行该事务的所有命令了

    // 取消对所有key的监控，否则会浪费CPU资源
    unwatchAllKeys(c); /* Unwatch ASAP otherwise we'll waste CPU cycles */
    // 先备份一次命令队列中的命令
    orig_argv = c->argv;
    orig_argc = c->argc;
    orig_cmd = c->cmd;
    addReplyMultiBulkLen(c,c->mstate.count);
    // 逐一将事务中的命令交给客户端redisClient执行
    for (j = 0; j < c->mstate.count; j++) {
        // 将事务命令队列中的命令设置给客户端
        c->argc = c->mstate.commands[j].argc;
        c->argv = c->mstate.commands[j].argv;
        c->cmd = c->mstate.commands[j].cmd;

        /* Propagate a MULTI request once we encounter the first write op.
         * This way we'll deliver the MULTI/..../EXEC block as a whole and
         * both the AOF and the replication link will have the same consistency
         * and atomicity guarantees. */
        //  当我们第一次遇到写命令时，传播MULTI命令。如果是读命令则无需传播
        //  这里我们MULTI/..../EXEC当做一个整体传输，保证服务器和AOF以及附属节点的一致性
        if (!must_propagate && !(c->cmd->flags & REDIS_CMD_READONLY)) {
            execCommandPropagateMulti(c);
            // 只需要传播一次MULTI命令即可
            must_propagate = 1;
        }

        // 真正执行命令
        call(c,REDIS_CALL_FULL);

        /* Commands may alter argc/argv, restore mstate. */
        // 命令执行后可能会被修改，需要更新操作
        c->mstate.commands[j].argc = c->argc;
        c->mstate.commands[j].argv = c->argv;
        c->mstate.commands[j].cmd = c->cmd;
    }
    // 恢复原命令
    c->argv = orig_argv;
    c->argc = orig_argc;
    c->cmd = orig_cmd;
    // 清除事务状态
    discardTransaction(c);
    /* Make sure the EXEC command will be propagated as well if MULTI
     * was already propagated. */
    if (must_propagate) server.dirty++;

handle_monitor:
    /* Send EXEC to clients waiting data from MONITOR. We do it here
     * since the natural order of commands execution is actually:
     * MUTLI, EXEC, ... commands inside transaction ...
     * Instead EXEC is flagged as REDIS_CMD_SKIP_MONITOR in the command
     * table, and we do it here with correct ordering. */
    if (listLength(server.monitors) && !server.loading)
        replicationFeedMonitors(c,server.monitors,c->db->id,c->argv,c->argc);
}
```

