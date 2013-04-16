#include "unp.h"
#include "request.h"

void cliProcess(FILE *fp, int sockfd);
int isValidedInput(char *input, int len);
int printRecvMessage(Request *req);

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
  
  cliProcess(stdin, sockfd);    /* do it all */

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
    if (printRecvMessage(req) == 0) {
      break;
    }
    
    Fgets(sendline, MAXLINE, fp);
    while (isValidedInput(sendline, MAXLINE) == 0) {
      printf("Invalid input, reselect please\nYour choice: ");
      Fgets(sendline, MAXLINE, fp);
    }
    printf("Submiting... Waiting for others...\n");
    sendRequest(sockfd, 1, sendline, 0);

    // if (!is_valided_input(sendline, MAXLINE)) {
    //   printf("Invalid input, choice again\nYour choice: ");;
    //   continue;
    // }

    // if ((received_question == 0) && ((int)sendline[0] >= (int)'a') && ((int)sendline[0] <= (int)'d')) {
    //   printf("Get ready by press y\nYour choice: ");;
    //   continue;        
    // }



    // else if (recvline[0] == 'c') {
    //   received_question = 1;
    // }

  }
  if (req == NULL)
    err_quit("str_cli: server terminated prematurely");
}

// 0 is invalid
int isValidedInput(char *input, int len) {
  int input_len = strlen(input);
  if ( input_len != 2) {
    return 0;
  }
  if (input[0] == 'y') {
    return 1;
  }
  if ((int)input[0] < (int)'a' || (int)input[0] > (int)'c') {
    return 0;
  }
  return 1;
}

int printRecvMessage(Request *req) {
  // char *main_player_answer_fast_forward = "You must wait for others finish their answer";
  // char *mp_after_anwsering_wrong_message = "Main player failed to victory!";
  // char *mp_not_continue = "Main player decided to stop. Good bye!";

  // if (strcmp(recvline, "mpaff") == 0) {
  //   printf("%s\n", main_player_answer_fast_forward);
  // } else if (strcmp(recvline, mp_after_anwsering_wrong_message) == 0) {
  //   printf("%s\n", mp_after_anwsering_wrong_message);
  //   return 0;
  // } else if (recvline[0] == '8') {
  //   printf("%s\n", mp_not_continue);
  //   return 0;
  // } else if (recvline[0] == '9') {
  //   printf("%s%s\n", recvline+1, ". You decided a wrong answer. Good bye!");
  //   return 0;
  // } else {
  //   printf("%s\n", recvline);
  // }

  switch (req->type) {
    case 9: 
      // serv gui cau hoi
      printf("\nThere are %d remaining players\n", req->num);
      printf("%s\n", req->mess);
      printf("Your choice: ");
      break;
    case 10:
      // main player tra loi cau hoi truoc khi 99 nguoi cung choi tra loi
      printf("%s\n", req->mess);
      printf("Your choice: ");
      break;
    case 12:
      // main player chien thang
      printf("%s\n", req->mess);
      printf("All other players are defeated.\n");
      printf("Your reward: %d\n", req->num);
      return 0;
    case 13:
      // main player that bai
      printf("Right answer is %c. ", (char)req->num);
      printf("You failed in this question. %s\n", req->mess);
      return 0;
  }

  return 1;
}