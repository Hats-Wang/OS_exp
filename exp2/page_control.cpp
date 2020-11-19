#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<ctime>
#include<queue>

using namespace std;
struct page {  //being divided process page
  int mem_page;
  bool in_mem;
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
  queue<page*> memory_page;
  struct page pg[ap],*out;
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
  for(int i=0;i<total_instruction;i++){
    if (pg[instruction[i]].in_mem) continue;
    diseffect++;
     while(memory_page.size()>=pp) {//full condition, use FIFO
    out=memory_page.front();
    memory_page.pop();
    out->in_mem=false;
    pgctrl[out->mem_page].used=false;
    pgctrl[out->mem_page].using_page=-1;
    out->mem_page=-1;
     }
    int j;
    for (j=0;j<pp;j++){  //find a free mem_page and link the free mem_page with page which need to be distributed.
      if (!pgctrl[j].used) 
        { 
          pgctrl[j].used=true;
	  pgctrl[j].using_page=i;
	  pg[instruction[i]].in_mem=true;
	  pg[instruction[i]].mem_page=j;
	  break;
	}
    }
    if (j==pp) cout<<"wrong";
  memory_page.push(&pg[instruction[i]]);
  }
  double ans = (1-double(diseffect)/total_instruction)*100;
  printf("%.2f%%\n",ans);
  return 0;
}
