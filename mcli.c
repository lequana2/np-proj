#include "unp.h"
#include "request.h"
#include "stdio.h"

#define TRUE  1
#define FALSE 0

int CAN_FIRST_HELP  = TRUE;
int CAN_SECOND_HELP = TRUE;
int CAN_THIRD_HELP  = TRUE;

/*main process*/
void cliProcess(FILE *fp, int sockfd);

/*main player choice*/
void printHelp();

/*validate function*/
int isValidedInputForQuestion(char *input, int len);
int isValidedInputForNavigation(char *input, int len);

/*print server message*/
int printRecvMessage(Request *req);

/*calculate score*/
int calScore(int number_of_defeated_players);

int main(int argc, char **argv)
{
  int sockfd;
  struct sockaddr_in  servaddr;

  if (argc != 2)
    err_quit("usage: tcpcli <IPaddress>");
  	
  sockfd = Socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
  
  cliProcess(stdin, sockfd);    // do it all

  exit(0);
}

void cliProcess(FILE *fp, int sockfd)
{
  char  sendline[MAXLINE], recvline[MAXLINE];
  int recv_bytes = 0;
  Request *req;
  // int received_question = 0;

  printf("DAU TRUONG 100\n");
  printf("Waiting for other players connect to game...\n");
  sendRequest(sockfd, 0, "m", 0);

  while ((req = recvRequest(sockfd)) != NULL) {
    if (printRecvMessage(req) == 0) break;
    
    Fgets(sendline, MAXLINE, fp);

    //TODO consider server messages    
    if (req->type == 9 || req->type == 10) {
      while (isValidedInputForQuestion(sendline, MAXLINE) == 0) {
        printf("Invalid input, reselect please\nYour choice: ");
        Fgets(sendline, MAXLINE, fp);
      }
      // consider request type !!!
      switch(sendline[0]){
        case '1': printf("\nSend request 1"); break;
        case '2': printf("\nSend request 2"); break;
        case '3': printf("\nSend request 3"); break;
      }
    } else if (req->type == 14) {
      while (isValidedInputForNavigation(sendline, MAXLINE) == 0) {
        printf("Invalid input, reselect please\nYour choice: ");
        Fgets(sendline, MAXLINE, fp);
      }
    } else {
      printf("Cannot reach here. Something wrongs\n");
    }

    printf("Submiting... Waiting for others...\n");

    // send message
    sendRequest(sockfd, TYPE_CLI_ANS, sendline, 0);
  }
  if (req == NULL)
    err_quit("str_cli: server terminated prematurely");
}

int isValidedInputForNavigation(char *input, int len) {
  int input_len = strlen(input);
  if ( input_len != 2) {
    return FALSE;
  }
  if (input[0] == 'y' || input[0] == 'n') {
    return TRUE;
  }
  return FALSE;
}

int isValidedInputForQuestion(char *input, int len) {
  int input_len = strlen(input);
  if ( input_len != 2) {
    return FALSE;
  }
  if ((int)input[0] >= '1' && (int)input[0] <= '3'){
	  return TRUE;	
  }
  if ((int)input[0] >= (int)'a' && (int)input[0] <= (int)'c') {
    return TRUE;
  }
  return FALSE;
}

void printHelp(){
	if(CAN_FIRST_HELP||CAN_SECOND_HELP||CAN_THIRD_HELP) printf("\n--- HELP HELP HELP ! ---");
	if(CAN_FIRST_HELP)  printf("\n1. Poll the mob \nPlayer choose an answer, show number of mob choose the same");
	if(CAN_SECOND_HELP) printf("\n2. Ask the mob  \nShow 1 incorrect, 1 correct answer."); //  If no one correct, not shown this option
	if(CAN_THIRD_HELP)  printf("\n3. Trust the mob\nShow the most popular answer");
}

int printRecvMessage(Request *req) {
  switch (req->type) {
    case 9: 
      // serv gui cau hoi
      printf("\nThere are %d remaining players\n", req->num);
      printf("You are having %d$ in reward\n", calScore(100-req->num));
      printf("%s\n", req->mess);
      printHelp();
      printf("\nYour choice: "); /*choice*/
      break;
    case 10:
      // main player tra loi cau hoi truoc khi 99 nguoi cung choi tra loi
      printf("%s\n", req->mess);
      printf("Your choice: ");
      break;
    case 12:
      // main player chien thang
      printf("\n%s\n", req->mess);
      printf("All other players are defeated.\n");
      printf("Your reward: %d$\n", req->num);
      return 0;
    case 13:
      // main player that bai
      printf("Right answer is %c. ", (char)req->num);
      printf("You failed in this question. %s\n", req->mess);
      return 0;
    case 14:
      // tiep tuc hay dung choi
      printf("%s\n", req->mess);
      printf("Your choice: ");
      break;
    case 15:
      // tiep tuc hay dung choi
      printf("You chosen to stop. Your reward: %d$\n", req->num);
      printf("%s\n", req->mess);
      return 0;
  }

  return 1;
}

int calScore(int number_of_defeated_players) {
  int n = number_of_defeated_players % 10;
  switch (n) {
    case 0:
      return 100;
      break;
    case 1:
      return 200;
      break;
    case 2:
      return 400;
      break;
    case 3:
      return 800;
      break;
    case 4:
      return 1000;
      break;
    case 5:
      return 2000;
      break;
    case 6:
      return 4000;
      break;
    case 7:
      return 8000;
      break;
    case 8:
      return 10000;
      break;
    case 9:
      return 20000;
      break;
  }
  return 0;
}
