
/**client.c 
.		Litsakis Alexandros
. The above code makes compile with the command: gcc client.c -o runnable
. To run the executable we write to the terminal: runnable -s server port. 
.    -Where port the port server will listen. (Group Ports 9040-9049) 
.    -     server the hostname or ip of the proxy server
. The html to be requested is stored in the same dir as the runnable
. 
.
**/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#define MAX 4000000//the max size of html the client can receive 


void error(const char *msg) //called if there is an error.
{
    perror(msg);
    exit(0);
}





int main(int argc, char *argv[])
{
	int sockfd;// socket descriptor
	int n;// variable for send-receive
	int portno;//used port
	int counter,counter2,i;// assistant var
	int size;//the size of the html that the client will receive
	char exit2[]="EXIT";//c-string which helps to understand the client if the command given is exit,
        char buffer[256];// keeps outgoing messages
	char buf[BUFSIZ+1];//keeps incoming messages
        char html[MAX];//the c-string stored by the server
  	char name[256];// The name of the file to be written in html 
    struct sockaddr_in serv_addr;// address structures #include <sys/socket.h>
    struct hostent *server;//address structure #include <sys/socket.h>

	bzero(name,256);//initialize the name
	

    if (argc < 4) {//check whether the arguments given by the executing authority are correct
       fprintf(stderr,"usage %s -s hostname port\n", argv[0]);
       exit(0);
    }else if (strcmp(argv[1],"-s"))
		 {
	fprintf(stderr,"usage %s -s hostname port\n", argv[0]);exit(0);
		}	
	

    portno = atoi(argv[3]);// the port that server listen
	while(1){//δημιουργία tcp socket descriptor για Internet addresses
	size=0;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[2]);
//η gethostname αναζητά πληροφορίες για τον υπολογιστή με το όνομα hostname στη βάση δεδομένων 
//και τις αποθηκεύει στη δομή server
//Η δομή αυτή ορίζεται στη βιβλιοθήκη netinet.h/in.h

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));// αρχικοποίηση δομής socket address 
    serv_addr.sin_family = AF_INET;// ίδιο address family με την δημιουργία του socket
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);//αντιγράφει στην δομή διευθύνσεων. την διεύθυνση του server
    serv_addr.sin_port = htons(portno);// το portno ειναι το port που θα μιλήσει  ο client με τον proxy
//και αποθηκεύεται στην σωστή μορφή με την htons

  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) //συνδέετε με τον proxy
        error("ERROR connecting");
      printf("Please ");
    bzero(buffer,256);// αρχικοποίηση του buffer

	printf("\nGive the address from which you would like to get your html\n");
	printf("with GET or GETNEW(for new copy) in front \n");
	printf("or write EXIT+ press enter to terminate the proxy client:\n");
  gets(buffer); //διάβασμα απο τον user
	
	counter2=0;
	for (counter=0;counter<=3;counter++){
	if (buffer[counter]==exit2[counter])//έλενχος αν η εντολή που έδωσε ο user είναι η EXIT
	counter2++;
	}
	if (counter2==4){//αν ο counter δίχνει 4 ίδια char τότε η εντολή είναι η exit
	 n = write(sockfd,buffer,strlen(buffer));//στέλνει μήνυμα στον proxy  οτι θα κλείσει
	printf("bye bye\n");
	exit(0);//τερματίζει
	}	

   n = write(sockfd,buffer,strlen(buffer));//στέλνει αυτο που του έδωσε ο user στον proxy
    if (n < 0) 
         error("ERROR writing to socket");
    bzero (html,MAX);
	bzero (buffer,256);

	n = read(sockfd,buffer,255);//διαβάζει το size του html 
sscanf (buffer, "%d",&size);

counter=0;
memset(buf, 0, BUFSIZ+1);
while (1){n=0;
    n = read(sockfd,buf,BUFSIZ);//διαβάζει το html από τον proxy

for(i=0; i<strlen(buf); i++)//αντιγράφει τα εισερχόμενα bytes στο msg2
		{
			html[counter] = buf[i];
			counter++;
	     	}	
	   	memset(buf, 0, n);
	size -=n;//αν τα bytes  που έχουν διαβαστεί είναι μεγαλύτερα απο το μέγεθος
	if ((n<=0) || (size<=0))// ή το n=0 δηλαδή σταματήσει να στέλνει ο proxy
		break;//τοτε η read σταματάει!








	}html[counter]='\0';
	
    if (n < 0) 
         error("ERROR reading from socket");
 //   printf("%s\n",html);
   FILE *book;//δημιουργεί ένα pointer που θα δείχνει στο αρχείο
	printf("\n Please, give the name of file which you want to save your the html:  \n");
	bzero(name,strlen(name));
	gets(name);//διαβάζει το όνομα του html που θα αποθηκευτεί

	strcat(name,".html");//προσθέτει στο όνομα που δόθηκε το .html
if ((book=fopen(name,"w"))==NULL)//ανοίγει το αρχείο για γράψιμο
	error("cant open file or Bad name");


 fwrite(html,  1,strlen(html), book);//γράφει το html σε ενα file

fclose (book);//κλείνει τον pointer
    close(sockfd);// κλείνει τον socket descriptor


}
    return 0;
}
