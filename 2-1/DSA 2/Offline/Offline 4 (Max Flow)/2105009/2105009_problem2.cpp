#include <iostream>
#include <vector>


//For BFS and DFS
#define WHITE 0   //Unvisited
#define GRAY 1    //Visited but not explored
#define BLACK 2   //Fully explored
///

using namespace std;


//arrays which declared to have N+1 elements are meant to be 1-indexed

void DFSVisit(const int& s, const vector<vector<int> > & residualGraph, vector<int> & color, vector<int> & prev, 
              vector<int> & start, vector<int> & finish){     //s=source 
    //time assignment is not actually necessary
    static int time=1;

    int nodeNum=residualGraph.size()-1;

    for(int v=1; v<=nodeNum; v++){ //going through the adjacency list of vertex 's'
        if(residualGraph[s][v]!=0 && color[v]==WHITE){ //that is, if there exists a weight and the vertex is unvisited
            start[v]= ++time;  //time assignment is not actually necessary

            color[v]=GRAY;
            prev[v]=s;
            DFSVisit(v,residualGraph,color,prev,start,finish);
        }
    }
    color[s]=BLACK;

    finish[s]= ++time;          //time assignment is not actually necessary
}


//DFS
bool augmentingPathExists(const int& s, const int& t, const vector<vector<int> > & residualGraph, vector<int> & prev){ //s=source, t=sink

    int N= residualGraph.size()-1; //N=node number
    

    vector<int> color(N+1,WHITE);

    ///the start and finish arrays are actually not necessary
    vector<int> start(N+1);
    vector<int> finish(N+1);

    color[s]=GRAY;
    start[s]=0;
    prev[s]=-1;

    DFSVisit(s, residualGraph, color, prev, start, finish);

    if(prev[t]!=-1){ //checking whether the sink is reachable or not
        return true;
    }
    return false;
}

void augmentPath(const int& t, const vector<int> & prev, vector<vector<int> > & residualGraph, vector<vector<int> > & flowGraph){
        int minWeight=1000000;  //assigning infinity as the minimnum weight

        int lastVertex=t;

        //finding minimum weight in the augmenting path
        while(prev[lastVertex]!=-1){
            if(minWeight>residualGraph[prev[lastVertex]][lastVertex]){
                minWeight=residualGraph[prev[lastVertex]][lastVertex];
            }

            lastVertex=prev[lastVertex]; //going to the previous vertex in the augmenting path
        }


        lastVertex=t; //reassigning to the sink

        while(prev[lastVertex]!=-1){
            //modifying the residual graph
            residualGraph[prev[lastVertex]][lastVertex] -= minWeight;
            residualGraph[lastVertex][prev[lastVertex]] += minWeight;

            
            //modifying the flow graph
            //f(u,v)=-f(v,u)
            if(flowGraph[prev[lastVertex]][lastVertex]>=0){
                flowGraph[prev[lastVertex]][lastVertex] += minWeight;
                flowGraph[lastVertex][prev[lastVertex]] = -flowGraph[prev[lastVertex]][lastVertex];
            }
            else{
                flowGraph[lastVertex][prev[lastVertex]] -= minWeight;
                flowGraph[prev[lastVertex]][lastVertex] = -flowGraph[lastVertex][prev[lastVertex]];
            }


            lastVertex=prev[lastVertex];   //going to the previous vertex in the augmenting path
        }
}


void fordFulkerson(const int& s, const int& t, vector<vector<int> > & residualGraph, vector<vector<int> > & flowGraph){ //s=source, t=sink
   int N=residualGraph.size()-1; //N=Node number

   //the prev array is needed in both augmentingPathExists() and augmentPath(). So, it is kept outside
   vector<int> prev(N+1, -1);

   while(augmentingPathExists(s,t,residualGraph,prev)){
        augmentPath(t,prev,residualGraph,flowGraph);

        prev=vector<int> (N+1,-1);  //reinitialized prev array is needed to again check for an aumenting path
    }
}

