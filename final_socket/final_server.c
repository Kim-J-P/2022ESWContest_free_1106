
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <signal.h>


#define BUF_SIZE 100
#define MAX_CLNT 32
#define ID_SIZE 10
#define ARR_CNT 5
char msg[100];
#define NAME_SIZE 10
char name_msg[100];

#define DEBUG
typedef struct {
	char fd;
	char* from;
	char* to;
	char* msg;
	int len;
}MSG_INFO;   //메세지 정보 관련 구조체

typedef struct {
	int index;
	int fd;
	char ip[20];
	char id[ID_SIZE];
	char pw[ID_SIZE];
}CLIENT_INFO;  //클라이언트 정보 관련 구조체


 //함수 선언
void* clnt_connection(void* arg);
void send_msg(MSG_INFO* msg_info, CLIENT_INFO* first_client_info);
void error_handling(char* msg);
void log_file(char* msgstr);
void *send_to_client(void *arg);

int clnt_cnt = 0;
pthread_mutex_t mutx;  //스레드 내에 변수를 선언할 때 사용, 그냥 형 이름인듯

int main(int argc, char* argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr; //주소체계가 IPv4인 경우에 사용하는 구조체
	int clnt_adr_sz;
	int sock_option = 1;
	pthread_t t_id[MAX_CLNT] = { 0 }; //동작중인 pthread의 식별자를 리턴함
	pthread_t send[MAX_CLNT] = {0};
	//pthread_t = 스레드 식별값 리턴
	int str_len = 0;
	int i = 0;
	char idpasswd[(ID_SIZE * 2) + 3];
	char* pToken;
	char* pArray[ARR_CNT] = { 0 };
	char msg[BUF_SIZE];

	

	if (argc != 2) { //포트번호를 입력하지 않은 경우
		//iot_server 실행 시   ./iot_server 5000 으로 5000번 포트로 서버를 여는데 이때 argc = 2로 화이트 스페이스로 갯수가 구분됨
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	//+
	FILE* idFd = fopen("idpasswd.txt", "r");//id, pw로 정한 데이터들이 있는 txt문서를 읽기모드롤 읽어오겠다
	if (idFd == NULL)
	{
		perror("fopen() ");
		exit(2);
	}
	char id[ID_SIZE];
	char pw[ID_SIZE];
	CLIENT_INFO* client_info = (CLIENT_INFO*)calloc(sizeof(CLIENT_INFO), MAX_CLNT);
	//클라이언트 정보를 받아오려고 메모리 동적할당
	//calloc함수 : sizeof(CLIENT_INFO)*MAX_CLNT 만큼의 공간 할당함
	int ret;
	do {
		ret = fscanf(idFd, "%s %s", id, pw);
		if (ret <= 0)
			break;
		client_info[i].fd = -1;
		//구조체 변수 중 fd에 -1 대입
		strcpy(client_info[i].id, id); //txt파일로부터 읽어온 값 구조체에 저장
		strcpy(client_info[i].pw, pw);

		i++; //main문 시작위치에 0으로 초기화되어있음
		if (i > MAX_CLNT) //지정한 갯수(32)개보다 많이 만들어지면
		{
			printf("error client_info pull(MAX:%d)\n", MAX_CLNT);
			exit(2); //txt파일에 id를 32개 정의했음, 그 숫자보다 더 많이 client_info 구조체를 만들려
			//하는 경우에는 exit실행
		}
	} while (1);
	fclose(idFd);  //do-while문에서 exit되면 열었던 txt파일 닫음
	//-

	fputs("IoT Server Start!!\n", stdout);

	if (pthread_mutex_init(&mutx, NULL))
		error_handling("mutex init error");

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&sock_option, sizeof(sock_option));
	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");

	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	while (1) {
		clnt_adr_sz = sizeof(clnt_adr); //클라이언트 사이즈 확보
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		//accept의 리턴값은 연결을 받아들인 새로운 소켓 디스크립터
		if (clnt_cnt >= MAX_CLNT)
		{
			printf("socket full\n");
			shutdown(clnt_sock, SHUT_WR);
			continue;
		}
		else if (clnt_sock < 0)
		{
			perror("accept()");
			continue;
		}

		str_len = read(clnt_sock, idpasswd, sizeof(idpasswd));
		idpasswd[str_len] = '\0';

		if (str_len > 0)
		{
			i = 0;
			pToken = strtok(idpasswd, "[:]");//strtok를 통해 [,:,]로 문자 자름

			while (pToken != NULL)
			{
				pArray[i] = pToken;
				if (i++ >= ARR_CNT)// 위에서 대기열 크기를 5개만 만들었음
					break;	//5개 이상 대기?중일 경우 프로그램 종료
				pToken = strtok(NULL, "[:]");
			}

			for (i = 0; i < MAX_CLNT; i++)
			{
				if (!strcmp(client_info[i].id, pArray[0]))
				{
					if (client_info[i].fd != -1)
					{
						sprintf(msg, "[%s] Already logged!\n", pArray[0]);
						write(clnt_sock, msg, strlen(msg));
						log_file(msg);
						shutdown(clnt_sock, SHUT_WR);
#if 0   //for MCU
						shutdown(client_info[i].fd, SHUT_WR);
						pthread_mutex_lock(&mutx);
						client_info[i].fd = -1;
						pthread_mutex_unlock(&mutx);
#endif  
						break;
					}
					if (!strcmp(client_info[i].pw, pArray[1]))
					{
						strcpy(client_info[i].ip, inet_ntoa(clnt_adr.sin_addr));
						pthread_mutex_lock(&mutx);
						client_info[i].index = i;
						client_info[i].fd = clnt_sock;
						clnt_cnt++;
						pthread_mutex_unlock(&mutx);
						sprintf(msg, "[%s] New connected! (ip:%s,fd:%d,sockcnt:%d)\n", pArray[0], inet_ntoa(clnt_adr.sin_addr), clnt_sock, clnt_cnt);
						log_file(msg);
						write(clnt_sock, msg, strlen(msg));

						pthread_create(t_id + i, NULL, clnt_connection, (void*)(client_info + i));
						pthread_create(send + i, NULL, send_to_client, (void*)(client_info + i));
						pthread_detach(t_id[i]);
						pthread_detach(send[i]);
						break;
					}
				}
			}
			if (i == MAX_CLNT)
			{
				sprintf(msg, "[%s] Authentication Error!\n", pArray[0]);
				write(clnt_sock, msg, strlen(msg));
				log_file(msg);
				shutdown(clnt_sock, SHUT_WR);
			}
		}
		else
			shutdown(clnt_sock, SHUT_WR);

	}
	return 0;
}

