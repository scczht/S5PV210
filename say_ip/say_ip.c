#include <linux/input.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "get_mac_ip.h"
typedef enum{false=0,true=!false} bool ;
#define KEY_TIMES   3
int i;
char mp3_name[60];
char count; 

char *pars_ip( char *value)
{
  char ipaddr[40]={0};
  char temp[40]={0};
  char *p;
  if(!get_ip("wlan0",ipaddr))
  {
    printf("get IP error\n");  
  }
  printf("ip:%s\n",ipaddr);
  strcpy(value,ipaddr);
  printf("---------------\n");
  return value;
}
//To determine whether a file exists
int isExist(char *filename)  
{  
 return (access(filename, 0) == 0);   
}
bool getkey_gpio_146()
{
   unsigned int value;
   FILE *pRreco = NULL;
   if(!isExist("/sys/class/gpio/gpio146"))   
   {
	unsigned int value;
	FILE *pRreco = NULL;
	if((pRreco = fopen("/sys/class/gpio/export", "w")) == NULL)
	{
	    printf("Open read gpio/export\n");
		return -1;
	}
   if(fwrite("146", strlen("146"), 1, pRreco) != 1)
    printf("File Write Error!\n");
	fclose(pRreco);  
  } 
	if((pRreco = fopen("/sys/class/gpio/gpio146/value","r")) == NULL)
	{
	    printf("Open read recovery button failure\n");
		return -1;
	}
	fscanf(pRreco,"%d",&value);
	fclose(pRreco);
	if(value)
		return true;
	else
		return false;
}

int key_test()             //Removing jitter
{
 int i=0;
 int key_times=0; 
 for(i=0;i<KEY_TIMES;i++)
 {
  if((getkey_gpio_146() == false))  //button down 
  { 
  usleep(20*1000);
  key_times++;
  }  
 } 
 if(KEY_TIMES==key_times)
  {
	 while((getkey_gpio_146() == false)); 
	 return 1;
  }
  return 0;
}

void say_ip()
{
 char *value;
 printf("key Down!!\n");
 value=(char*)calloc(40,sizeof(char));  
 count=strlen(pars_ip(value));
 system("madplay /home/plg/src/R2d2.mp3");
 sleep(1);
 system("madplay /home/plg/src/nihao.mp3");
 sleep(1);
 for(i=0;i<count;i++)
 {  
  if(value[i] != '.')
   {
    if(value[i]>='0'&&value[i]<='9')
	 {
	  sprintf(mp3_name,"madplay /home/plg/src/%c.mp3",value[i]);
	  //printf("%s\n",mp3_name);
	  system(mp3_name);
	  usleep(400000);
	 }
 else
   {
	printf("it is ERROR");
	system("madplay /home/plg/src/ip_error.mp3");
   }
   }
   else
   {
    printf("it is .\n");
	system("madplay /home/plg/src/dian.mp3");
	usleep(400000);
   }
}
 free(value);
}


void main()
{
 printf("say_ip： %s--%s\n", __DATE__,__TIME__);
 say_ip();
 while(1)
 {
   if(1==key_test())
   {
    say_ip(); 
   }
   else
   {
    usleep(500*1000);
   }
 }
}

























