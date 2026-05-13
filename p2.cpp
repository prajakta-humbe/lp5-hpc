#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

void bubbleSortParallel(vector<int>& arr)
{
    int n = arr.size();
    for (int i = 0; i < n; i++)
    {
        #pragma omp parallel for
        for (int j = (i % 2); j < n - 1; j += 2)
        {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
   
}

void bubbleSort(vector<int> &arr)
{
    int n = arr.size();
    for (int i = 0; i < n - 1; i++)
    {
        bool isSwapped = false;
        for (int j = 0; j < n - 1 - i; j++)
        {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]), isSwapped = true;
        }

        if (!isSwapped)
            break;
    }
}

void merge1(vector<int> &arr, int left, int mid, int right)
{
    vector<int> temp(right - left + 1); 
    int ptr = 0, i = left, j = mid + 1;

    while(i <= mid && j <= right)
    {
        if (arr[i] < arr[j])
            temp[ptr++] = arr[i++];
        else 
            temp[ptr++] = arr[j++];
    }

    while(i <= mid)
        temp[ptr++] = arr[i++];
    
    while(j <= right)
        temp[ptr++] = arr[j++];

    for (int i = 0; i < right - left + 1; i++)
    {
        arr[i + left] = temp[i];
    }
}

void mergeSortUtil(vector<int> &arr, int left, int right)
{
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;
    mergeSortUtil(arr, left, mid);
    mergeSortUtil(arr, mid + 1, right);
    merge1(arr, left, mid, right);
}

void mergeSortUtilParallel(vector<int> &arr, int left, int right)
{
    if (left >= right)
        return;

    if (right - left < 1000)
    {
        mergeSortUtil(arr, left, right);
        return;
    }

    int mid = left + (right - left) / 2;

    #pragma omp task shared(arr)
    mergeSortUtilParallel(arr, left, mid);

    #pragma omp task shared(arr)
    mergeSortUtilParallel(arr, mid + 1, right);

    #pragma omp taskwait
    merge1(arr, left, mid, right);
}

void mergeSort(vector<int> &arr)
{
    int n = arr.size();
    mergeSortUtil(arr, 0, n - 1);
}

void mergeSortParallel(vector<int> &arr)
{
    int n = arr.size();
    #pragma omp parallel
    {
        #pragma omp single
        {
            mergeSortUtilParallel(arr, 0, n - 1);
        }
    }
}

void printArray(const vector<int> &arr)
{
    for (int i : arr)
        cout << i << " ";
    cout << endl;
}

int main() 
{
    //int n = 8000;

    int n;
    cout<<"Enter the number of elements: ";
    cin>>n;

    vector<int> arr(n);

    srand(time(0));   //This ensures you get different random numbers every time the program runs. Without this, rand() would produce the same sequence every run.

    for (int i = 0; i < n; i++)
        arr[i] = rand() % 1000;  //generates the random number between 0 to 99

    // for(int i=0;i<n;i++){
    //     cin>>arr[i];
    // }

    double t1, t2;

    t1 = omp_get_wtime();  //returns call time 
    vector<int> arrCopy1 = arr;
    bubbleSort(arrCopy1);
    t2 = omp_get_wtime();
    // printArray(arrCopy1);
    cout << "Time taken by sequential bubble sort: " << t2 - t1 << " seconds" << endl;

    t1 = omp_get_wtime();
    vector<int> arrCopy2 = arr;
    bubbleSortParallel(arrCopy2);
    t2 = omp_get_wtime();
    // printArray(arrCopy2);
    cout << "Time taken by parallel bubble sort: " << t2 - t1 << " seconds" << endl;

    t1 = omp_get_wtime();
    vector<int> arrCopy3 = arr;
    mergeSort(arrCopy3);
    t2 = omp_get_wtime();
    // printArray(arrCopy3);
    cout << "Time taken by sequential merge sort: " << t2 - t1 << " seconds" << endl;

    t1 = omp_get_wtime();
    vector<int> arrCopy4 = arr;
    mergeSortParallel(arrCopy4);
    t2 = omp_get_wtime();
    // printArray(arrCopy4);
    cout << "Time taken by parallel merge sort: " << t2 - t1 << " seconds" << endl;
}