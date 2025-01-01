#include "unp.h"
#include "unpthread.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#define len_name 105
#define ROOM1 0
#define ROOM2 4
#define ROOM3 8
#define ROOM4 12
// int a;
int sep_room[4];
int room_status[4];
pthread_mutex_t mutex[4];

const char kick[200] = "You won\n";
int participant[16];
int id[16];
char name[16][len_name];
void *kahoot_game(void *);
const char start[200] = "start\n";
const char no_one[200] = "no\n";
const char nobody[200] = "empty\n";
int main(int argc, char **argv)
{
	for (int i = 0; i < 4; i++)
	{
		sep_room[i] = i * 4;
		for (int i = 0; i < 4; i++)
		{
			sep_room[i] = i * 4;
			pthread_mutex_init(&mutex[i], NULL);
		}
		room_status[i] = 0;
	}
	signal(SIGPIPE, SIG_IGN);

	int counter = 1; /* incremented by threads */
	srand(time(NULL));
	int listenfd, tmp, flag;
	char str[MAXLINE];
	const char waiting[200] = "waiting\n";
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	pthread_t tid[4];
	char how_many[MAXLINE] = "";
	for (int i = 0; i < 16; i++)
	{
		participant[i] = -1;
	}
	for (int i = 0; i < 4; i++)
	{
		int ttt = i;
		Pthread_create(&(tid[i]), NULL, &kahoot_game, (void *)(long)(ttt));
	}

	sleep(1);
	// Pthread_create(&tidB, NULL, &doit, NULL);
	for (;;)
	{
		// printf("OK\n");

		clilen = sizeof(cliaddr);
		tmp = Accept(listenfd, (SA *)&cliaddr, &clilen);
		// sprintf(name[i], "%s", str);
		printf("accepting new client\n");

		readline(tmp, str, sizeof(str));

		str[strlen(str) - 1] = '\0';
		flag = 0;
		sprintf(how_many, "%d\n", counter);
		if (writen(tmp, how_many, strlen(how_many)) <= 0)
		{
			continue;
		}
		if (writen(tmp, waiting, strlen(waiting)) <= 0)
		{
			continue;
		}
		// 檢查所有房間狀態
		for (int room = 0; room < 4; room++) {
			if (room_status[room] == 0) {
				Pthread_mutex_lock(&(mutex[room]));
				for (int i = sep_room[room]; i < sep_room[room] + 4; i++) {
					if (participant[i] == -1) {
						participant[i] = tmp;
						flag = 1;
						id[i] = counter;
						++counter;
						sprintf(name[i], "%s", str);
						break;
					}
				}
				Pthread_mutex_unlock(&(mutex[room]));
				if (flag == 1) break;
			}
		}
		if (flag == 0)
		{
			sprintf(how_many, "sorry\n");
			printf("sorry\n");
			if (writen(tmp, how_many, strlen(how_many)) <= 0)
			{
				continue;
			}
			close(tmp);
		}
	}
	// printf("OK\n");
}

#define MAX_QUESTIONS 5
#define MAX_ANSWER_OPTIONS 4

typedef struct {
	char question[MAXLINE];
	char options[MAX_ANSWER_OPTIONS][MAXLINE];
	int correct_answer;
} Question;

void load_questions(const char *filename, Question questions[], int *num_questions) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		perror("Failed to open questions file");
		exit(1);
	}

	char line[MAXLINE];
	*num_questions = 0;
	while (fgets(line, sizeof(line), file) && *num_questions < MAX_QUESTIONS) {
		Question *q = &questions[*num_questions];
		sscanf(line, " %[^\n]", q->question); // 使用 " %[^\n]" 來處理空格
		printf("Question: %s\n", q->question); // Debug print

		for (int i = 0; i < MAX_ANSWER_OPTIONS; i++) {
			fgets(line, sizeof(line), file);
			sscanf(line, " %[^\n]", q->options[i]); // 使用 " %[^\n]" 來處理空格
			printf("Option %d: %s\n", i + 1, q->options[i]); // Debug print
		}

		fgets(line, sizeof(line), file);
		sscanf(line, "%d", &q->correct_answer);
		printf("Correct Answer: %d\n", q->correct_answer); // Debug print

		(*num_questions)++;
	}

	fclose(file);
}

