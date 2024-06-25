#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define DOCKS 3
#define TUGS 5

int available_docks = DOCKS;
int available_tugs = TUGS;

pthread_mutex_t port_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t dock_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t tug_cond = PTHREAD_COND_INITIALIZER;

void enter(int id, int tugs_required)
{
    pthread_mutex_lock(&port_mutex);

    while (available_docks == 0 || available_tugs < tugs_required)
    {
        if (available_docks == 0)
            pthread_cond_wait(&dock_cond, &port_mutex);

        if (available_tugs < tugs_required)
            pthread_cond_wait(&tug_cond, &port_mutex);
    }

    available_docks--;
    available_tugs -= tugs_required;
    printf("Statek numer: %d wpływa do porty przy pomocy %d holownikw.\n", id, tugs_required);

    pthread_mutex_unlock(&port_mutex);
}

void leave(int id, int tugs_required)
{
    pthread_mutex_lock(&port_mutex);

    available_docks++;
    available_tugs += tugs_required;
    printf("statek numer %d opuszcza port, uwalniając %d holownikow.\n", id, tugs_required);

    pthread_cond_signal(&dock_cond);
    pthread_cond_signal(&tug_cond);

    pthread_mutex_unlock(&port_mutex);
}

void *ship(void *arg)
{
    int ship_id = *((int *)arg); // kocham rzutowanie typow
    int tugs_needed = rand() % 3 + 1;
    enter(ship_id, tugs_needed);
    sleep(rand() % 5 + 1);
    leave(ship_id, tugs_needed);
    return NULL;
}

int main()
{
    srand(time(NULL));

    pthread_t ships[5];
    int ship_ids[5] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++)
        pthread_create(&ships[i], NULL, ship, &ship_ids[i]);

    for (int i = 0; i < 5; i++)
        pthread_join(ships[i], NULL);

    pthread_mutex_destroy(&port_mutex);
    pthread_cond_destroy(&dock_cond);
    pthread_cond_destroy(&tug_cond);

    return 0;
}
