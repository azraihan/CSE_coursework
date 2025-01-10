#include <iostream>
#include <vector>

using namespace std;

void formWeightMatrix(int n, int m, vector<vector<int> > & weightMatrix)
{
    int u,v,w;

    for(int i=0;i<m;i++){
        cin>>u>>v>>w;
        weightMatrix[u][v]=w;
        weightMatrix[v][u]=w; //since the graph is undirected
    }
    for(int i=0;i<n; i++){
        weightMatrix[i+1][i+1]=0;  //assigning the diagonal elements zero
    }
}

vector<vector<int> > floydWarshall(int n, const vector<vector<int> > & weightMatrix){
    vector<vector<int> > d=weightMatrix;

    for(int u=1; u<=n; u++){
        for(int v=1; v<=n; v++){
            for(int x=1; x<=n; x++){
                if(d[u][v]>d[u][x]+d[x][v]){
                    d[u][v]=d[u][x]+d[x][v];
                }
            }
        }
    }
    return d;
}

int main()
{
    int n, m;  //n=no. of cities, m=no.of roads

    cin>>n>>m;

    vector<vector<int> > weightMatrix(n+1,vector<int>(n+1,10000000));   //assigning infinite weight to the whole weight matrix initially
                                                                       //n+1 to amke the matrix 1 indexed
    formWeightMatrix(n, m, weightMatrix);

    vector<vector<int> > distanceMatrix=floydWarshall(n,weightMatrix);

    int thresholdCost;
    cin>>thresholdCost;

    vector<int> minCities(n+1); //making the cities 1 indexed
    int min=INT_MAX; //assigning infinity to the min

    for(int city=1; city<=n; city++){
        int count=0;
        for(int distance: distanceMatrix[city]){
            if(distance<=thresholdCost){
                count++;
            }
        }
        if(min>count) min=count;
        minCities[city]=count;
    }

    for(int city=1; city<=n; city++){
        if(min==minCities[city]) cout<<city<<" ";
    }
    cout<<endl;

    return 0;
}