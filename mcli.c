#include "unp.h"
#include "request.h"
#include "stdio.h"
#include "util.h"

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

int main(int argc, char **argv){
  int sockfd;
  struct sockaddr_in  servaddr;

  // check IP
  if (argc != 2) err_quit("usage: tcpcli <IPaddress>");
  	
  // get socket IPv4, data stream
  sockfd = Socket(AF_INET, SOCK_STREAM, 0);
  
  // bind and connect
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
  
  // client side
  cliProcess(stdin, sockfd);  

  // exit(0); 
  return 0;
}

void cliProcess(FILE *fp, int sockfd) {
  char  sendline[MAXLINE], recvline[MAXLINE];
  int recv_bytes = 0;
  Request *req;
  int reqType = 8;
  // int received_question = 0;

  printf("DAU TRUONG 100\n");
  printf("Waiting for other players connect to game...\n");
  sendRequest(sockfd, 0, "m", 0);

  while ((req = recvRequest(sockfd)) != NULL) {
    if (printRecvMessage(req) == 0) break;
    
    Fgets(sendline, MAXLINE, fp);

    //TODO consider server messages    
    if (req->type == 9 || req->type == 10 
      || req->type == TYPE_SERV_HELP_ANS) {
      while (isValidedInputForQuestion(sendline, MAXLINE) == 0) {
        printf("\nInvalid input, reselect please\nYour choice: ");
        Fgets(sendline, MAXLINE, fp);
      }
      // consider request type !!!
      switch(sendline[0]) {
        case '1': 
          printf("\nPlayer choose 1st help\n"); 
          reqType = TYPE_CLI_HELP;
          break;
        case '2': 
          printf("\nPlayer choose 2nd help\n"); 
          reqType = TYPE_CLI_HELP;
          break;
        case '3': 
          printf("\nPlayer choose 3rd help\n"); 
          reqType = TYPE_CLI_HELP;
          break;
        default:
          printf("\nYour last answer is %c\n", sendline[0]);
          reqType = TYPE_CLI_ANS;
      }
    } else if (req->type == 14) {
      while (isValidedInputForNavigation(sendline, MAXLINE) == 0) {
        printf("Invalid input, reselect please\nYour choice: ");
        Fgets(sendline, MAXLINE, fp);
      }
    } 
    // error    
    else {
      printf("Cannot reach here. Something wrongs\n");
    }

    printf("Submiting... Waiting for others...\n");

    sendRequest(sockfd, reqType, sendline, 0);
  }
  if (req == NULL)
    err_quit("str_cli: server terminated prematurely");
}

int isValidedInputForNavigation(char *input, int len) {
  int input_len = strlen(input);

  if ( input_len != 2) return FALSE;
  if (input[0] == 'y' || input[0] == 'n') return TRUE;
  
  return FALSE;
}

int isValidedInputForQuestion(char *input, int len) {
  int input_len = strlen(input);

  if ( input_len != 2) return FALSE;
  if ((int)input[0] == '1' && CAN_FIRST_HELP)  return TRUE;
  if ((int)input[0] == '2' && CAN_SECOND_HELP) return TRUE;
  if ((int)input[0] == '3' && CAN_THIRD_HELP)  return TRUE;
  if ((int)input[0] >= (int)'a' && (int)input[0] <= (int)'c') return TRUE;

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
    //TODO consider help 1 2 3
    case TYPE_SERV_HELP_ANS: 
      if (req->mess[0] == '1') {
        printf("1st help content\n");
        CAN_FIRST_HELP  = FALSE;
        printHelp();
        printf("\nYour choice: ");
        break;
      } else if (req->mess[0] == '2') {
        printf("2nd help content\n");
        CAN_SECOND_HELP = FALSE; 
        printHelp();
        printf("\nYour choice: ");
        break;
      } else if (req->mess[0] == '3') {
        printf("3rd help content\n");
        CAN_THIRD_HELP  = FALSE; 
        printHelp();
        printf("\nYour choice: "); /*choice*/
        break;
      }
      break;
    case 9: 
      // serv gui cau hoi
      printf("\nThere are %d remaining players\n", req->num);
      printf("You are having %d$ in reward\n", calScore(100-req->num));
      printf("%s\n", req->mess);
      printHelp();
      printf("\n\nYour choice: "); /*choice*/
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
      return FALSE;
    case 13:
      // main player that bai
      printf("Right answer is %c. ", (char)req->num);
      printf("You failed in this question. %s\n", req->mess);
      return FALSE;
    case 14:
      // tiep tuc hay dung choi
      printf("%s\n", req->mess);
      printf("Your choice: ");
      break;
    case 15:
      // tiep tuc hay dung choi
      printf("You chosen to stop. Your reward: %d$\n", req->num);
      printf("%s\n", req->mess);
      return FALSE;
  }

  return TRUE;
}