void* clnt_connection(void* arg)
{
	CLIENT_INFO* client_info = (CLIENT_INFO*)arg;
	int str_len = 0;
	int index = client_info->index;
	char msg[BUF_SIZE];
	char to_msg[MAX_CLNT * ID_SIZE + 1];
	int i = 0;
	char* pToken;
	char* pArray[ARR_CNT] = { 0 };
	char strBuff[130] = { 0 };
	int single_send;
	MSG_INFO msg_info;
	CLIENT_INFO* first_client_info;
	char clid[10];

	first_client_info = (CLIENT_INFO*)((void*)client_info - (void*)(sizeof(CLIENT_INFO) * index));
	while (1)
	{
		memset(msg, 0x0, sizeof(msg));
		str_len = read(client_info->fd, msg, sizeof(msg) - 1);
		if (str_len <= 0)
			break;

		msg[str_len] = '\0';
		pToken = strtok(msg, "[:]");
		i = 0;
		while (pToken != NULL)
		{
			pArray[i] = pToken;
			if (i++ >= ARR_CNT)
				break;
			pToken = strtok(NULL, "[:]");
		}

		msg_info.fd = client_info->fd;
		msg_info.from = client_info->id;
		msg_info.to = pArray[0];
		sprintf(to_msg, "[%s]%s", msg_info.from, pArray[1]);
		msg_info.msg = to_msg;
		msg_info.len = strlen(to_msg);
		single_send = client_info->fd;
		strcpy(clid, client_info->id);

		sprintf(strBuff, "msg : [%s->%s] %s", msg_info.from,msg_info.to, pArray[1]);
		log_file(strBuff);
		//send_msg(&msg_info, first_client_info);
		//send_to_client(&client_info);
	}

	close(client_info->fd);

	sprintf(strBuff, "Disconnect ID:%s (ip:%s,fd:%d,sockcnt:%d)\n", client_info->id, client_info->ip, client_info->fd, clnt_cnt - 1);
	log_file(strBuff);

	pthread_mutex_lock(&mutx);
	clnt_cnt--;
	client_info->fd = -1;
	pthread_mutex_unlock(&mutx);

	return 0;
}


void* send_to_client(void* arg) {
	
	CLIENT_INFO* clnt = (CLIENT_INFO *)arg;
	int *sock = &(clnt->fd);
	int str_len;
	int ret;
	fd_set initset, newset;
	struct timeval tv;
	char name_msg[150];
	char msg[100];

	//t index = client_info->index;
	//ar to_msg[50];
	char* pArray[5]={0};
	char* pToken;
	//ar strBuff[130] = {0};
	//G_INFO msg_info;
	//IENT_INFO* first_client_info;
	//ar clid[10];
	//rst_client_info = (CLIENT_INFO*)((void)client_info - (void*)(sizeof(CLIENT_INFO)* index));


	FD_ZERO(&initset);
	FD_SET(STDIN_FILENO, &initset);

	int count = 0;
	while(1){
		memset(msg, 0, sizeof(msg));
		memset(name_msg, 0, sizeof(name_msg));	
		//name_msg[0] = '\0';
		tv.tv_sec=1;
		tv.tv_usec = 0;
		newset = initset;
		ret=select(STDIN_FILENO, &newset, NULL, NULL, &tv);
		if(FD_ISSET(STDIN_FILENO, &newset)){
			fgets(msg, 100, stdin);
			if(!strncmp(msg, "quit\n", 5)){
				*sock = -1;
				return NULL;
			}
			else {
				strcat(name_msg, msg);
			}
			
			
					
			if(atoi(&msg[0])<4)
				printf("retry\n");
			else if(write(atoi(&msg[0]), name_msg, strlen(name_msg))<=0){
						*sock =-1;
						return NULL;
					}
						
			
				
		}
		
	}

	if(ret == 0){
		if(*sock == -1)
			return NULL;
	}
	
}




void error_handling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void log_file(char* msgstr)
{
	fputs(msgstr, stdout);
}
