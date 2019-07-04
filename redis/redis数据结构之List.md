### redis数据结构之`List`

redis的链表结构为双向链表，并为其实现了一个list迭代器。

```c
/* listNode结点 */
typedef struct listNode {
	//结点的前一结点
    struct listNode *prev;
    //结点的下一结点
    struct listNode *next;
    //Node的函数指针
    void *value;
} listNode;

/* list迭代器，只能为单向 */
typedef struct listIter {
	//当前迭代位置的下一结点
    listNode *next;
    //迭代器的方向
    int direction;
} listIter;
 
/* listNode 列表 */
typedef struct list {
	//列表头结点
    listNode *head;
    //列表尾结点
    listNode *tail;
    
    /* 下面3个方法为所有结点公用的方法，分别在相应情况下回调用 */
    //复制函数指针
    void *(*dup)(void *ptr);
    //释放函数指针
    void (*free)(void *ptr);
   	//匹配函数指针
    int (*match)(void *ptr, void *key);
    //列表长度
    unsigned long len;
} list;

/* 宏定义方法 */

#define listLength(l) ((l)->len)   //获取list长度
#define listFirst(l) ((l)->head)   //获取列表首部
#define listLast(l) ((l)->tail)    //获取列表尾部
#define listPrevNode(n) ((n)->prev)  //给定结点的上一结点
#define listNextNode(n) ((n)->next)  //给定结点的下一节点
#define listNodeValue(n) ((n)->value) //给点的结点的值，这个value不是一个数值类型，而是一个函数指针
 
#define listSetDupMethod(l,m) ((l)->dup = (m))  //列表的复制方法的设置
#define listSetFreeMethod(l,m) ((l)->free = (m)) //列表的释放方法的设置
#define listSetMatchMethod(l,m) ((l)->match = (m)) //列表的匹配方法的设置
 
#define listGetDupMethod(l) ((l)->dup) //列表的复制方法的获取
#define listGetFree(l) ((l)->free)     //列表的释放方法的获取
#define listGetMatchMethod(l) ((l)->match) //列表的匹配方法的获取


/* Prototypes */
/* 定义了方法的原型 */
list *listCreate(void);   //创建list列表
void listRelease(list *list);  //列表的释放
list *listAddNodeHead(list *list, void *value);  //添加列表头结点
list *listAddNodeTail(list *list, void *value);  //添加列表尾结点
list *listInsertNode(list *list, listNode *old_node, void *value, int after);  //某位置上插入及结点
void listDelNode(list *list, listNode *node);  //列表上删除给定的结点
listIter *listGetIterator(list *list, int direction);  //获取列表给定方向上的迭代器
listNode *listNext(listIter *iter);  //获取迭代器内的下一结点
void listReleaseIterator(listIter *iter);  //释放列表迭代器 
list *listDup(list *orig);  //列表的复制
listNode *listSearchKey(list *list, void *key); //关键字搜索具体结点
listNode *listIndex(list *list, long index);   //下标索引具体的结点
void listRewind(list *list, listIter *li);    // 重置迭代器为方向从头开始 
void listRewindTail(list *list, listIter *li); //重置迭代器为方向从尾部开始 
void listRotate(list *list);  //列表旋转操作，方法名说的很玄乎，具体只能到实现里去看了
 
/* Directions for iterators */
/* 定义2个迭代方向，从头部开始往尾部，第二个从尾部开始向头部 */
#define AL_START_HEAD 0
#define AL_START_TAIL 1

/* 方法的实现 */
list *listCreate(void) /* list创建 */
{
    struct list *list;
 
	//申请空间，如果失败了就直接返回NULL
    if ((list = zmalloc(sizeof(*list))) == NULL)
        return NULL;
    //初始化操作，头尾结点，，3个公共的函数指针全部赋值为NULL
    list->head = list->tail = NULL;
    list->len = 0;
    list->dup = NULL;
    list->free = NULL;
    list->match = NULL;
    return list;
}
void listRelease(list *list) /* 释放 先释放实际node节点，再释放list管理结构体 */
{
    unsigned long len;
    listNode *current, *next;
	
	//找到当前结点，也就是头结点
    current = list->head;
    len = list->len;
    while(len--) {
    	//while循环依次释放结点
        next = current->next;
        //如果列表有free释放方法定义，每个结点都必须调用自己内部的value方法
        if (list->free) list->free(current->value);
        //采用redis新定义的在zfree方式释放结点，与zmalloc对应，不是free！！ 
        zfree(current);
        current = next;
    }
    //最后再次释放list同样是zfree
    zfree(list);
}
```



