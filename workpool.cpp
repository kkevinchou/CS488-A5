#include "workpool.hpp"

WorkPool::WorkPool() {
    pthread_mutex_init(&lock, NULL);
}

void WorkPool::addJob(int jobId) {
    pthread_mutex_lock(&lock);
        jobIds.push(jobId);
    pthread_mutex_unlock(&lock);
}

unsigned int WorkPool::numJobsLeft() {
    pthread_mutex_lock(&lock);
        unsigned int numJobs = jobIds.size();
    pthread_mutex_unlock(&lock);

    return numJobs;
}

bool WorkPool::getNextJob(int &jobId) {
    bool result;

    pthread_mutex_lock(&lock);
        if (jobIds.size() == 0) {
            result = false;
        } else {
            jobId = jobIds.front();
            jobIds.pop();
            result = true;
        }
    pthread_mutex_unlock(&lock);

    return result;
}