#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/*file for currnet guest information*/
#define FILE_NAME "guests"
/*max length of the name for a guest*/
#define NAME_LEN_MAX 21
/*total number of rooms */
#define NUM_ROOMS 10

/* print guest name in each room*/
void print_guests()
{
	int fd;
	off_t offset;
	ssize_t nread;

	int i;
	char namebuf[NAME_LEN_MAX+1];

	if((fd = open(FILE_NAME, O_RDONLY)) == -1)
	{
		printf("fail to open file\n");	exit(0);
	}

	for(i=1; i<=NUM_ROOMS; i++)
	{

		offset = (i - 1) * NAME_LEN_MAX;

		if(lseek(fd, offset, SEEK_SET) == -1)
		{
			printf("faile to seek position\n"); exit(0);
		}

		nread = read(fd, namebuf, NAME_LEN_MAX);
		namebuf[NAME_LEN_MAX] = '\0';

		if(nread > 0)
		{
			printf("room # %d: %s", i, namebuf);
		}
		else
		{
			printf("room # %d:", i);
		}

		memset(namebuf, 0, NAME_LEN_MAX);
	}

	close(fd);
}


/* hw#1-1 define this function
 *this fuction finds the number of empty rooms
 *return  "the SMALLEST ROOM NUMBER among empty rooms" if successful & and print out "the number of empty rooms is XXX"; 
 *return 0 if there are no available rooms & print out "there are no available rooms"
 */
int check_vacancies()
{
	int empty_room = 0;
	int smallest = -1;

	//add your code here
	 int fd;
        off_t offset;
        ssize_t nread;

        int i;
        char namebuf[NAME_LEN_MAX+1];

        if((fd = open(FILE_NAME, O_RDONLY)) == -1)
        {
                printf("fail to open file\n");  exit(0);
        }

        for(i=1; i<=NUM_ROOMS; i++)
        {

           offset = (i - 1) * NAME_LEN_MAX;
           if(lseek(fd, offset, SEEK_SET) == -1){
           printf("faile to seek position\n"); exit(0);}
           nread = read(fd, namebuf, NAME_LEN_MAX);
           namebuf[NAME_LEN_MAX] = '\0';
           if(nread > 0)
	   {
		 
                   if(namebuf[0] == 32){
                   empty_room++;
                   }
            }
	  memset(namebuf,0,NAME_LEN_MAX);
	}

        close(fd);

	if (empty_room)
	{
		printf("the number of empty rooms is %d\n",empty_room);
		return smallest;
	}
	else
	{
		printf("there are no available rooms\n");
		return 0;
	}


}



/* hw#1-2 define this function
 *1st arg: guest name
 *2nd arg: room # to assign
 *this fuction adds guest name (1st arg) to the corresponding file position (2nd arg) 
 *return  0 if successful; 
 *return -1 if the room is occupied & print out "room # <2nd arg> is occupied by XXX"
 *return -1 if the room number exceeds NUM_ROOMS & print out "room # <2nd arg> does not exist"
 */
int checkin_guest(char *guestname, int roomnum)
{
	//add your code here
	int fd;
        off_t offset;
        ssize_t nread;
	ssize_t nwrite;
        int i;
       char namebuf[NAME_LEN_MAX+1];
       char in_name[NAME_LEN_MAX+1];
	if((fd = open(FILE_NAME,O_CREAT |  O_RDWR,0644)) == -1)
        {
                printf("fail to open file\n");  exit(0);
        }

	if(roomnum<=NUM_ROOMS){
		offset = ((roomnum-1) * NAME_LEN_MAX);
                if(lseek(fd,offset,SEEK_SET)== -1){
                printf("fali to seek position\n"); exit(0);}
	       	
		nread = read(fd, namebuf, NAME_LEN_MAX);
          	namebuf[NAME_LEN_MAX] = '\0';
		
		if(namebuf[0] == 32){
			
		if((nwrite = pwrite(fd,guestname,strlen(guestname),offset)) == -1){
                	printf("error while writing\n"); exit(0);}
                	return 0;
		}
		
		else if(namebuf[0] != 32){
		printf("room #%d is occupied by %s",roomnum,namebuf);
		return -1;}}
	else {
	 printf("room #%d does not exist\n",roomnum);
                return -1;}
	close(fd);
}


/* hw#1-3 define this function
 *1st arg: guest name
 *this fuction eliminates guest name (1st arg) from the list
 *return "room #" that beomces empty if successful; 
 *return -1 if the given name (1st arg) is not in the list & print out "guest XXX does not exist in the room"
 */
int checkout_guest(char *guestname)
{
	//add your code here
	int fd;
        off_t offset;
        ssize_t nread;
        ssize_t nwrite;
        int i;
       char namebuf[NAME_LEN_MAX+1];
      int roomnum = 0;
      
        if((fd = open(FILE_NAME,O_CREAT |  O_RDWR,0644)) == -1)
        {
                printf("fail to open file\n");
		exit(0);
        }

	for(i=1; i<=NUM_ROOMS; i++){
		offset = (i-1) * NAME_LEN_MAX;
		if(lseek(fd,offset,SEEK_SET) == -1){
		printf("faile to seek position\n");}
		
		nread = read(fd,namebuf,NAME_LEN_MAX);
		if(strstr(namebuf, guestname) != NULL){
		roomnum = i;
		char none[strlen(guestname)];
		for(int j=0; j<strlen(guestname); j++){
			strcat(none," ");
		}
		if((nwrite = pwrite(fd,none,strlen(guestname),offset)) == -1){
                printf("error while writing\n"); exit(0);}
                }
		}

	if(roomnum ==0) {
	printf("guest %s does not exist in the room\n",guestname);
	return -1;
	}
}



int main(int argc, char *argv[])
{
	char guestname[NAME_LEN_MAX];
	int roomnum;
	int result;

	/*print out the current status*/
	print_guests();


	//check vacancies
	result = -1;
	if((result = check_vacancies()) > 0) 	
	{
		printf("the smallest number of empty rooms is %d\n", result);
	}

	//add guestname to the position of roomnum
	result = -1;
	memcpy(guestname, "kidzero" , NAME_LEN_MAX);
	roomnum = 9;
	if((result = checkin_guest(guestname, roomnum)) != -1)
	{
		printf("check in completed\n");
		print_guests();
	}

	//remove guestname from where it actually is
	result = -1;
	memcpy(guestname, "kidzero", NAME_LEN_MAX);
	if((result = checkout_guest(guestname)) != -1)
	{
		printf("check out completed\n");
		print_guests();
	}

	return 0;
}


