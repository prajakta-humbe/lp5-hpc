#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

void min_seq(vector<int> &arr)
{
    int mini = INT_MAX;
    int n = arr.size();

    for (int i = 0; i < n; i++)
    {
        if (arr[i] < mini)
            mini = arr[i];
    }
}

void min_par(vector<int> &arr)
{
    int mini = INT_MAX;
    int n = arr.size();

    #pragma omp parallel for reduction(min:mini)
    for (int i = 0; i < n; i++)
    {
        if (arr[i] < mini)
            mini = arr[i];
    }
}

void max_seq(vector<int> &arr)
{
    int maxi = INT_MIN;
    int n = arr.size();

    for (int i = 0; i < n; i++)
    {
        if (arr[i] > maxi)
            maxi = arr[i];
    }
}

void max_par(vector<int> &arr)
{
    int maxi = INT_MIN;
    int n = arr.size();

    #pragma omp parallel for reduction(max:maxi)
    for (int i = 0; i < n; i++)
    {
        if (arr[i] > maxi)
            maxi = arr[i];
    }
}

long long sum_seq(vector<int> &arr)
{
    long long sum = 0;
    int n = arr.size();

    for (int i = 0; i < n; i++)
    {
        sum += arr[i];
    }

    return sum;
}

long long sum_par(vector<int> &arr)
{
    long long sum = 0;
    int n = arr.size();

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++)
    {
        sum += arr[i];
    }

    return sum;
}

int main()
{
    int n = 5e7;  //5 * 10^7
    vector<int> arr(n);

    // cout << omp_get_max_threads();
    omp_set_num_threads(8);
    for (int i = 0; i < n; i++)
        arr[i] = rand() % 100000;

    double t1, t2;

    t1 = omp_get_wtime();
    min_seq(arr);
    t2 = omp_get_wtime();
    cout << "Time taken by sequential min: " << t2 - t1 << " seconds" << endl;

    t1 = omp_get_wtime();
    min_par(arr);
    t2 = omp_get_wtime();
    cout << "Time taken by parallel min: " << t2 - t1 << " seconds" << endl;

    t1 = omp_get_wtime();
    max_seq(arr);
    t2 = omp_get_wtime();
    cout << "Time taken by sequential max: " << t2 - t1 << " seconds" << endl;

    t1 = omp_get_wtime();
    max_par(arr);
    t2 = omp_get_wtime();
    cout << "Time taken by parallel max: " << t2 - t1 << " seconds" << endl;

    t1 = omp_get_wtime();
    sum_seq(arr);
    t2 = omp_get_wtime();
    cout << "Time taken by sequential sum: " << t2 - t1 << " seconds" << endl;

    t1 = omp_get_wtime();
    sum_par(arr);
    t2 = omp_get_wtime();
    cout << "Time taken by parallel sum: " << t2 - t1 << " seconds" << endl;
}