
#pragma once 

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>


typedef struct {
    int tfd;
    struct itimerspec ts;
    size_t id;
    char* name;
    void (*func)(void);
} ev_timer_t;

typedef struct {
    ev_timer_t* items;
    size_t count;
    size_t capacity;
} event_array_t;


void event_init();

int event_wait(void);

void event_close();

int event_get_id(int tfd);

ev_timer_t* event_add_timer(int id, char* name, size_t intervall);

void event_array_init(event_array_t* array);



