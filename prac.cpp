#include <bits/stdc++.h>
#include<omp.h>
using namespace std;

class Graph{
    public:
    int v;
    vector<vector<int>> adj;

    Graph(int v){
        this->v=v;
        adj.resize(v);
    }

    void addEdge(int u,int v){
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    
    void bfsu(int node,vector<int> &vis){
        queue<int> q;
        q.push(node);
        vis[node]=1;

        while(!q.empty()){

            int front=q.front();
            q.pop();

            for(auto nbr:adj[front]){
                if(!vis[nbr]){
                    vis[nbr]=1;
                    q.push(nbr);
                }
            }
        }
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
                for(auto nbr:adj[node]){
                    if(!vis[nbr]){
                        #pragma omp critical
                        {
                            if(!vis[nbr]){
                                next.push_back(nbr);
                                vis[nbr]=1;
                            }
                        }

                    }
                }
             }
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
    

    void dfsu(int node,vector<int> &vis){
        stack<int> s;
        s.push(node);
        vis[node]=1;

        while(!s.empty()){

            int front=s.top();
            s.pop();

            for(auto nbr:adj[front]){
                if(!vis[nbr]){
                    vis[nbr]=1;
                    s.push(nbr);
                }
            }
        }
    }
     
    void dfsup(int node,vector<int> &vis,int depth=0){

        for(auto nbr : adj[node]){
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
};
int main(){
    int v,e;

cout<<"Enter the number of vertices: ";
cin>>v;

cout<<"Enter the number of edges: ";
cin>>e;

Graph *obj = new Graph(v);

    // for(int i=0;i<e;i++){
    //     int u,v;
    //     cin>>u>>v;
    //     obj->addEdge(u, v);        
    // }
    
    srand(time(0));
    for(int i=0;i<e;i++){
        int u,v1;
        u=rand()%v;
        v1=rand()%v;
        
        if(u!=v1){
           obj->addEdge(u,v1);
        }
    }

     double t1, t2;
    t1 = omp_get_wtime();
    obj->dfs(0);
    t2 = omp_get_wtime();  
    cout << "Time taken by  DFS Seq: " << (t2 - t1)*1000 << " seconds" << endl;
    cout<<endl;

    t1 = omp_get_wtime();
    obj->dfsp(0);
    t2 = omp_get_wtime();  
    cout << "Time taken by  DFS Parllel: " << (t2 - t1)*1000 << " seconds" << endl;
    cout<<endl;
    
    t1 = omp_get_wtime();
    obj->bfs(0);
    t2 = omp_get_wtime();  
    cout << "Time taken by  BFS Seq: " << (t2 - t1)*1000 << " seconds" << endl;
    cout<<endl;
    
    t1 = omp_get_wtime();
    obj->bfsp(0);
    t2 = omp_get_wtime();  
    cout << "Time taken by  BFS Parallel: " << (t2 - t1)*1000 << " seconds" << endl;
    cout<<endl;

}