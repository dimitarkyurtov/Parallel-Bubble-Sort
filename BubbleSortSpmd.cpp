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


std::thread threads[10];
int arr[100000000], arr2[100000000], arr3[100000000], n = 40000, numThread = 1;
bool isSortedd[65];


int getRandom(int);
void init();
void printArr();
void cpyArr(int [], int []);
void bubbleSort(int, int, int);
void sortThread(int, int, int);
void merge(int);
void testWithThreads();
bool isSortedAllThreads();
bool isSorted();

int main()
{
    init();
    cpyArr(arr, arr3);
    do
    {
        //printArr();
        testWithThreads();
        //printArr();
        numThread *= 2;
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
    for(int i = 0; i < numThread; ++i)
    {
        isSortedd[i] = false;
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


void testWithThreads()
{
    int startTime = clock();
    sortThread(0, n, numThread);
    //merge(numThread);
    int endTime = clock();
    std::cout << "Execution time with " << numThread << " threads: " <<
            (endTime - startTime)/double(CLOCKS_PER_SEC)
            << std::endl;
    std::cout << "Is it sorted: " << isSorted() << std::endl;
}

bool isSorted()
{
    for(int i = 0; i < n-1; ++i)
    {
        if(arr[i] > arr[i+1])
        {
            std::cout << "Sorting unsuccessful!" << std::endl;
            return false;
        }
    }
    std::cout << "Sorting successful!" << std::endl;
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
    std::mutex mu;
    if(start == 0)
    {
        start = 1;
    }
    do
    {
        if(iteration > n)
        {
            break;
        }
        if(iteration == 1 && threadIdx < numThread)
        {
            threads[threadIdx] = std::thread(bubbleSort, threadIdx*(n/numThread), (threadIdx+1)*(n/numThread), threadIdx+1);
        }

        /*
        {
            std::lock_guard<std::mutex> lock(mu);
            std::cout << "Thread " << threadIdx << " running iteration " << iteration << " start " << start << " end " << end << std::endl;
            printArr();
        }
        */

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
        if(isSorted)
        {
            isSortedd[threadIdx-1] = true;
        }
        else
        {
            isSortedd[threadIdx-1] = false;
        }
        iteration++;
    }while(true);
}

void sortThread(int start, int end, int numThread)
{
    int split = (end-start)/numThread;


    threads[0] = std::thread(bubbleSort, 0, split, 1);

    for(int i = 0; i < numThread; ++i)
    {
        threads[i].join();
    }
}

void merge(int numThread)
{
    int markers[32], markersMin[32];
    int split = n/numThread;
    int start = -1, max, idx;
    for(int i = 0; i < numThread; ++i)
    {
        markersMin[i] = start;
        start += split;
        markers[i] = start;
    }

    for(int j = n-1; j >= 0; --j)
    {
        max = -1;
        idx = 0;
        for(int i = 0; i < numThread; ++i)
        {
            if(markers[i] > markersMin[i] && arr[markers[i]] > max)
            {
                max = arr[markers[i]];
                idx = i;
            }
        }
        if(max != -1){
            arr2[j] = max;
            markers[idx] --;
        }
    }
}



