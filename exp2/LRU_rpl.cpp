#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<ctime>
#include<queue>

using namespace std;
struct page {  //being divided process page
  int mem_page;
  bool in_mem;
  page* next;
  page* former;
};

struct pagecontrol { //distributive page
  bool used;
  int using_page;
};


int main(){
  int total_instruction,diseffect=0,ap,pp;
  cout<<"AP=";
  cin>>ap;
  cout<<"PP=";
  cin>>pp;
  cout<<"total_instruction=";
  cin>>total_instruction;

  int instruction[total_instruction];

  struct page pg[ap],* out;
  struct pagecontrol pgctrl[pp];
  for(int i=0;i<ap;i++){
    pg[i].mem_page=-1;
    pg[i].in_mem=false;
  }
  for(int i=0;i<pp;i++){
    pgctrl[i].used=false;
    pgctrl[i].using_page=-1;
  }
  srand((unsigned)time(NULL));
  for(int i=0;i<total_instruction;i++){
  instruction[i]=rand()%ap;
  cout<<instruction[i]<<" ";
  }
  cout<<endl;
  
  page* start = new(page);
  page* end = new(page);
  start->next = end;
  end->former = start;
  int size = 0;
 
  for(int i=0;i<total_instruction;i++){
    if (pg[instruction[i]].in_mem) {
      pg[instruction[i]].former->next=pg[instruction[i]].next;
      pg[instruction[i]].next->former=pg[instruction[i]].former;
      pg[instruction[i]].next=start->next;
      start->next->former=&pg[instruction[i]];
      pg[instruction[i]].former=start;
      start->next=&pg[instruction[i]];
      continue;
    }

    diseffect++;
     while(size>=pp) {//full condition, use LRU to delete.
      out=end->former;
      out->former->next=end;
      end->former=out->former;
      out->in_mem=false;
      out->next=NULL;
      out->former=NULL;
      pgctrl[out->mem_page].used=false;
      pgctrl[out->mem_page].using_page=-1;
      out->mem_page=-1;
      size--;
    }
    
    for (int j=0;j<pp;j++)  //find a free mem_page and link the free mem_page with page which need to be distributed.
      if (!pgctrl[j].used) 
        { 
          pgctrl[j].used=true;
	  pgctrl[j].using_page=i;
	  pg[instruction[i]].in_mem=true;
	  pg[instruction[i]].mem_page=j;
	  pg[instruction[i]].former=start;
	  pg[instruction[i]].next=start->next;
          start->next->former=&pg[instruction[i]];
	  start->next=&pg[instruction[i]];
	  size++;
	  break;
	}
  }
  delete start;
  delete end;
  double ans = (1-double(diseffect)/total_instruction)*100;
  printf("%.2f%%\n",ans);

  return 0;
}
