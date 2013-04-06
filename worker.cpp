#include "worker.hpp"
#include "a4.hpp"

extern unsigned short port;
extern bool superSampling;
extern int sampleDimension;

void Worker::setParams(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const list<Light*>& lights) {
    if (this->r != NULL) {
        delete this->r;
    }

    this->width = width;
    this->height = height;
    this->r = new Renderer(root, filename, width, height, eye, view, up, fov, ambient, lights);
}

static void *testMethod(void *args) {
    struct worker_thread_args *worker_args = (struct worker_thread_args *)args;

    pthread_mutex_t *sendLock = worker_args->sendLock;
    WorkPool *workPool = worker_args->workPool;
    Renderer *r = worker_args->renderer;
    int height = worker_args->height;
    int coordSocketFd = worker_args->coordSocketFd;

    int y = 0;
    while (true) {
        int x;

        if (!workPool->getNextJob(x)) {
            break;
            return NULL;
        }

        for (y = 0; y < height; y++) {
            vector<double> colour = r->render(x, y, superSampling, sampleDimension);

            int status = 0;

            pthread_mutex_lock(sendLock);
                status = sendDouble(coordSocketFd, (double)x);
                if (status < 0) {
                    goto cleanup;
                }
                status = sendDouble(coordSocketFd, (double)y);
                if (status < 0) {
                    goto cleanup;
                }

                for (unsigned int k = 0; k < colour.size(); k++) {
                    int status = sendDouble(coordSocketFd, colour[k]);
                    if (status < 0) {
                        goto cleanup;
                    }
                }
            pthread_mutex_unlock(sendLock);
        }
    }

    cleanup:
    pthread_mutex_unlock(sendLock);
    return NULL;
}

int Worker::handleRequest(queue<double> &inData) {
    double d;
    int status = readDouble(coordSocketFd, d);

    if (status == 0) {
        inData.push(d);
    }

    if (inData.size() >= 2) {
        int column = (int)inData.front();
        inData.pop();
        int numWorkers = (int)inData.front();
        inData.pop();

        WorkPool workPool;
        for (int i = column; i < this->width; i += numWorkers) {
            workPool.addJob(i);
        }

        pthread_mutex_t lock;
        pthread_mutex_init(&lock, NULL);

        struct worker_thread_args args;
        args.sendLock = &lock;
        args.workPool = &workPool;
        args.renderer = this->r;
        args.height = this->height;
        args.coordSocketFd = coordSocketFd;

        vector<pthread_t *> workerThreads;

        int numThreads = numCPUs;
        for (int i = 0; i < numThreads; i++) {
            pthread_t *thread = new pthread_t();
            pthread_create(thread, NULL, &testMethod, &args);
            workerThreads.push_back(thread);
        }
        for (int i = 0; i < numThreads; i++) {
            pthread_t *thread = workerThreads[i];
            pthread_join(*thread, NULL);
            delete thread;
        }

        return 1;
    }
    return status;
}

void Worker::wait() {

    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    FD_SET(localSocketFd, &master_set);
    FD_SET(coordSocketFd, &master_set);

    queue<double> inData;

    bool computationDone = false;

    while (!computationDone) {
        memcpy(&working_set, &master_set, sizeof(master_set));
        int selectResult = select(coordSocketFd + 1, &working_set, NULL, NULL, NULL);

        if (selectResult < 0) {
            cerr << "WORKER - SELECT FAILED" << endl;
            return;
        }

        if (FD_ISSET(coordSocketFd, &working_set)) {
            int handleResult = handleRequest(inData);
            if (handleResult == 1) {
                computationDone = true;
            }
            if (handleResult < 0) {
                FD_CLR(coordSocketFd, &master_set);
                close(coordSocketFd);
            }
        }
    }
}

void Worker::accept() {
    if (firstRun) {
        localSocketFd = socket(AF_INET, SOCK_STREAM, 0);
        listenOnSocket(localSocketFd, port);
        firstRun = false;

        numCPUs = sysconf( _SC_NPROCESSORS_ONLN );
        cerr << "NUMCPUS: " << numCPUs << endl;

        printServerSettings(localSocketFd);
    }

    coordSocketFd = acceptConnection(localSocketFd);
}

Worker::Worker() {
    firstRun = true;
    this->r = NULL;
}