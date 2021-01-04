#include<sys/types.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

int main()
{
	pid_t pid, pid1;

	pid = fork();
			
	if (pid < 0)
	{
	fprintf(stderr, "Failed");
	return 1;
	}
	else if (pid == 0)
	{
	execl("/home/wzm/OS_exp/exp1/h","none",(char*)0);
	}
	else{
	printf("Done!\n");
	wait(NULL);
	}

	return 0;
}
