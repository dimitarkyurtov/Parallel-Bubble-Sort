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
int arr[100000000], arr3[100000000], n = 32000, numThread = 1, z = 8000, glctr;
std::mutex mutexes[65], gl_mu;


int getRandom(int);
void init();
void initCond();
void printArr();
void cpyArr(int [], int []);
void bubbleSort(int);
void sortThread(int);
void testWithThreads();
void lockAll();
bool isSorted();

int main(int argc, char *argv[])
{
    if(argc > 2)
    {
        //n = atoi(argv[2]);
    }
    init();
    cpyArr(arr, arr3);
    lockAll();
    do
    {
        //printArr();
        testWithThreads();
        //printArr();
        numThread  ++;
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
        arr[i] = getRandom(10000000);
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
    if(z < 4500)
    {
	z = 4500;
    }
    //int startTime = clock();
    auto timeVal = std::chrono::steady_clock::now();
    sortThread(numThread);
    //merge(numThread);
    //int endTime = clock();
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
            //std::cout << i << " " << std::endl;
            //return false;
        }
    }
    //std::cout << "Sorting successful!" << std::endl;
    return true;
}


void bubbleSort(int threadIdx)
{
    int part=0;
    for(int i = 0; i < n/numThread; ++i)
    {
        //if(numThread > 1 && i < 1000)
       // std::cout << " Thread " << threadIdx << " reached iteration " << i << std::endl;
        //Sync_with_previous(threadIdx, part);
        for(int j = 0; j < n-1-glctr; ++j)
        {

            if(j%z == 0)
            {
                Sync_with_previous(threadIdx, part);
            }

            if(j%z == 0 && j != 0)
            {
                part ++;
                Sync_with_next(threadIdx, part);
                //if(numThread > 1 && threadIdx == 0)
                //std::cout << j << std::endl;
            }
            else if(j == n-2-glctr)
            {
                part ++;
                Sync_with_next(threadIdx, part);
                 //if(numThread > 1 && threadIdx == 0)
                 //std::cout << j << std::endl;

            }
            if(arr[j] > arr[j+1])
            {
                std::swap(arr[j], arr[j+1]);
            }
        }
        {
            std::lock_guard<std::mutex> lock(gl_mu);
            glctr++;
        }
        //Sync_with_next(threadIdx, part);
    }
}

void sortThread(int numThread)
{
    glctr = 0;
    for(int i = 0; i < numThread; ++i)
    {
        threads[i] = std::thread(bubbleSort, i);
    }

    for(int i = 0; i < numThread; ++i)
    {
        threads[i].join();
    }
}

void Sync_with_previous(int id, int i)
{
    if (id>0) {
        if (index[id-1]<i)
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
    if (id < numThread-1)
    {

        index[id] = i;
        {
            std::lock_guard<std::mutex> lock(mu2[id+1]);
            flag[id+1] = true;
            myconds[id+1].notify_one();
        }
    }
}
