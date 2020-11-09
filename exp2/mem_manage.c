#include<stdio.h>
#include<stdlib.h>

#define PROCESS_NAME_LEN 32   /*进程名长度*/
#define MIN_SLICE    10             /*最小碎片的大小*/
#define DEFAULT_MEM_SIZE 1024     /*内存大小*/
#define DEFAULT_MEM_START 0       /*起始位置*/
/* 内存分配算法 */
#define MA_FF 1
#define MA_BF 2
#define MA_WF 3


/*描述每一个空闲块的数据结构*/
struct free_block_type{
  int size;
  int start_addr;
  struct free_block_type *next;
};
/*指向内存中空闲块链表的首指针*/
struct free_block_type *free_block;

/*每个进程分配到的内存块的描述*/
struct allocated_block{
  int pid;    int size;
  int start_addr;
  char process_name[PROCESS_NAME_LEN];
  struct allocated_block *next;
};
/*进程分配内存块链表的首指针*/
struct allocated_block *allocated_block_head = NULL;

int mem_size=DEFAULT_MEM_SIZE; /*内存大小*/
int ma_algorithm = MA_FF;           /*当前分配算法*/
static int pid = 0;                                      /*初始pid*/
int flag = 0,free_block_count = 0,current_free_mem;                             /*设置内存大小标志*/

struct free_block_type* init_free_block(int mem_size);
void display_menu();
int set_mem_size();
void set_algorithm();
void rearrange(int algorithm);
int rearrange_FF();
int rearrange_BF();
int rearrange_WF(); 
int new_process();
int allocate_mem (struct allocated_block *ab);
struct allocated_block * find_process(int pid);
void kill_process();
int free_mem (struct allocated_block *ab);
int dispose (struct allocated_block *free_ab);
int display_mem_usage();
void do_exit();
int allocate_FF(struct allocated_block *ab);
int allocate_BF(struct allocated_block *ab);
int allocate_WF(struct allocated_block *ab);
int allocate(struct free_block_type *pre,struct free_block_type *allocate_free_block,struct allocated_block *ab);

int main(){
  char choice;   pid=0;
  free_block = init_free_block(mem_size); //初始化空闲区
  while(1) {
  display_menu();	//显示菜单
  fflush(stdin);
  scanf("%s",&choice);
  switch(choice){
  case '1': set_mem_size(); break; 	//设置内存大小
  case '2': set_algorithm();flag=1; break;//设置算法
  case '3': new_process(); flag=1; break;//创建新进程
  case '4': kill_process(); flag=1;   break;//删除进程				
  case '5': display_mem_usage();  flag=1; break;	//显示内存使用		
  case '0': do_exit(); exit(0);	//释放链表并退出							
  default: break;
    }    
  } 
}

struct free_block_type* init_free_block(int mem_size){
  struct free_block_type *fb;
  fb=(struct free_block_type *)malloc(sizeof(struct free_block_type));
  if(fb==NULL){
  printf("No mem\n");
  return NULL;					            
  }
  fb->size = mem_size;
  fb->start_addr = DEFAULT_MEM_START;
  fb->next = NULL;
  current_free_mem = mem_size;
  free_block_count=1;
  return fb;
}

void display_menu(){
  printf("\n");
  printf("1 - Set memory size (default=%d)\n", DEFAULT_MEM_SIZE);
  printf("2 - Select memory allocation algorithm\n");
  printf("3 - New process \n");
  printf("4 - Terminate a process \n");
  printf("5 - Display memory usage \n");
  printf("0 - Exit\n");
}

int set_mem_size(){
  int size;
  if(flag!=0){  //防止重复设置
  printf("Cannot set memory size again\n");
  return 0;
  }
  printf("Total memory size =");
  scanf("%d", &size);
  if(size>0) {
  mem_size = size;
  free_block->size = mem_size;
  }
  flag=1;  return 1;
}

void set_algorithm(){
  int algorithm;
  printf("\t1 - First Fit\n");
  printf("\t2 - Best Fit \n");
  printf("\t3 - Worst Fit \n");
  scanf("%d", &algorithm);
  if(algorithm>=1 && algorithm <=3)  
  ma_algorithm=algorithm;
  //按指定算法重新排列空闲区链表
  rearrange(ma_algorithm); 
}

void rearrange(int algorithm){
  switch(algorithm){
  case MA_FF:  rearrange_FF(); break;
  case MA_BF:  rearrange_BF(); break;
  case MA_WF:  rearrange_WF(); break;
  }
}

