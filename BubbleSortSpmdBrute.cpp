#include<cstdlib>
#include<iostream>
#include<string>
#include<vector>
#include<ctime>
#include<numeric>
#include<cmath>
#include<sstream>
#include<thread>
#include<chrono>
#include<ctime>
#include<mutex>
#include<stack>
#include <condition_variable>


bool flag[65];
int index[65];
std::mutex mu[65], mu2[65];
std::condition_variable myconds[65];
void Sync_with_previous(int, int);
void Sync_with_next(int, int);
std::thread threads[65];
int arr[100000000], arr2[100000000], arr3[100000000], n = 40000, numThread = 1;
bool isSortedd[65];
std::mutex mutexes[65], gl_mu;


int getRandom(int);
void init();
void initCond();
void printArr();
void cpyArr(int [], int []);
void bubbleSort(int, int, int);
void sortThread(int, int, int);
void merge(int);
void testWithThreads();
void lockAll();
bool isSortedAllThreads();
bool isSorted();

int main(int argc, char *argv[])
{
    init();
    cpyArr(arr, arr3);
    n = atoi(argv[2]);
    lockAll();
    do
    {
        //printArr();
        testWithThreads();
        //printArr();
        numThread ++;
        cpyArr(arr3, arr);
    }while(numThread <= atoi(argv[1]));
    return 0;
}


int getRandom(int max)
{
    return rand() % max;
}

void init()
{
    srand(time(NULL));
    for(int i = 0; i < n; ++i)
    {
        arr[i] = getRandom(10000000);
    }
    for(int i = 0; i < numThread; ++i)
    {
        isSortedd[i] = false;
    }
}

void initCond()
{
    for(int i = 0; i < numThread; ++i)
    {
        index[i] = 0;
        flag[i] = false;
    }
}

void printArr()
{
    for(int i = 0; i < n; ++i)
    {
        std::cout << arr[i] << std::endl;
    }
    std::cout << std::endl;
}


void cpyArr(int arr[], int arr3[])
{
    for(int i = 0; i < n; ++i)
    {
        arr3[i] = arr[i];
        arr2[i] = 0;
    }
}

void lockAll()
{
   for(int i = 1; i < numThread; ++i)
    {
        mutexes[i].lock();
    }
}

void testWithThreads()
{
    initCond();
    auto timeVal = std::chrono::steady_clock::now();
    sortThread(0, n, numThread);
    //merge(numThread);
    std::chrono::duration<double> timePassed = std::chrono::steady_clock::now() - timeVal;
    std::cout << numThread << " threads: " <<
            timePassed.count()
            << " total: "
            << timePassed.count()*numThread
            << std::endl;
    std::cout << "Is it sorted: " << isSorted() << std::endl;
}

bool isSorted()
{
    for(int i = 0; i < n-1; ++i)
    {
        if(arr[i] > arr[i+1])
        {
            //std::cout << "Sorting unsuccessful!" << std::endl;
            return false;
        }
    }
    //std::cout << "Sorting successful!" << std::endl;
    return true;
}

bool isSortedAllThreads()
{
    for(int i = 0; i < numThread; ++i)
    {
        if(!isSortedd[i])
        {
            return false;
        }
    }
    return true;
}

void bubbleSort(int start, int end, int threadIdx)
{
    bool isSorted;
    int iteration = 0;
    if(start == 0)
    {
        start = 1;
    }
    for(int j = 0; j < 5*n/4; ++j)
    {
       // gl_mu.lock();
       // std::cout << "Thread " << threadIdx << " iteration " << iteration << std::endl;
        //gl_mu.unlock();
        /*
        if(threadIdx == 0)
        {
            gl_mu.lock();
            for(int i = 0; i < numThread; ++i)
            {
                isSortedd[i] = false;
            }
            gl_mu.unlock();
        }
        */
        Sync_with_previous(threadIdx, j);
        //std::cout << "Thread " << threadIdx << " running\n";
        isSorted = true;
        for(int i = start-1; i < end && i < n-1; ++i)
        {
            if(arr[i] > arr[i+1])
            {
                std::swap(arr[i], arr[i+1]);
                isSorted = false;
            }
        }
        //printArr();
        /*
        if(isSorted)
        {
            gl_mu.lock();
            isSortedd[threadIdx] = true;
            gl_mu.unlock();
        }
        else
        {
            gl_mu.lock();
            isSortedd[threadIdx] = false;
            gl_mu.unlock();
        }
        */

       Sync_with_next(threadIdx, j);

    }
}

void sortThread(int start, int end, int numThread)
{
    int split = (end-start)/numThread;
    int newStart = start, newEnd = newStart+split;

    for(int i = 0; i < numThread; ++i)
    {
        if(i == numThread-1)
        {
            newEnd = end;
        }
        threads[i] = std::thread(bubbleSort, newStart, newEnd, i);
        newStart += split;
        newEnd += split;
    }

    for(int i = 0; i < numThread; ++i)
    {
        threads[i].join();
    }
}

void Sync_with_previous(int id, int i)
{
    if (id>0) {
        //pthread_mutex.lock(&mutex[id]);
        if (index[id-1]<i)
        {
            std::unique_lock<std::mutex> lock(mu[id]);
            myconds[id].wait_for( lock,
                        std::chrono::seconds(100),
                        [id]() {  return flag[id]; } );
        }
        //pthread_cond_wait(&cond[id],&mutex[id]);
        //pthread_mutex_unlock(&mutex[id]);
   }
}

void Sync_with_next(int id, int i)
{
    if (id < n-1)
    {
        //pthread_mutex_lock(&mutex[id+1]);
        index[id] = i;
        {
           std::lock_guard<std::mutex> lock(mu2[id]);
           flag[id+1] = true;
           myconds[id+1].notify_one();
        }
        //pthread_cond_signal(&cond[id+1]);
        //pthread_mutex_unlock(&mutex[id+1]);
    }
}
