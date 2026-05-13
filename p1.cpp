#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

class Graph {
    int v;
    vector<vector<int>> adj;

public:
    Graph(int v)
    {
        this->v = v;
        adj.resize(v);
    }

    void addEdge(int u, int v)
    {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void dfs(int node) 
    {
        vector<int> visited(v, 0);
        dfsUtil(node, visited);
    }

    void dfsParallel(int node)
    {
        vector<int> visited(v, 0);
        visited[node] = 1;

        #pragma omp parallel
        {
            #pragma omp single
            {
                #pragma omp task shared(visited)
                dfsUtilParallel(node, visited);
                #pragma omp taskwait
            }
        }
    }

    void bfs(int node)
    {
        vector<int> visited(v, 0);
        bfsUtil(node, visited);
    }

    void bfsParallel(int node) 
    {   
        vector<int> visited(v, 0);
        bfsUtilParallel(node, visited);
    }

    void dfsUtil(int node, vector<int>& visited)
    {
       stack<int> st;          // ← use heap memory, not call stack
       st.push(node);
       visited[node] = 1; 

    while (!st.empty()) {
        int cur = st.top();
        st.pop();

        if (!visited[cur]) {
            visited[cur] = 1;
            for (auto nbr : adj[cur]) {
                if (!visited[nbr])
                    st.push(nbr);
            }
        }
    }
    }

    // void dfsUtilParallel(int node, vector<int>& visited)
    // {
    //     for (auto nbr : adj[node])
    //     {
    //         if (!visited[nbr])
    //         {
    //             bool spawn_task = false;    

    //             #pragma omp critical
    //             {
    //                 if (!visited[nbr])
    //                 {
    //                     visited[nbr] = 1;
    //                     spawn_task = true;
    //                 }
    //             }

    //             if (spawn_task)
    //             {
    //                 #pragma omp task shared(visited)
    //                 dfsUtilParallel(nbr, visited);
    //             }
    //         }
    //     }

    //     #pragma omp taskwait
    // }
    void dfsUtilParallel(int node, vector<int>& visited, int depth = 0)
{
    for (auto nbr : adj[node])
    {
        if (!visited[nbr])
        {
            bool spawn_task = false;
            #pragma omp critical
            {
                if (!visited[nbr])
                {
                    visited[nbr] = 1;
                    spawn_task = true;
                }
            }

            if (spawn_task)
            {
                if (depth < 5)  // ← cap recursion depth
                {
                    #pragma omp task shared(visited)
                    dfsUtilParallel(nbr, visited, depth + 1);
                }
                else
                {
                    dfsUtilParallel(nbr, visited, depth + 1); // sequential fallback
                }
            }
        }
    }
    #pragma omp taskwait
}

    void bfsUtil(int node, vector<int>& visited)
    {
        queue<int> q;
        q.push(node);
        visited[node] = 1;

        while(!q.empty())
        {
            auto front = q.front();
            q.pop();

            for (auto &nbr : adj[front])
            {
                if (!visited[nbr])
                {
                    visited[nbr] = 1;
                    q.push(nbr);
                }
            }
        }
    }

    void bfsUtilParallel(int node, vector<int>& visited) 
    {
        vector<int> current;
        current.push_back(node);
        visited[node] = 1;

        while(!current.empty())
        {
            vector<int> next;

            #pragma omp parallel for
            for (int i = 0; i < current.size(); i++)
            {
                int node = current[i];

                for (int nbr : adj[node])
                {
                    if (!visited[nbr])
                    {
                        #pragma omp critical
                        {
                            if (!visited[nbr])
                            {
                                visited[nbr] = 1;
                                next.push_back(nbr);
                            }
                        }
                    }
                }
            }

            current = next;
        }
    }

   
};

int main()
{
    int V,E;
    cout << "Enter number of vertices: ";
    cin >> V;

    cout << "Enter number of edges: ";
    cin >> E;
    Graph *obj = new Graph(V);

    // for (int i = 0; i < E; i++) {

    //     int u, v;
    //     cin >> u >> v;

    //     obj->addEdge(u, v);
    // }

    srand(42);

    for(int i = 0; i < E; i++) {

        int u = rand() % V;
        int v = rand() % V;

        // avoid self-loop
        if(u != v) {
            obj->addEdge(u, v);
        }
    }

    double t1, t2;
    t1 = omp_get_wtime();
    obj->dfsParallel(0);
    t2 = omp_get_wtime();  
    cout << "Time taken by parallel DFS: " << (t2 - t1)*1000 << " seconds" << endl;

    t1 = omp_get_wtime();
    obj->bfsParallel(0);
    t2 = omp_get_wtime();
    cout << "Time taken by parallel BFS: " << (t2 - t1)*1000 << " seconds" << endl;

    t1 = omp_get_wtime();
    obj->dfs(0);
    t2 = omp_get_wtime();
    cout << "Time taken by sequential DFS: " << (t2 - t1)*1000 << " seconds" << endl;

    t1 = omp_get_wtime();
    obj->bfs(0);
    t2 = omp_get_wtime();
    cout << "Time taken by sequential BFS: " <<(t2 - t1)*1000 << " seconds" << endl;

    // cout <<  _OPENMP << endl;

    return 0;
}