void *
kahoot_game(void *vptr)
{
	int room_num = (int)(long)vptr;
	const int ROOM = sep_room[room_num];

	fd_set fd;
	const char game_start[200] = "GameStart\n";
	const char question_start[200] = "QuestionStart\n";
	const char timeout_msg[200] = "Timeout\n";
	const char game_over[200] = "GameOver\n";
	char user_time[MAXLINE], mes[MAXLINE];
	int maxfdp1, people = 0, score[4] = {0}, num_ans, answer = 0, quit;
	double tmp_f;

	struct timeval tv, timeout;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	timeout.tv_sec = 10; // 設定答題時間為10秒
	timeout.tv_usec = 0;

	Question questions[MAX_QUESTIONS];
	int num_questions;
	load_questions("Questions/questions.txt", questions, &num_questions);

	while (1)
	{
	re:
		answer = 0;

		for (int i = 0; i < 4; i++)
		{
			score[i] = 0;
		}

		while (1)
		{
			FD_ZERO(&fd);
			maxfdp1 = -1;
			people = 0;
			Pthread_mutex_lock(&(mutex[room_num]));

			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (participant[i] != -1)
				{
					FD_SET(participant[i], &fd);
					maxfdp1 = max(participant[i], maxfdp1);
					++people;
				}
			}

			if (maxfdp1 == -1)
			{
				room_status[room_num] = 0;
				Pthread_mutex_unlock(&(mutex[room_num]));
				sleep(2);
			}
			else
			{
				tv.tv_sec = 0;
				tv.tv_usec = 0;
				Select(maxfdp1 + 1, &fd, NULL, NULL, &tv);
				for (int i = ROOM; i < ROOM + 4; i++)
				{
					if (participant[i] != -1)
					{
						if (FD_ISSET(participant[i], &fd))
						{
							participant[i] = -1;
							people--;
						}
						else
						{
							if (people != 4)
							{
								char how_many[MAXLINE] = "";
								for (int j = ROOM; j < ROOM + 4; j++) {
									if (participant[j] != -1) {
										strcat(how_many, name[j]);
										strcat(how_many, " ");
									}
								}
								char people_count[10];
								sprintf(people_count, "%d ", people);
								char message[MAXLINE] = "";
								strcat(message, people_count);
								strcat(message, how_many);
								strcat(message, "\n");

								if (writen(participant[i], message, strlen(message)) <= 0)
								{
									participant[i] = -1;
									people--;
								}
							}
						}
					}
				}

				if (people == 4)
				{
					const char four[200] = "4\n";
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						if (writen(participant[i], four, strlen(four)) <= 0)
						{
							participant[i] = -1;
							people--;
							sprintf(name[i], "-");
							id[i] = 0;
						}
					}

					if (people == 4)
					{
						room_status[room_num] = 1;
					}
					sleep(2);
					Pthread_mutex_unlock(&(mutex[room_num]));
					break;
				}
				else
				{
					room_status[room_num] = 0;
					Pthread_mutex_unlock(&(mutex[room_num]));
					sleep(2);
				}
			}
		}

		// 遊戲開始前倒數五秒
		for (int i = 5; i > 0; i--) {
			char countdown[50];
			sprintf(countdown, "Game starts in %d seconds\n", i);
			for (int j = ROOM; j < ROOM + 4; j++) {
				if (participant[j] != -1) {
					writen(participant[j], countdown, strlen(countdown));
				}
			}
			sleep(1);
		}

		// 傳送 "GameStart" 給所有參與者
		for (int i = ROOM; i < ROOM + 4; i++) {
			if (participant[i] != -1) {
				writen(participant[i], game_start, strlen(game_start));
				printf("Sent GameStart to participant %d\n", i);
			}
		}

		for (int q_index = 0; q_index < num_questions; q_index++)
		{
			int who = -1;

			quit = 0;

			Question *q = &questions[q_index];
			answer = q->correct_answer;

			// 每題題目開始前倒數三秒
			for (int i = 3; i > 0; i--) {
				char countdown[50];
				sprintf(countdown, "Question starts in %d seconds\n", i);
				for (int j = ROOM; j < ROOM + 4; j++) {
					if (participant[j] != -1) {
						writen(participant[j], countdown, strlen(countdown));
					}
				}
				sleep(1);
			}

			// 傳送 "QuestionStart" 給所有參與者
			for (int i = ROOM; i < ROOM + 4; i++) {
				if (participant[i] != -1) {
					writen(participant[i], question_start, strlen(question_start));
					printf("Sent QuestionStart to participant %d\n", i);
				}
			}

			// 傳送題目、選項和答案
			snprintf(mes, sizeof(mes), "%s\n1. %s\n2. %s\n3. %s\n4. %s\nAnswer: %d\n", q->question, q->options[0], q->options[1], q->options[2], q->options[3], q->correct_answer);
			printf("Question: %s\nOption1: %s\nOption2: %s\nOption3: %s\nOption4: %s\n",
       				q->question, q->options[0], q->options[1], q->options[2], q->options[3]);

			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (participant[i] != -1)
				{
					if (writen(participant[i], mes, strlen(mes)) <= 0)
					{
						participant[i] = -1;
					}
					else
					{
						maxfdp1 = max(maxfdp1, participant[i]);
					}
				}
			}

			// 傳送 "ClientAnswer" 標籤
			const char client_answer_tag[200] = "ClientAnswer\n";
			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (participant[i] != -1)
				{
					if (writen(participant[i], client_answer_tag, strlen(client_answer_tag)) <= 0)
					{
						participant[i] = -1;
					}
					else
					{
						printf("Sent ClientAnswer to participant %d\n", i);
					}
				}
			}

			FD_ZERO(&fd);
			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (participant[i] != -1)
				{
					FD_SET(participant[i], &fd);
				}
			}

			num_ans = 0;
			num_ans = select(maxfdp1 + 1, &fd, NULL, NULL, &timeout);

			// 傳送"Timeout"超時消息給所有client
			for (int i = ROOM; i < ROOM + 4; i++) {
				if (participant[i] != -1) {
					writen(participant[i], timeout_msg, strlen(timeout_msg));
					printf("Sent Timeout to participant %d\n", i);
				}
			}

			if (num_ans != 0)
			{
				double time = 100000;
				who = -1;
				for (int i = ROOM; i < ROOM + 4; i++)
				{
					if (participant[i] != -1 && FD_ISSET(participant[i], &fd))
					{
						if (readline(participant[i], user_time, MAXLINE) <= 0)
						{
							participant[i] = -1;
						}
						else
						{
							int user_answer;
							sscanf(user_time, "%d", &user_answer);
							if (user_answer == answer)
							{
								sscanf(user_time, "%lf", &tmp_f);
								if (tmp_f < time)
								{
									time = tmp_f;
									who = i;
								}
							}
							printf("accept: id:%d time%s", id[i], user_time);
						}
					}
				}

				if (who != -1)
				{
					score[who - ROOM] += 2; // two points for the fastest correct answer
					for (int i = ROOM; i < ROOM + 4; i++)
					{
						if (i != who && participant[i] != -1)
						{
							int user_answer;
							sscanf(user_time, "%d", &user_answer);
							if (user_answer == answer)
							{
								score[i - ROOM]++; // one point for correct answer
							}
						}
					}
				}
			}

			quit = 0;
			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (participant[i] == -1)
				{
					score[i - ROOM] = 0;
					id[i] = 0;
					sprintf(name[i], "-");
					++quit;
				}
			}

			int flag = 0;
			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (strcmp(name[i], "-") == 0)
				{
					flag++;
				}
			}
			if (flag >= 3)
			{
				char st[MAXLINE];
				sprintf(st, "1\n%s %s %s %s %d %d %d %d %d %d %d %d\n", name[ROOM], name[ROOM + 1], name[ROOM + 2], name[ROOM + 3], id[ROOM], id[ROOM + 1], id[ROOM + 2], id[ROOM + 3], score[0], score[1], score[2], score[3]);
				for (int i = ROOM; i < ROOM + 4; i++)
				{
					if (participant[i] != -1)
					{
						if (writen(participant[i], st, strlen(st)) <= 0)
						{
							;
						}
						close(participant[i]);
					}
				}
				for (int i = ROOM; i < ROOM + 4; i++)
				{
					participant[i] = -1;
					id[i] = 0;
					sprintf(name[i], "-");
					score[i - ROOM] = 0;
				}
				room_status[room_num] = 0;
				Pthread_mutex_unlock(&(mutex[room_num]));
				goto re;
			}

			// 傳送 "Info" 標籤
			const char info_tag[200] = "Info\n";
			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (participant[i] != -1)
				{
					if (writen(participant[i], info_tag, strlen(info_tag)) <= 0)
					{
						participant[i] = -1;
					}
					else
					{
						printf("Sent Info to participant %d\n", i);
					}
				}
			}

			// 保留數據的輸出給客戶端
			char st[MAXLINE];
			sprintf(st, "%s %s %s %s %d %d %d %d %d %d %d %d\n", name[ROOM], name[ROOM + 1], name[ROOM + 2], name[ROOM + 3], id[ROOM], id[ROOM + 1], id[ROOM + 2], id[ROOM + 3], score[0], score[1], score[2], score[3]);
			int people_flag = 0;
			printf("%s\n", st);
			for (int i = ROOM; i < ROOM + 4; i++)
			{
				if (participant[i] == -1)
				{
					people_flag = 1;
				}
				if (participant[i] != -1)
				{
					if (writen(participant[i], st, strlen(st)) <= 0)
					{
						participant[i] = -1;
					}
				}
			}
			if (people_flag == 1)
			{
				room_status[room_num] = 0;
			}
			else
			{
				room_status[room_num] = 1;
			}

			// 停留五秒再到下一題
			sleep(5);
		}

		// 五題結束後傳送 "FinalInfo" 標籤和最終數據
		const char final_info_tag[200] = "FinalInfo\n";
		for (int i = ROOM; i < ROOM + 4; i++)
		{
			if (participant[i] != -1)
			{
				if (writen(participant[i], final_info_tag, strlen(final_info_tag)) <= 0)
				{
					participant[i] = -1;
				}
				else
				{
					printf("Sent FinalInfo to participant %d\n", i);
				}
			}
		}

		// 傳送最終數據
		char st[MAXLINE];
		sprintf(st, "%s %s %s %s %d %d %d %d %d %d %d %d\n", name[ROOM], name[ROOM + 1], name[ROOM + 2], name[ROOM + 3], id[ROOM], id[ROOM + 1], id[ROOM + 2], id[ROOM + 3], score[0], score[1], score[2], score[3]);
		int people_flag = 0;
		printf("%s\n", st);
		for (int i = ROOM; i < ROOM + 4; i++)
		{
			if (participant[i] == -1)
			{
				people_flag = 1;
			}
			if (participant[i] != -1)
			{
				if (writen(participant[i], st, strlen(st)) <= 0)
				{
					participant[i] = -1;
				}
			}
		}

		// 等待10秒後伺服器主動切斷連線
		sleep(10);

		// 傳送 "GameOver" 標籤
		for (int i = ROOM; i < ROOM + 4; i++)
		{
			if (participant[i] != -1)
			{
				if (writen(participant[i], game_over, strlen(game_over)) <= 0)
				{
					participant[i] = -1;
				}
				else
				{
					printf("Sent GameOver to participant %d\n", i);
				}
			}
		}


		for (int i = ROOM; i < ROOM + 4; i++)
		{
			if (participant[i] != -1)
			{
				close(participant[i]);
				participant[i] = -1;
			}
		}

		room_status[room_num] = 0;
		Pthread_mutex_unlock(&(mutex[room_num]));

		return (NULL);
	}
}