vector<vector<int> > mapToFlowNetwork(const vector<vector<int> > & men, const vector<vector<int> > & women){
    int menNumber=men.size()-1;   //since men[] and women[] are 1 indexed
    int womenNumber=women.size()-1;

    int totalNodeNum= menNumber+womenNumber+2;   //"+2" for the source and the sink

    vector<vector<int> > residualGraph(totalNodeNum+1, vector<int>(totalNodeNum+1,0));   //the residual graph of the flow network
                                                                                         //1-indexed. Hence, the "+1"
                                                                                         //The men are indexed from 1, after that women. And finally the source and the sink respectively.

    //forming the bipartite graph for men and women
    //The men are indexed from 1, afterward women
    for(int u=1; u<=menNumber; u++){
        for(int v=1; v<=womenNumber; v++){
            //format of the data (the vector conatained within men[] and women[]) of men and women => {height,age,divorced}

            int heightDifference=abs(men[u][0]-women[v][0]);
            int ageDifference=abs(men[u][1]-women[v][1]);
            bool divorceStatusSame=false;
            if(men[u][2]==men[v][2]) divorceStatusSame=true;

            if(heightDifference<=10 && ageDifference<=5 && divorceStatusSame){
                residualGraph[u][v+menNumber]=1;   //1 since a women can be chosen once by a single man
                                                   //v+menNumber since the women are indexed after the men
            }
        }
    }

    //adding source
    int source=menNumber+womenNumber+1;     //since source is put after all the men and the women
    for(int u=1; u<=menNumber; u++){//connecting the source to all the men
        residualGraph[source][u]=1;         //1 since a man can make maximum of one choice
    }

    //adding sink
    int sink=menNumber+womenNumber+2;       //since the sink comes after all the men, all the women, and the source
    for(int u=menNumber+1; u<=menNumber+womenNumber; u++){//connecting all the women to the sink
        residualGraph[u][sink]=1;           //1 since there can be only maximum one "copy" of a women
    }

    return residualGraph;
}

int findMaxMatch(vector<vector<int> > & residualGraph){
    int nodeNum= residualGraph.size()-1;   //since the graph is 1 indexed

    vector<vector<int> > flowGraph(nodeNum+1,vector<int>(nodeNum+1,0)); //Since the nodes are 1-indexed, we are taking an (N+1)X(N+1) matrix for ease of representation
                                                            //we are taking the initial value of flow from any u to any v as 0

    //the last node is sink and its previous one is the source
    int source=nodeNum-1;
    int sink=nodeNum;

    fordFulkerson(source,sink,residualGraph,flowGraph);

    //calculate total flow from source
    int totalFlow=0;
    for(int i=1; i<=nodeNum; i++){
        totalFlow+=flowGraph[source][i];         
    }

    return totalFlow;
}


int main()
{
    int m,n;  //m=no. of men, n=no. of women
    cin>>m>>n;

    vector<vector<int> > men(m+1);   //1-indexed. Hence the "+1"
    vector<vector<int> > women(n+1);

    
    for(int i=1; i<=m; i++){             //taking input for men
        int height, age, divorced;
        cin>>height>>age>>divorced;
        vector<int> data(3);
        data[0]= height;
        data[1]= age;
        data[2]= divorced;
        
        men[i]=data;
    }


    for(int i=1; i<=n; i++){             //taking input for women
        int height, age, divorced;
        cin>>height>>age>>divorced;
        vector<int> data(3);
        data[0]= height;
        data[1]= age;
        data[2]= divorced;
        
        women[i]=data;
    }

    vector<vector<int> > residualGraph;  //the residual graph of the flow network
                                         //1-indexed.
                                         //The men are indexed from 1, after that women. And finally the source and the sink respectively.
    
    residualGraph=mapToFlowNetwork(men,women);

    cout<<findMaxMatch(residualGraph);

    
    return 0;
}