/*************************************************************************
  > File Name: test_sys_file.c
  > Author: 
  > Mail: 
  > Created Time: Thu 16 Jan 2020 03:33:15 PM CST
 ************************************************************************/
#include <sys_file.h>
#include <stdio.h>
  
int main(int argc, char *argv[])
{
  char tmp[123];
  int len;
  int i=0;
  if(argc==3)
    write_sysfs_raw(argv[1],argv[2],strlen(argv[2])+1);
  len = read_sysfs_raw(argv[1],tmp,123);
  printf("raw:[");
  for(i=0; i<len;i++)
    printf("%x ",tmp+i);
  printf("]\n");
  printf("int:%d\n",read_sysfs_int(argv[1],0));
  printf("hex:%d\n",read_sysfs_hex(argv[1],0));
  return 0;
}
