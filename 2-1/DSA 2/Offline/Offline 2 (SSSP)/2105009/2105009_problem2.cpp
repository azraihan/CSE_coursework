#include <iostream>
#include <vector>

using namespace std;

void addEdges(int M, vector<pair<pair<int,int>, int> > & edges){
    int u,v,w;

    for(int i=0; i<M; i++){
        cin>>u>>v>>w;

        edges.push_back(make_pair(make_pair(u,v),w));
    }
}

bool bellmanFord(int N, int source, const vector<pair<pair<int,int>, int> > & edges, vector<int> & distance, vector<int> & prev){
    distance[source]=0;
    for(int i=0; i<N-1; i++){
        for(pair<pair<int,int>, int> p: edges){
           int u=p.first.first;
           int v=p.first.second;
           int w=p.second;

           if(distance[v]>distance[u]+w){  //edge relax
                distance[v]=distance[u]+w;
                //cout<<newDistance<<endl;
                prev[v]=u;
           }
        }
    }

    //checking for negative cycle
    for(pair<pair<int,int>, int> p: edges){
           int u=p.first.first;
           int v=p.first.second;
           int w=p.second;

           if(distance[v]>distance[u]+w){  //negative cycle
                return true; //negative cycle found
           }
    }
    return false;   //np negative cycle found
}

int max(int a, int b){
    return a>b?a:b;
}

int main()
{
    int N,M;
    cin>>N>>M;

    vector<pair<pair<int,int>, int> > edges;
    addEdges(M,edges);

    vector<int> distance(N+1, 1000000); //1 indexed
    vector<int> prev(N+1,-1); //1 indexed

    int A,B,L,H,S,D; //edge to be connected from node A to B,
                     //[L,H] is the range of weights
                     //S=source, D=destination
    
    cin>>A>>B>>L>>H>>S>>D;
    
    bellmanFord(N,S,edges,distance,prev);
    int initialDistance=distance[D];

    int minDistance=initialDistance;
    int assignedWeight;

    // for(int weight=L; weight<=H; weight++){
    //     vector<int> distance(N+1, 1000000); //1 indexed
    //                                         //temp distance array
    //     vector<int> prev(N+1,-1); //1 indexed
    //                               //not needed actually

    //     edges.push_back(make_pair(make_pair(A,B),weight)); //adding a temporary edge

    //     bool hasNegativeCycle=bellmanFord(N,S,edges,distance,prev);

    //     if(!hasNegativeCycle && minDistance>distance[D]){
    //         minDistance=distance[D];
    //         assignedWeight=weight;
    //     }

    //     edges.pop_back(); //removing the temporary edge
    // }

    // if(minDistance==initialDistance) cout<<"impossible\n";

    // else{
    //     cout<<assignedWeight<<" "<<minDistance<<endl;
    // }

    vector<int> distance2(N+1, 1000000); //1 indexed
                                         //temp distance array
    
    vector<int> prev2(N+1,-1); //1 indexed
                               //not needed actually
    
    bellmanFord(N,B,edges,distance2,prev2);

    //if no negative cycle is present, add the edgge weight between A and B with minimum weight
    if(distance2[A]==1000000){
        int newDistance=distance2[D]+distance[A]+L;
        if(distance[D]>newDistance) cout<<L<<" "<<newDistance;
        else cout<<"impossible\n"; 
    }

    //when the distance to A from B is negative
    else if(distance2[A]<0){
        //if negative cycle is present, add the edge between A and B such that it makes the net weight of the overall cycle zero
        //or reduce it to its minimum possible value
            if(H>=-distance2[A]){
                int newDistance=distance2[D]+max(L,-distance2[A])+distance[A];
                if(distance[D]>newDistance) cout<<-distance2[A]<<" "<<newDistance;
                else cout<<"impossible\n";
            }
            else cout<<"impossible\n";
    }
    
    //when the distance to A from B is positive
    else{
            //If the least weight of all the possible choices is negative, then theres no possibility of a negative cycle
            if(L>=0){
                int newDistance=distance2[D]+L+distance[A];
                if(distance[D]>newDistance) cout<<L<<" "<<newDistance;
                else cout<<"impossible\n";
            }
            //else there's a possibility of a negative cycle. Make the net weight of the cycle zero
            //or reduce it to its minimum possible value
            else{
                if(H>=-distance2[A]){
                    int newDistance=distance2[D]+max(L,-distance2[A])+distance[A];
                    if(distance[D]>newDistance) cout<<-distance2[A]<<" "<<newDistance;
                    else cout<<"impossible\n";
                }
                else cout<<"impossible\n";
            }
    }

    return 0;    
}


