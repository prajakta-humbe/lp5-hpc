#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

class Graph{
    int v;
    vector<vector<int>> adj;

    public:

    Graph(int v){
        this->v=v;
        adj.resize(v);
    }

    void addEdge(int u,int v){
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    void dfsu(int node,vector<int> &vis){
        stack<int> s;
        s.push(node);
        vis[node]=1;
        while(!s.empty()){
            int front=s.top();
            cout<<front<<" ";
            s.pop();

            for(int nbr:adj[front]){
                if(!vis[nbr]){
                    vis[nbr]=1;
                    s.push(nbr);
                }
            }
        }cout<<endl;
    }
    void dfsup(int node,vector<int> &vis,int depth=0){
        #pragma omp critical
{
    cout<<node<<" ";
}
        for(auto nbr:adj[node]){
            if(!vis[nbr]){
                bool task=false;
                #pragma omp critical
                {
                    if(!vis[nbr]){
                        vis[nbr]=1;
                        task=true;
                    }
                }
                if(task){
                    if(depth<5){
                        #pragma omp task shared(vis)
                        dfsup(nbr,vis,depth+1);
                    }
                    else{
                        dfsup(nbr,vis,depth+1);
                    }
                }
            }
        }
        #pragma omp taskwait
    }
    void dfs(int node){
        vector<int> vis(v,0);
        dfsu(node,vis);
    }
     void dfsp(int node){
        vector<int> vis(v,0);
        vis[node]=1;

        #pragma omp parallel 
        {
            #pragma omp single 
            {
                #pragma omp task shared(vis)
                dfsup(node,vis);
                #pragma omp taskwait
            }
        }
    }
    void bfsu(int node,vector<int> &vis){
        queue<int> q;
        q.push(node);
        vis[node]=1;
        while(!q.empty()){
            int front=q.front();
            cout<<front<<" ";
            q.pop();

            for(int nbr:adj[front]){
                if(!vis[nbr]){
                    vis[nbr]=1;
                    q.push(nbr);
                }
            }
        }cout<<endl;
    }

     void bfsup(int node,vector<int> &vis){
        vector<int> current;
        current.push_back(node);
        vis[node]=1;

        while(!current.empty()){
            vector<int> next;
            #pragma omp parallel for
            for(int i=0;i<current.size();i++){
                int node=current[i];
                #pragma omp critical
{
    cout<<node<<" ";
}              
                    for(auto nbr:adj[node]){

                        if(!vis[nbr]){

                            #pragma omp critical
                            {
                                if(!vis[nbr]){
                                    vis[nbr]=1;
                                    next.push_back(nbr);
                                }
                            }
                        }
                }
            }cout<<endl;
            current=next;
        }
     }

    void bfs(int node){
        vector<int> vis(v,0);
        bfsu(node,vis);
    }
    void bfsp(int node){
        vector<int> vis(v,0);
        bfsup(node,vis);
    }
};
int main(){
    int V,E;
    cout << "Enter number of vertices: ";
    cin >> V;

    cout << "Enter number of edges: ";
    cin >> E;
    Graph *obj = new Graph(V);

    for (int i = 0; i < E; i++) {

        int u, v;
        cin >> u >> v;

        obj->addEdge(u, v);
    }

    // srand(42);

    // for(int i = 0; i < E; i++) {

    //     int u = rand() % V;
    //     int v = rand() % V;

    //     // avoid self-loop
    //     if(u != v) {
    //         obj->addEdge(u, v);
    //     }
    // }

    double t1, t2;
    t1 = omp_get_wtime();
    obj->dfs(0);
    t2 = omp_get_wtime();  
    cout << "Time taken by Sequential DFS: " << (t2 - t1)*1000 << " seconds" << endl;

    t1 = omp_get_wtime();
    obj->dfsp(0);
    t2 = omp_get_wtime();
    cout << "Time taken by Parallel DFS: " << (t2 - t1)*1000 << " seconds" << endl;


    t1 = omp_get_wtime();
    obj->bfs(0);
    t2 = omp_get_wtime();
    cout << "Time taken by Seq BFS: " << (t2 - t1)*1000 << " seconds" << endl;

  
    t1 = omp_get_wtime();
    obj->bfsp(0);
    t2 = omp_get_wtime();
    cout << "Time taken by Parallel BFS: " <<(t2 - t1)*1000 << " seconds" << endl;

    // cout <<  _OPENMP << endl;

    return 0;
}