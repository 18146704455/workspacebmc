/*************************************************************************
  > File Name: time.c
  > Author: 
  > Mail: 
  > Created Time: Tue 14 Jan 2020 03:36:37 PM CST
 ************************************************************************/
#include<stdio.h>
#include<sys/time.h>
#include<unistd.h>
int main()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    printf("tv_sec:%d\n",tv.tv_sec*100);
    printf("tv_usec:%d\n",tv.tv_usec/100);
    return 0;
}
