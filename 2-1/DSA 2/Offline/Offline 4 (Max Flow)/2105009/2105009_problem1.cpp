#include <iostream>
#include <vector>
#include <queue>


//For BFS and DFS
#define WHITE 0   //Unvisited
#define GRAY 1    //Visited but not explored
#define BLACK 2   //Fully explored
///

using namespace std;


//arrays which declared to have N+1 elements are meant to be 1-indexed


void formAdjacencyMatrix(const int& M, vector<vector<int> > & adjacencyMatrix)
{
    int u,v,c;  //u,v are nodes and c=capacity

    for(int i=0; i<M; i++){
        cin>>u>>v>>c;
        adjacencyMatrix[u][v]=c;
    }
}

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

//The first element is the node, and the second one is the flow through it
pair<int,int> findNodeWithMaxFlow(const vector<vector<int> > & flowGraph){
    int node=-1;
    int maxFlow=-1;

    int totalFlow=0;

    int nodeNum=flowGraph.size()-1;

    //notice the indexing of the loops as we are excluding the source and the sink
    for(int u=2; u<nodeNum; u++){  //considering all vertices u except the source and the sink. hence 2<=u<nodeNum
        for(int v=1; v<=nodeNum; v++){  //considering flow from vertex u to all other vertices v (including source and sink). Hence 1<=v<=nodeNum
            if(flowGraph[u][v]>0) totalFlow+=flowGraph[u][v];   //flowGraph[u][v]>0 means there is an outward flow from u to v. We are only considering those
        }
        if(totalFlow>maxFlow){  //determining whether max
            node=u;
            maxFlow=totalFlow;
        }
        totalFlow=0;
    }

    return make_pair(node,maxFlow);
}

//This BFS returns the status of all the vertices reachable from the source (visited or not)
vector<int> BFS(const int& source, const vector<vector<int> > & adjacencyMatrix){
    
    int nodeNum=adjacencyMatrix.size()-1;
    vector<int> color(nodeNum+1, WHITE);

    //keeping depth[] and prev[] arrays are not actually necessary here
    vector<int> depth(nodeNum+1, 1000000);   //assigning infinite depth initially
    vector<int> prev(nodeNum+1,-1);          //assigning NULL (or -1) as prev node initially

    queue<int> Q;
    
    color[source]=GRAY;
    depth[source]=0;

    Q.push(source);

    while(!Q.empty()){
        int u= Q.front();
        Q.pop();

        if(color[u]==GRAY){  //if the vertex was visited but not explred, explre it
            for(int v=1; v<=nodeNum; v++){  //explore the adjacency list of vertex 'u'
                if(color[v]==WHITE && adjacencyMatrix[u][v]!=0){
                    color[v]=GRAY;
                    depth[v]=depth[u]+1;
                    prev[v]=u;

                    Q.push(v);
                }
            }
        }
        color[u]=BLACK;  
    }

    return color;
}

//vertices reachable from the source in the final residual graph form the part of min cut containing the source
vector<vector<int> > findMinCut(const int& source, const vector<vector<int> > & residualGraph){
    vector<int> status= BFS(source, residualGraph);  //BLACK=reachable from source, otherwise not reachable

    vector<vector<int> > minCut(2);  //Contains two arrays. First one is the left cut (contains the source), and the other one is the right cut (contains the sink)

    int nodeNum= residualGraph.size()-1;

    for(int u=1; u<=nodeNum; u++){  //traversing the status array
        if(status[u]==BLACK) minCut[0].push_back(u);  //reachable arrays are in the left cut
        else minCut[1].push_back(u);    //otherwise right
    }

    return minCut;
}



int main()
{
    int N, M; //N=no. of nodes, M= no. of edges

    cin>>N>>M;

    vector<vector<int> > residualGraph(N+1,vector<int>(N+1,0));  //Since the nodes are 1-indexed, we are taking an (N+1)X(N+1) matrix for ease of representation
                                                                 //If there is no edge between u and v, then w(u,v)=0

    //This is not needed, but keeping it anyway as the residual graph (initially the adjacency matrix) is changed
    vector<vector<int> > adjacencyMatrix= residualGraph;

    formAdjacencyMatrix(M,residualGraph);  //The initial residual graph is just the adjacency matrix

    int s,t; //s=source, t=sink
    cin>>s>>t;

    vector<vector<int> > flowGraph(N+1,vector<int>(N+1,0)); //Since the nodes are 1-indexed, we are taking an (N+1)X(N+1) matrix for ease of representation
                                                            //we are taking the initial value of flow from any u to any v as 0

    fordFulkerson(s, t,residualGraph, flowGraph); 


//problem#1(a)
    pair<int,int> nodeWithMaxFlow=findNodeWithMaxFlow(flowGraph);  // returns a pair {node with maximum flow, value of flow through it}
    cout<<nodeWithMaxFlow.first<<" "<<nodeWithMaxFlow.second<<endl;  

    
//problem#1(b)
    vector<vector<int> > minCut=findMinCut(s,residualGraph);  //returns the left cut and the right cut altogether

    //displaying the minimum cut
    cout<<"[";
    for(vector<int> cut: minCut){
        cout<<"{";
        for(int i=0; i<cut.size(); i++){
            if(i==cut.size()-1) cout<<cut[i];          
            else cout<<cut[i]<<", ";
        }
        cout<<"}";
    }
    cout<<"]\n";

    //calculating total flow of the network, as we know that the flow through any cut is equal to the total flow
    int totalFlow=0;
    for(int i=1; i<=N; i++){
        totalFlow+=flowGraph[s][i];         
    }
    cout<<totalFlow<<endl;

    return 0;
}