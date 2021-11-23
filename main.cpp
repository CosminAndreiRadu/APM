#include <bits/stdc++.h>

using namespace std;

ifstream fin("bellmanford.in");
ofstream fout("bellmanford.out");

class Graph
{
private:

    int verticesNo;
    int edgesNo;
    int oriented;      //0/1/3(for unoriented with weights)/4(for oriented with weights)
    //-----------------------------------------|
    vector<vector<int>> adjacencyList;//       |
    vector<vector<pair<int,int>>> weights;//   |->should be compressed in a single way of storing both weighted and simple graphs
    vector<tuple<int,int,int>> edges;//        |
    //-----------------------------------------|


    void dfs(int vertex, vector<int> &visited);                         //simple dfs
    void dfsScc(int vertex, vector<int> &visited, vector<int> &scc);    //dfs for storing strongly connected components
    void dfsWstack(int vertex, vector<int> &visited, stack<int> &st);   //dfs with a stack for both topological sorting and strongly connected components
    int minWeightVertex(vector<int> &helper, vector<int> &includeInMst);         //pretty efficently choosing the vertex with min weight


public:

    Graph();
    Graph(int verticesNo, int edgesNo);                                 //only for transposing a graph
    Graph(int verticesNo, int edgesNo, int oriented);
    Graph transpose();

    void bfs(int start);                                                //BFS

    void stronglyConnectedComponents();

    int connectedComponentsNo();

    void topologicalSort();

    void HH();

    void MST();

    void BF(int start);

