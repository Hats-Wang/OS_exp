#include<sys/types.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

int check=1024;

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
        check--;
        printf("child:check = %d\n", check);
	}
	else{
        check++;
	printf("parent:check = %d\n", check);
	wait(NULL);
	}

	return 0;
}
