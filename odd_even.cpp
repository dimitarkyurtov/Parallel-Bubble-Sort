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

struct MyStack {
    std::stack<int> s;
    int maxEle;

    int getMax()
    {
        if (s.empty())
            return -1;
            //cout << "Stack is empty\n";
        else
            return maxEle;
            //cout << "Maximum Element in the stack is: "
                 //<< maxEle << "\n";
    }

    // Prints top element of MyStack
    void peek()
    {
        if (s.empty()) {
            //cout << "Stack is empty ";
            return;
        }

        int t = s.top(); // Top element.

       // cout << "Top Most Element is: ";

        // If t < maxEle means maxEle stores
        // value of t.
        //(t > maxEle) ? cout << maxEle : cout << t;
    }

    // Remove the top element from MyStack
    void pop()
    {
        if (s.empty()) {
            //cout << "Stack is empty\n";
            return;
        }

        //cout << "Top Most Element Removed: ";
        int t = s.top();
        s.pop();

        // Maximum will change as the maximum element
        // of the stack is being removed.
        if (t > maxEle) {
            //cout << maxEle << "\n";
            maxEle = 2 * maxEle - t;
        }

        //else
            //cout << t << "\n";
    }

    // Removes top element from MyStack
    void push(int x)
    {
        // Insert new number into the stack
        if (s.empty()) {
            maxEle = x;
            s.push(x);
            //cout << "Number Inserted: " << x << "\n";
            return;
        }

        // If new number is less than maxEle
        if (x > maxEle) {
            s.push(2 * x - maxEle);
            maxEle = x;
        }

        else
            s.push(x);

        //cout << "Number Inserted: " << x << "\n";
    }
};

int arr[100000000], arr2[100000000], arr3[100000000], n = 40000, numThread = 1;
bool isSortedd;


int getRandom(int);
void init();
void printArr();
void cpyArr(int [], int []);
void bubbleSort(int, int);
void sortThread(int, int, int);
void merge(int);
void testWithThreads();
bool isSorted();

int main()
{
    init();
    cpyArr(arr, arr3);
    do
    {
        testWithThreads();
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
}

void printArr()
{
    for(int i = 0; i < n; ++i)
    {
        std::cout << arr2[i] << std::endl;
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
    merge(numThread);
    int endTime = clock();
    std::cout << "Execution time with " << numThread << " threads: " <<
            (endTime - startTime)/double(CLOCKS_PER_SEC)
            << std::endl;
    std::cout << "Is it sorted: " << isSorted() << std::endl;
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



