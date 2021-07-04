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

int arr[100000000], arr2[100000000], n = 100, numThread = 2;
bool isSortedd;


int getRandom(int);
void init();
void printArr();
void bubbleSort(int, int);
void sortThread(int, int, int);
void merge(int);
bool isSorted();

int main()
{
    init();
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    int startTime = clock();
    sortThread(0, n, numThread);
    merge(numThread);
    int endTime = clock();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    //printArr();
    std::cout << "Execution time: " <<
            (endTime - startTime)/double(CLOCKS_PER_SEC)
            << std::endl;
    std::cout << isSorted() << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[ms]" << std::endl;
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

    for(int i = 0; i < n; ++i)
    {
        std::cout << arr[i] << std::endl;
    }
}

void printArr()
{
    for(int i = 0; i < n; ++i)
    {
        std::cout << arr2[i] << std::endl;
    }
    std::cout << std::endl;
}

bool isSorted()
{
    for(int i = 0; i<n-1; ++i)
    {
        if(arr2[i] > arr2[i+1])
        {
            std::cout << "Sorting unsuccessful!" << std::endl;
            return false;
        }
    }
    std::cout << "Sorting successful!" << std::endl;
    return true;
}

void bubbleSort(int start, int end)
{
    bool isSorted;
    int j = 0;
    do
    {
        isSorted = true;
        for(int i = start; i < end-1; ++i)
        {
            if(arr[i] > arr[i+1])
            {
                std::swap(arr[i], arr[i+1]);
                isSorted = false;
            }
        }
    }while(!isSorted);
}

void sortThread(int start, int end, int numThread)
{
    std::thread threads[10];
    int split = (end-start)/numThread;
    int newStart = start, newEnd = start+split;

    for(int i = 0; i < numThread; ++i)
    {
        threads[i] = std::thread(bubbleSort, newStart, newEnd);
        newStart += split;
        newEnd += split;
    }

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