/*按FF算法重新整理内存空闲块链表*/
int  rearrange_FF(){   
//请自行补充
  struct free_block_type *head;
  struct free_block_type *pre,*now,*next;
  int i;
  for(i = 0;i < free_block_count;i++){
  
  head= free_block;
  if (head == NULL) return -1;
  pre = NULL; 
  now = head;
  
  next = now->next; 
  if(next == NULL)  return 1;

  while(next != NULL){
    if (pre== NULL&&now->start_addr > next->start_addr){
    //比较空闲链表中第一个空闲块与第二个空闲块的开始地址的大小
    head->next= next->next;
    next->next= head;
    head= next;
    pre = head;
    now= pre->next;
    next= now->next;
    free_block=head;
    }
    else if(now->start_addr > next->start_addr){
    now->next= next->next;
    pre->next= next;
    next->next= now;
    pre= next;
    next= now->next;
    }
    else{
    pre = now;
    now = next;
    next = next->next;
    }
   }
  }
  return 1;
}
/*按BF算法重新整理内存空闲块链表*/
int rearrange_BF(){
//请自行补充
  struct free_block_type *head;
  struct free_block_type *pre,*now,*next;
  int i;
  for(i = 0;i < free_block_count;i++){
    head= free_block;
   if (head == NULL) return -1;
   pre = NULL;
   now = head;
 
   next = now->next;
   if(next == NULL)  return 1;
 
    while(next != NULL){
      if (now->size > next->size&& pre == NULL){
      //比较空闲链表中第一个空闲块与第二个空闲块的开始地址的大小
     head->next= next->next;
     next->next= head;
     head= next;
     pre = head;
     now= pre->next;
     next= now->next;
     free_block=head;
      }
      else if(now->size > next->size){
      now->next= next->next;
      pre->next= next;
      next->next= now;
      pre= next;
      next= now->next;
      }
       else{
	  pre = now;
	  now = next;
	  next = next->next;
	  }
    }
  }  
  return 1;
}
/*按WF算法重新整理内存空闲块链表*/
int rearrange_WF(){
//请自行补充
  struct free_block_type *head;
  struct free_block_type *pre,*now,*next;
  int i;
  for(i = 0;i < free_block_count;i++){
    head= free_block;
    if (head == NULL) return -1;
    pre = NULL;
    now = head;
 
    next = now->next;
    if(next == NULL)  return 1;


    while(next != NULL){
      if (now->size < next->size && pre == NULL){
	  //比较空闲链表中第一个空闲块与第二个空闲块的开始地址的大小
      head->next= next->next;
      next->next= head;
      head= next;
      pre = head;
      now= pre->next;
      next= now->next;
      free_block=head;
	 }
      else if(now->size < next->size){
      now->next= next->next;
      pre->next= next;
      next->next= now;
      pre= next;
      next= now->next;
      }
      else{
	  pre = now;
	  now = next;
	  next = next->next;
	 }
    }
  }
  return 1;
  }
  
/*创建新的进程，主要是获取内存的申请数量*/
int new_process(){
  struct allocated_block *ab;
  int size;    int ret;
  ab=(struct allocated_block *)malloc(sizeof(struct allocated_block));
  if(!ab) exit(-5);
  ab->next = NULL;
  pid++;
  sprintf(ab->process_name, "PROCESS-%02d", pid);
  ab->pid = pid;    
  printf("Memory for %s:", ab->process_name);
  scanf("%d", &size);
  if(size<=0) {
    printf("\ninvalid number of memory\n"); 
    return -1;
  }
  ab->size=size;
  ret = allocate_mem(ab);  /* 从空闲区分配内存，ret==1表示分配ok*/  
  if((ret==1) &&(allocated_block_head == NULL)){
    allocated_block_head=ab;
    return 1;        }
  /*分配成功，将该已分配块的描述插入已分配链表*/
  else if (ret==1) {
    ab->next=allocated_block_head;
    allocated_block_head=ab;
    return 2;        }
  else if(ret==-1){ /*分配不成功*/
    printf("\nAllocation fail\n");
    free(ab);
    return -1;       
  }
  return 3;
  }

int allocate_mem(struct allocated_block *ab){
  int ret;
  switch(ma_algorithm){
      case MA_FF:  ret = allocate_FF(ab); break;
      case MA_BF:  ret = allocate_BF(ab); break;
      case MA_WF:  ret = allocate_WF(ab); break;
}
return ret;
}

int allocate_FF(struct allocated_block *ab){
  struct free_block_type *ff= free_block,* pre=NULL;
  int ret;
  if (ff == NULL) return -1;
  while(ff != NULL){
    if (ff->size >= ab->size) {
        ret = allocate(pre,ff,ab);
	break;
    }
    pre =ff;
    ff = ff->next;
  }
if (ff->size < ab->size) return -1;
  rearrange_FF();
  return ret;
}

int allocate_BF(struct allocated_block *ab){
struct free_block_type *bf= free_block,* pre=NULL;
int ret;  
if (bf == NULL) return -1;
    while(bf != NULL){
      if (bf->size >= ab->size) {
	 ret = allocate(pre,bf,ab);
         break;    
      }
      pre = bf;
      bf = bf->next;
    }
   if (bf->size < ab->size) return -1;
   rearrange_BF();
   return ret;
}

