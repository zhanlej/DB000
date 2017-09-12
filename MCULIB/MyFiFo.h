#ifndef MYFIFO
#define MYFIFO

#define recvbufLen 1024
#define sendbufLen 1024

struct Fifo {
	char* head;
	char* write;
	char* read;
	char* bound;
};

extern struct Fifo recv_fifo1;
extern struct Fifo recv_fifo3;

extern void Fifo_Init(struct Fifo* f, int length_, char* head_);
extern char Fifo_canPush(struct Fifo* f);
extern char Fifo_canPop(struct Fifo* f);
extern char Fifo_Pop(struct Fifo* f);
extern void Fifo_Push(struct Fifo* f, char c);

extern void Fifo_All_Initialize(void);

extern void Fifo_PushList(struct Fifo* f, const char* s, int length);

int fifo1readdata(unsigned char* s, int maxlen);
int fifo3readdata(unsigned char* s, int maxlen);

#endif
