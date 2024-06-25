#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>

#define DOCKS 3
#define TUGS 5

sem_t dock_sem;
sem_t tug_sem;

void enter(int ship_id, int tugs_needed)
{
    printf("Statek o numerze %d oczekuje na %d holownikow aby wplynac do portu.\n", ship_id, tugs_needed);
    for (int i = 0; i < tugs_needed; i++)
        sem_wait(&tug_sem);
    sem_wait(&dock_sem);
    printf("Statekl o numerze %d wplywa do statku i jest cumowany przez %d holownikow.\n", ship_id, tugs_needed);
}

void leave(int ship_id, int tugs_needed)
{
    printf("Statek o numerze %d opuszcza port, uwalniając %d holownikow.\n", ship_id, tugs_needed);
    for (int i = 0; i < tugs_needed; i++)
        sem_post(&tug_sem);
    sem_post(&dock_sem);
    printf("Statek o numerze %d opuścił port.\n", ship_id);
}

void ship(int ship_id, int tugs_needed)
{
    enter(ship_id, tugs_needed);
    sleep(rand() % 5 + 1);
    leave(ship_id, tugs_needed);
    exit(0);
}

int main()
{
    srand(time(NULL));

    sem_init(&dock_sem, 1, DOCKS);
    sem_init(&tug_sem, 1, TUGS);

    int ship_ids[] = {1, 2, 3, 4, 5};
    int tugs_needed[] = {2, 1, 3, 2, 1};

    for (int i = 0; i < 5; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
            ship(ship_ids[i], tugs_needed[i]);
    }

    for (int i = 0; i < 5; i++)
        wait(NULL);

    sem_destroy(&dock_sem);
    sem_destroy(&tug_sem);

    return 0;
}
