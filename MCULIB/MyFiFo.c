#include "MyFifo.h"

struct Fifo recv_fifo1;
struct Fifo recv_fifo2;
struct Fifo recv_fifo3;
struct Fifo recv_fifo4;
struct Fifo recv_fifo5;

struct Fifo send_fifo1;
struct Fifo send_fifo2;
struct Fifo send_fifo3;
struct Fifo send_fifo4;
struct Fifo send_fifo5;

char inner_recvbuffer1[1];
char inner_recvbuffer3[1];

void Fifo_Init(struct Fifo* f, int length_, char* head_) {
	(f->read) = (f->write) = (f->head) = head_;
	(f->bound) = head_ + length_;
}
char Fifo_canPush(struct Fifo* f) {
	char* next = (f->write) + 1;
	return next == (f->bound) ? ((f->read) != (f->head)) : ((f->read) != next);
}
char Fifo_canPop(struct Fifo* f) {
	return (f->read) != (f->write);
}
char Fifo_Pop(struct Fifo* f) {
	char x = *(f->read);
	++(f->read);
	if ((f->read) == (f->bound)) {
		(f->read) = (f->head);
	}
	return x;
}
void Fifo_Push(struct Fifo* f, char c) {
	*(f->write) = c;
	++(f->write);
	if ((f->write) == (f->bound)) {
		(f->write) = (f->head);
	}
}

void Fifo_PushList(struct Fifo* f, const char* s, int length) {
	int i;
	for (i=0; i<length && Fifo_canPush(f); ++i, ++s) {
		Fifo_Push(f, *s);
	}
}

void Fifo_All_Initialize(void) {
	Fifo_Init(&recv_fifo1, recvbufLen, inner_recvbuffer1); 
	Fifo_Init(&recv_fifo3, recvbufLen, inner_recvbuffer3); 
}

int fifo1readdata(unsigned char* s, int maxlen)
{
  int i;
  for (i = 0; i < maxlen; ++i)
  {
    if (Fifo_canPop(&recv_fifo1)) s[i] = Fifo_Pop(&recv_fifo1);
    else return i;
  }
  return maxlen;
}

int fifo3readdata(unsigned char* s, int maxlen)
{
  int i;
  for (i = 0; i < maxlen; ++i)
  {
    if (Fifo_canPop(&recv_fifo3)) s[i] = Fifo_Pop(&recv_fifo3);
    else return i;
  }
  return maxlen;
}


