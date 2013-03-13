#ifndef WORKPOOL_HPP
#define WORKPOOL_HPP

#include "a4.hpp" // need this to get pthread_mutex_t
#include <queue>

using namespace std;

class WorkPool {
public:
    WorkPool();
    void addJob(int jobId);
    unsigned int numJobsLeft();
    bool getNextJob(int &jobId);
private:
    queue<int> jobIds;
    pthread_mutex_t lock;
};

#endif