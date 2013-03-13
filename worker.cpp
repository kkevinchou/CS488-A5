#include "worker.hpp"
#include "a4.hpp"

extern unsigned short port;

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
            vector<double> colour = r->render(x, y, true, 4);

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

    if (inData.size() >= 4) {
        int column = (int)inData.front();
        inData.pop();
        int width = (int)inData.front();
        inData.pop();
        int height = (int)inData.front();
        inData.pop();
        int numWorkers = (int)inData.front();
        inData.pop();

        WorkPool workPool;
        for (int i = column; i < width; i += numWorkers) {
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

        pthread_t t1;
        pthread_create(&t1, NULL, &testMethod, &args);
        pthread_t t2;
        pthread_create(&t2, NULL, &testMethod, &args);
        pthread_t t3;
        pthread_create(&t3, NULL, &testMethod, &args);
        pthread_t t4;
        pthread_create(&t4, NULL, &testMethod, &args);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        pthread_join(t3, NULL);
        pthread_join(t4, NULL);

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
    }

    printServerSettings(localSocketFd);
    coordSocketFd = acceptConnection(localSocketFd);
}

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

Worker::Worker() {
    firstRun = true;
    this->r = NULL;
}