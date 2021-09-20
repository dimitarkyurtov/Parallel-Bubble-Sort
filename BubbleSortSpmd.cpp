#include<cstdlib>
#include<iostream>
#include<numeric>
#include<cmath>
#include<sstream>
#include<thread>
#include<chrono>
#include<mutex>
#include<vector>
#include <condition_variable>
#include <map>


bool flag[65];
int index[65];
std::vector<int> iters;
std::vector<bool> sorted[65];
std::mutex mu[65], mu2[65], lidermu;
std::condition_variable myconds[65];
void Sync_with_previous(int, int);
void Sync_with_next(int, int);
int arr[100000000], arr3[100000000], n = 42000, numThread = 1;
//bool isSortedd[65];
std::mutex mutexes[65], gl_mu;
std::map<int,int> itermap;
int lider, part;
bool horn = false;



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
bool isSortedAllThreads(int);
bool isSorted();

int main(int argc, char *argv[])
{
    init();
    cpyArr(arr, arr3);
    //n = atoi(argv[2]);
    lockAll();
    do
    {
        //printArr();
        testWithThreads();
        //printArr();
        numThread ++;
        cpyArr(arr3, arr);
    }while(numThread <= 4);
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
        arr[i] = getRandom(100000);
    }
}

void initCond()
{
    for(int i = 0; i < numThread; ++i)
    {
        index[i] = -1;
        flag[i] = false;
    }
    itermap.clear();
    //std::cout << "Here" << std::endl;
    //std::cout << "There" << std::endl;
    horn = false;
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

    //std::cout << "thread " <<  " exiting" << std::endl;
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
            //std::cout << "Inversion between index " << i << " and " << i+1 << std::endl;
            return false;
        }
    }
    //std::cout << "Sorting successful!" << std::endl;
    return true;
}

bool isSortedAllThreads(int iteration)
{
    for(int i = 0; i < numThread; ++i)
    {
        if(!sorted[i][iteration])
        {
            return false;
        }
    }
    return true;
}

void bubbleSort(int start, int end, int threadIdx)
{
    bool isSortedd;
    int iter = 0;
    if(start == 0)
        start = 1;
    //std::cout << "thread " << threadIdx << " entering" << std::endl;
    while(!horn && iter < n*10)
    {
            Sync_with_previous(threadIdx, iter);
            isSortedd = true;
            for(int i = start-1; i < end+1 && i < n-1; ++i)
            {
                if(arr[i] > arr[i+1])
                {
                    std::swap(arr[i], arr[i+1]);
                    isSortedd = false;
                }
            }
            Sync_with_next(threadIdx, iter);
            if(isSortedd)
            {
                //std::lock_guard<std::mutex> lock(gl_mu);
                if ( itermap.find(iter) == itermap.end() ) {
                  // not found
                  itermap[iter] = 1;
                } else {
                  // found
                  itermap[iter] ++;
                }
                if(itermap[iter] == numThread)
                {
                    //std::cout << "iteration " << iter << " numThread " << numThread << std::endl;
                    horn = true;
                }
            }
            else
            {
                //horn = false;
            }
            iter ++;
    }
    //std::cout << "thread " << threadIdx << " exiting" << std::endl;
}

void sortThread(int start, int end, int numThread)
{
    std::thread threads[65];
    int split = ((end-start)/numThread)+1;
    int newStart = start, newEnd = newStart+split;

    //std::cout << split << std::endl;
    for(int i = 0; i < numThread; ++i)
    {
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

    if (id < numThread-1)
    {
        //pthread_mutex_lock(&mutex[id+1]);
        index[id] = i;
        {
           std::lock_guard<std::mutex> lock(mu2[id+1]);
           flag[id+1] = true;
           myconds[id+1].notify_one();
           flag[id+1] = false;
        }
        //pthread_cond_signal(&cond[id+1]);
        //pthread_mutex_unlock(&mutex[id+1]);
    }
}