    virtual ~Graph();

};
Graph::Graph()
{
    verticesNo = edgesNo = oriented = 0;
    this->adjacencyList.clear();
    this->weights.clear();
}
Graph::Graph(int verticesNo, int edgesNo)
{
    this->verticesNo = verticesNo;
    this->edgesNo = edgesNo;
    this->adjacencyList.resize(verticesNo+1);
}
Graph::Graph(int verticesNo, int edgesNo, int oriented)
{
    this->verticesNo = verticesNo;
    this->edgesNo = edgesNo;
    this->oriented = oriented;


    if(oriented < 2)
    {this->adjacencyList.resize(verticesNo + 1);
    int vertex1, vertex2;
    for(int i = 0; i < this->edgesNo; i++)
    {
        fin>>vertex1>>vertex2;
        this->adjacencyList[vertex1].push_back(vertex2);
        if(this->oriented == 0)
            this->adjacencyList[vertex2].push_back(vertex1);
    }
    }
    else if( oriented == 4)
    {
        this->edges.resize(edgesNo+1);
        int vertex1,vertex2,cost;
        tuple<int,int,int> temp;
        for(int i = 0; i <this->edgesNo; i++)
        {
            fin>>vertex1>>vertex2>>cost;
            temp = make_tuple(vertex1,vertex2,cost);
            edges[i] = temp;

        }

    }
    else
    {
        this->adjacencyList.resize(verticesNo + 1);
        this->weights.resize(verticesNo+1);
        int vertex1, vertex2, cost;
        pair<int,int> temp;
        for(int i = 0; i < this->edgesNo; i++)
        {
            fin>>vertex1>>vertex2>>cost;

            temp.first = vertex2;
            temp.second = cost;


            this->weights[vertex1].push_back(temp);

            if(oriented == 3)
            {
                temp.first = vertex1;
                this->weights[vertex2].push_back(temp);
            }
        }


    }

}
Graph::~Graph()
{
        this->adjacencyList.clear();
        this->weights.clear();
}
void Graph::bfs(int start)
{
    int n = this->verticesNo;
    vector<int> visited;
    vector<int> distances;
    queue<int> order;
    order.push(start);

    for(int i = 0; i <= n; i++)
    {
        visited.push_back(0);
        distances.push_back(-1);
    }
    distances[start] = 0;
    visited[start] = 1;

    while(!order.empty())
    {
        int current = order.front();
        int sz = adjacencyList[current].size();

        order.pop();

        for(int i = 0; i < sz; i++)
        {
            int temp = adjacencyList[current][i];

            if(visited[temp] == 0)
            {
                visited[temp] = 1;
                order.push(temp);
                distances[temp] = distances[current]+1;
            }
        }

    }

    for(int i = 1; i < distances.size(); i++)
        fout<<distances[i]<<" ";

}
void Graph::dfs(int vertex, vector<int> &visited)
{
    visited[vertex] = 1;

    int sz = adjacencyList[vertex].size();

    for(int i = 0; i < sz; i++)
    {
        if(visited[adjacencyList[vertex][i]] == 0)
            dfs(adjacencyList[vertex][i],visited);
    }
}
void Graph::dfsScc(int vertex, vector<int> &visited, vector<int> &scc)
{
    visited[vertex] = 1;
    scc.push_back(vertex);

    int sz = adjacencyList[vertex].size();

    for(int i = 0; i < sz; i++)
    {
        if(visited[adjacencyList[vertex][i]] == 0)
            dfsScc(adjacencyList[vertex][i],visited, scc);
    }
}
int Graph::connectedComponentsNo()
{

    vector<int> visited;
    int ccn = 0; //no of connected components

    for(int i = 0 ; i <= this->verticesNo; i++)
        visited.push_back(0);

    for(int i = 0; i <= this->verticesNo; i++)
        if(visited[i] == 0)
        {
            dfs(i,visited);
            ccn++;
        }


    return ccn;

}
void Graph::stronglyConnectedComponents()
{
    stack<int> st;

    vector<int> visited;
    vector<vector<int>> sccs;
    vector<int> tempScc;

    int sccn = 0;
    for(int i = 0; i <= verticesNo; i++)
    {
        visited.push_back(0);
    }

    for(int i = 1; i <= verticesNo; i++)
        if(visited[i] == 0)
            dfsWstack(i,visited,st);

    Graph tg = transpose();

    for(int i = 0; i <= verticesNo; i++)
        visited[i] = 0;

    while(!st.empty())
    {
        int temp = st.top();
        st.pop();

        if(visited[temp] == 0)
        {   tempScc.clear();
            tg.dfsScc(temp,visited, tempScc);
            sccn++;
            sccs.push_back(tempScc);
        }
    }
    fout<<sccn<<endl;

    for(int i = 0 ; i < sccs.size(); i++)
    {
        int sz = sccs[i].size();
        for(int j = 0; j < sz; j++)
            fout<<sccs[i][j]<<" ";
            fout<<"\n";

    }


}
void Graph::dfsWstack(int vertex, vector<int> &visited, stack<int> &st)
{
    visited[vertex] = 1;

    int sz = adjacencyList[vertex].size();

    for(int i = 0; i < sz; i++)
    {
        if(visited[adjacencyList[vertex][i]] == 0)
            dfsWstack(adjacencyList[vertex][i],visited,st);
    }

    st.push(vertex);

}
void Graph::topologicalSort()
{
    stack<int> st;

    vector<int> visited;

    for(int i = 0; i <= this->verticesNo; i++)
        visited.push_back(0);

    for(int i = 1 ; i <=this->verticesNo; i++)
        if(visited[i] == 0)
            {dfsWstack(i,visited,st);}

    while(!st.empty())
    {
        fout<<st.top()<<" ";
        st.pop();
    }

}
void Graph::HH()
{

    int n,temp;
    vector<int> seq;
    fin>>n;
    for(int i = 0; i < n; i++)
    {
        fin>>temp;
        seq.push_back(temp);
    }

    while(true)
    {
        sort(seq.begin(),seq.end());
        int sz = seq.size()-1;
        if(seq[sz] == 0)
        {
            cout<<1;
            return;
        }

        int v = seq[sz];
        seq.pop_back();

        if(v > seq.size())
        {
            cout<<0;
            return;
        }

        for(int i = v-1; i >=0; i--)
        {
            seq[i]--;
            if(seq[i] < 0)
            {
                cout<<0;
                return;
            }

        }
    }


}
Graph Graph::transpose()
{
    Graph transposedGraph(verticesNo, edgesNo);

    for(int v = 1; v <= verticesNo; v++)
    {
        int sz = adjacencyList[v].size();

       for(int i = 0; i < sz; i++)
        {
            transposedGraph.adjacencyList[this->adjacencyList[v][i]].push_back(v);
        }

    }


    return transposedGraph;

}
int Graph::minWeightVertex(vector<int> &helper, vector<int> &includeInMst)
{
    int minimum, indexOfMin;
    minimum  = INT_MAX;

    for(int i = 1; i <= verticesNo; i++)
        if(includeInMst[i] == 0 && helper[i] < minimum)
        {
            minimum = helper[i];
            indexOfMin = i;
        }

    return indexOfMin;
}
void Graph::MST()
{
    vector<int> mst;            //used for storing the MST the vertex with the lowest weight from includeInMst
    vector<int> includeInMst;        //vertices that are included in MST
    vector<int> helper;         //the smallest weights to go from current vertex to any other vertex
                                //updated every step

    for(int i = 0 ; i <= verticesNo; i++)
    {
        helper.push_back(INT_MAX);
        includeInMst.push_back(0);
        mst.push_back(NULL);
    }

    helper[1] = 0;
    mst[1] = -1;
    for(int i = 1 ; i <= verticesNo; i++)
    {
        int nextVertex = minWeightVertex(helper, includeInMst);  //we get the least weighted edge
        includeInMst[nextVertex] = 1;

        int sz = weights[nextVertex].size();//-----------------------
        //cout<<nextVertex<<"--"<<sz<<endl;
        for(int j = 0; j < sz; j++)
        {   int temp1 = weights[nextVertex][j].first;
            int temp2 = weights[nextVertex][j].second;

            if(includeInMst[temp1] == 0 && temp2 < helper[temp1])
            {
                mst[temp1] = nextVertex;
                helper[temp1] = temp2;
            //cout<<helper[weights[nextVertex][j].first];
                //cout<<endl;
            }

        }

    }
    int sum = 0;
    for(int  i = 2; i <= verticesNo; i++)
        sum+= helper[i];



    fout<<sum<<"\n";
    fout<<verticesNo - 1<<"\n";
    for(int  i = 2; i <= verticesNo; i++)
        fout<<mst[i]<<" "<<i<<"\n";

}
void Graph::BF(int start)
{
     vector<int> distances;
     for(int  i = 0; i <= verticesNo; i++)
        distances.push_back(INT_MAX);

     distances[start] = 0;

     for(int  i = 1; i <= verticesNo; i++)
     {
         for(int j = 0; j < edgesNo; j++)
         {

            int temp1 = get<0>(edges[j]);
            int temp2 = get<1>(edges[j]);
            int weight = get<2>(edges[j]);

            if(distances[temp1] != INT_MAX && distances[temp1] + weight < distances[temp2])
                distances[temp2] = distances[temp1] + weight;

         }


     }


     for(int i = 0; i < edgesNo; i++)
     {
        int temp1 = get<0>(edges[i]);
        int temp2 = get<1>(edges[i]);
        int weight = get<2>(edges[i]);

        if(distances[temp1]!= INT_MAX && distances[temp1] + weight < distances[temp2])
        {
                fout<<"Ciclu negativ!";
                return;
        }


     }

    for(int i = 2; i <= verticesNo; i++)
        fout<<distances[i]<<" ";


}


int main()
{
    int n,m;
    fin>>n>>m;
    Graph g(n,m,3);
    //g.MST();
    g.BF(1);



    return 0;
}

