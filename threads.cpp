#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <cstdlib>

#define DOCKS 3
#define TUGS 5

int available_docks = DOCKS;
int available_tugs = TUGS;

using namespace std;

mutex port_mutex;
condition_variable dock_cond;
condition_variable tug_cond;

void enter(int id, int tugs_needed)
{
    unique_lock<mutex> lock(port_mutex);

    dock_cond.wait(lock, []
                   { return available_docks > 0; });
    tug_cond.wait(lock, [tugs_needed]
                  { return available_tugs >= tugs_needed; });

    available_docks--;
    available_tugs -= tugs_needed;
    cout << "Statek numer " << id << " wplywa do portu z " << tugs_needed << " holownikami.\n";
}

void leave(int id, int tugs_needed)
{
    lock_guard<mutex> lock(port_mutex);

    available_docks++;
    available_tugs += tugs_needed;
    cout << "statek nuimer " << id << " opuszcza port i uwalnia " << tugs_needed << " holownuiki.\n";

    dock_cond.notify_all();
    tug_cond.notify_all();
}

void ship(int id)
{
    int tugs_needed = rand() % 3 + 1;

    enter(id, tugs_needed);
    this_thread::sleep_for(chrono::seconds(rand() % 5 + 1));
    leave(id, tugs_needed);
}

int main()
{
    srand(time(NULL));

    vector<thread> ships;
    int ship_ids[5] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++)
        ships.emplace_back(ship, ship_ids[i]);

    for (auto &th : ships)
        th.join();

    return 0;
}