int allocate_WF(struct allocated_block *ab){
struct free_block_type *wf= free_block;
int ret;  
if (wf == NULL) return -1;

  if (wf->size >= ab->size) ret = allocate(NULL,wf,ab);
  else if (wf->size < ab->size) return -1;
  rearrange_WF();
  return ret;

}

int allocate(struct free_block_type *pre,struct free_block_type *allocate_free_block,struct allocated_block *ab){
	ab->start_addr = allocate_free_block->start_addr;
	
        if(allocate_free_block->size < MIN_SLICE) {
	  ab->size = allocate_free_block->size;
          if(pre == NULL)  free_block= allocate_free_block->next;
	  else pre->next = allocate_free_block->next;
	  free(allocate_free_block);
          free_block_count--;
	}
	else{
	allocate_free_block->start_addr+=ab->size;
	allocate_free_block->size-=ab->size;
	}
	current_free_mem -= ab->size;
	if(!current_free_mem) free_block=NULL;
	return 1;
}

struct allocated_block* find_process(int pid){
  struct allocated_block *ab= allocated_block_head;
  if (ab == NULL){
  printf("\nno allocated block!\n");
  return NULL;
  }

  while(ab->next != NULL && ab->pid != pid) ab = ab->next;
  if (ab->pid == pid) return ab;
  printf("\ncan't find the appointed allocated_block!\n");
  
  return NULL;
}

void kill_process(){
  struct allocated_block *ab;
  int pid;
  printf("Kill Process, pid=");
  scanf("%d", &pid);
  ab=find_process(pid);
  if(ab!=NULL){
  free_mem(ab); /*释放ab所表示的分配区*/
  dispose(ab);  /*释放ab数据结构节点*/
  }
}

/*将ab所表示的已分配区归还，并进行可能的合并*/
int free_mem(struct allocated_block *ab){
  int algorithm = ma_algorithm;
  struct free_block_type *fbt, *pre=free_block, *work;
  fbt=(struct free_block_type*) malloc(sizeof(struct free_block_type));
  if(!fbt) return -1;
  fbt->size=ab->size;fbt->start_addr=ab->start_addr;fbt->next=NULL;

  if (pre==NULL) free_block=fbt;
  else{
  while(pre->next != NULL) pre=pre->next;
  pre->next = fbt;
  }
  free_block_count++;
  rearrange_FF();
  
  pre=free_block;
  work=pre->next;
  while(work!=NULL){
  if(pre->start_addr+pre->size==work->start_addr){
    pre->size+=work->size;
    pre->next=work->next;
    free(work);  
    free_block_count--;
    work=pre->next;
  }
  else{
    pre=work;
    work=work->next;
    }
  }

  rearrange(ma_algorithm);
  current_free_mem+= ab->size;
  return 1;
}

/*释放ab数据结构节点*/
int dispose(struct allocated_block *free_ab){
  struct allocated_block *pre, *ab;
  if(free_ab == allocated_block_head) { /*如果要释放第一个节点*/
  allocated_block_head = allocated_block_head->next;
  free(free_ab);
  return 1;
  }
  pre = allocated_block_head;  
  ab = allocated_block_head->next;
  while(ab!=free_ab){ pre = ab;  ab = ab->next; }
  pre->next = ab->next;
  free(ab);
  return 1;
}
  
  /* 显示当前内存的使用情况，包括空闲区的情况和已经分配的情况 */
int  display_mem_usage(){
  struct free_block_type *fbt=free_block;
  struct allocated_block *ab=allocated_block_head;
  if(fbt==NULL) return(-1);
  printf("----------------------------------------------------------\n");
  
  /* 显示空闲区 */
  printf("Free Memory:\n");
  printf("%20s %20s\n", "start_addr", "size");
  while(fbt!=NULL){
  printf("%20d %20d\n", fbt->start_addr, fbt->size);
  fbt=fbt->next;
}    
  /* 显示已分配区 */
  printf("\nUsed Memory:\n");
  printf("%10s %20s %10s %10s\n", "PID", "ProcessName", "start_addr", "size");
  while(ab!=NULL){
  printf("%10d %20s %10d %10d\n", ab->pid, ab->process_name, ab->start_addr, ab->size);
  ab=ab->next;
  }
  printf("----------------------------------------------------------\n");
  return 0;
} 

void do_exit(){
  struct allocated_block *allocated_ab,*allocated_next;
  struct free_block_type *free_ab,*free_next;
  free_ab= free_block;
  allocated_ab= allocated_block_head;
  if(free_ab!= NULL)
  {
    free_next= free_ab->next;
    while(free_next!= NULL)
    {
      free(free_ab);
      free_ab= free_next;
      free_next= free_next->next;
    }
    free(free_ab);
  }
  if(allocated_ab!= NULL)
  {
    allocated_next= allocated_ab->next;
    while(allocated_next!= NULL)
    {
      free(allocated_ab);
      allocated_ab= allocated_next;
      allocated_next= allocated_next->next;
    }
    free(allocated_ab);
  }
}
