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
std::mutex mu[10], mu2[65];
std::condition_variable myconds[65];
void Sync_with_previous(int, int);
void Sync_with_next(int, int);
std::thread threads[65];
int arr[100000000], arr2[100000000], arr3[100000000], n = 60000, numThread = 1, z = 8000;
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
    if(argc > 2)
    {
        n = atoi(argv[2]);
    }
    init();
    cpyArr(arr, arr3);
    lockAll();
    do
    {
        //printArr();
        testWithThreads();
        //printArr();
        numThread *= 2;
        cpyArr(arr3, arr);
    }while(numThread <= atoi(argv[2]));
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
    //z = 4000;
    z = n / (numThread*2);
    //int startTime = clock();
    auto timeVal = std::chrono::steady_clock::now();
    sortThread(0, n, numThread);
    //merge(numThread);
    //int endTime = clock();
    std::chrono::duration<double> timePassed = std::chrono::steady_clock::now() - timeVal;
    std::cout << numThread << " threads: " <<
            timePassed.count()
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
    for(int i = 0; i < n/numThread; ++i)
    {
        for(int j = 0; j < n-1; ++j)
        {
            if(j % z == 0 && j != 0)
            {
                Sync_with_next(threadIdx, 0);
                //Sync_with_previous(threadIdx, 0);
            }
            if(arr[j] > arr[j+1])
            {
                std::swap(arr[j], arr[j+1]);
            }
        }
    }
}

void sortThread(int start, int end, int numThread)
{
    int split = (end-start)/numThread;
    int newStart = start, newEnd = newStart+split;

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
        //if (index[id-1]<i)
        {
            std::unique_lock<std::mutex> lock(mu[id]);
            myconds[id].wait_for( lock,
                        std::chrono::seconds(100),
                        [id]() {  return flag[id]; } );
        }
   }
}

void Sync_with_next(int id, int i)
{
    if (id < n-1)
    {
        //index[id] = i;
        {
           std::lock_guard<std::mutex> lock(mu2[id]);
           flag[id+1] = true;
           myconds[id+1].notify_one();
        }
    }
}
