#include <iostream>
#include <vector>
#include <queue>

using namespace std;


void addEdges(int N, int M, int C, vector<pair<pair<int,int> , int> > & edges,  vector<pair<int,int> > & edgeWithId)
{
    int u, v, w;

    for(int i=0; i<M; i++){
        cin>>u>>v>>w;
            edges.push_back(make_pair(make_pair(u,v),w));
    }

    for(int i=1; i<=N; i++){    //0 indexed IDs
        for(int j=0; j<=C; j++){
            edgeWithId.push_back(make_pair(i,j));
        }
    }
}

void formGraph(const vector<pair<pair<int,int> , int> > & edges, vector<vector<pair<int,int> > > & adjacencyList)
{
    for(pair<pair<int,int> , int> p : edges){  //nodes are 1 indexed
        int u=p.first.first;
        int v=p.first.second;
        int w=p.second;

        adjacencyList[u].push_back(make_pair(v,w));
        adjacencyList[v].push_back(make_pair(u,w));
    }
}

void formStateGraph(int N, int C, const vector<int> & fuelPrice, const vector<pair<pair<int,int> , int> > & edges, 
                    const vector<pair<int,int> > & edgeWithId, const vector<vector<pair<int,int> > > & adjacencyList,
                    vector<vector<pair<int,int> > > & adjacencyListOfStateGraph)
{

    //adding nodes with different states in the same city
    for(int i=0; i<N; i++){  //zero indexed city count
        int startIndex=C*i+i;
        for(int j=startIndex; j<startIndex+C ;j++){
            adjacencyListOfStateGraph[j].push_back(make_pair(j+1,fuelPrice[i]));
        }
    }

    for(int i=1; i<=N; i++){
        for(pair<int, int> p: adjacencyList[i]){
            int u=i;
            int v=p.first;
            int w=p.second;

            int startIndex=C*(u-1)+(u-1);

            for(int j=startIndex; j<=startIndex+C; j++){
                int startIndex2=C*(v-1)+(v-1);
                int currentGasState= edgeWithId[j].second;
                int nextGasState=currentGasState-w;

                if(nextGasState>=0){
                    adjacencyListOfStateGraph[j].push_back(make_pair(startIndex2+nextGasState,0));
                }
            }
        }
    }
}


class compare{
    public:
    bool operator()(pair<int,int> p1, pair<int,int> p2){
         return p1.second>p2.second;
    }
};

template <typename E, typename Compare>
void removeElementFromPriorityQueue(priority_queue<E, vector<E>, Compare> &pq, E element) {
    priority_queue<E, vector<E>, Compare> temp;

    while (!pq.empty()) {
        E val = pq.top();
        pq.pop();

        if (val != element) {
            temp.push(val);
        }
    }

    pq = temp;
}

template <typename E, typename Compare>
void updateKey(priority_queue<E, vector<E>, Compare> &pq, E oldKey, E newKey) {
    removeElementFromPriorityQueue(pq, oldKey);
    pq.push(newKey);
}

void djikstra(int source, vector<pair<int,int> > & distanceVertexPair, vector<int> & distanceArray, vector<int> & prev, const vector<vector<pair<int,int> > > & adjacencyListOfStateGraph)
{
    distanceVertexPair[source].second=0;
    distanceArray[source]=0;

    priority_queue<pair<int,int>, vector<pair<int,int> >, compare> pq (distanceVertexPair.begin(), distanceVertexPair.end());

    while(!pq.empty()){
        pair<int,int> p = pq.top();
        pq.pop();

        int u=p.first;

        for(pair<int,int> p: adjacencyListOfStateGraph[u]){
            int v= p.first;
            int w= p.second;
            int oldDistance=distanceArray[v];
            if(distanceArray[v]>distanceArray[u]+w){   //Edge Relax
                int newDistance=distanceArray[v]=distanceArray[u]+w;
                prev[v]=u;

                updateKey<pair<int,int>, compare>(pq, make_pair(v,oldDistance),make_pair(v, newDistance));
            }
        }
    }
}



int main()
{
    int N,M,C;
    cin>>N>>M>>C;  //N=no. of cities, M=no. of edges, C=capacity of car


    vector<int> fuelPrice;
    for(int i=0;i<N;i++){
        int cost;
        cin>>cost;
        fuelPrice.push_back(cost);
    }

    vector<pair<pair<int,int> , int> > edges;
    vector<pair<int,int> > edgeWithId;
    addEdges(N,M,C,edges,edgeWithId);

    vector<vector<pair<int,int> > > adjacencyList(N+1); //N+1 because the vertices are 1-indexed
                                                        //The first one is the vertex and the second one is the weight
    formGraph(edges,adjacencyList);

    
    vector<vector<pair<int,int> > > adjacencyListOfStateGraph(N*(C+1)); //0 indexed
                                                            //The first one is the vertex and the second one is the weight
    formStateGraph(N,C,fuelPrice,edges,edgeWithId,adjacencyList,adjacencyListOfStateGraph);



    vector<int> distanceArray(N*(C+1), 100000);
    vector<pair<int,int> > distanceVertexPair(N*(C+1));
    for(int i=0; i<N*(C+1); i++){
        distanceVertexPair[i]=make_pair(i,100000);
    }

    vector<int> prev(N*(C+1),-1);

    
    int source, destination;
    cin>>source>>destination;

    int actualSource=(source-1)*(C+1);
    int actualDestination=(destination-1)*(C+1);

    djikstra(actualSource, distanceVertexPair, distanceArray, prev, adjacencyListOfStateGraph);

    if(distanceArray[actualDestination]==100000){
        cout<<"impossible\n";
    }
    else{
        cout<<distanceArray[actualDestination]<<endl;
    }

    return 0;
}