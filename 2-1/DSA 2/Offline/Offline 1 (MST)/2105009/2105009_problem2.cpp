#include <iostream>
#include <vector>

using namespace std;

class disjointSet{
    vector<int> parent, rank;
    int n;
 
public:
    disjointSet(int n)
    {
        parent.assign(n,-1);
        rank.assign(n,-1);
        this->n = n;
        makeSet();
    }

    void makeSet()
    {
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }
 
    int findSet(int x)
    {
        if (parent[x] != x) {
 
            parent[x] = findSet(parent[x]);
        }
 
        return parent[x];
    }

    void Union(int x, int y)
    {

        int setX = findSet(x);
        int setY = findSet(y);

        if (setX == setY)
            return;
 
        if (rank[setX] < rank[setY]) {
            parent[setX] = setY;
        }
        else if (rank[setX] > rank[setY]) {
            parent[setY] = setX;
        }
        else {
            parent[setY] = setX;
            rank[setX] = rank[setX] + 1;
        }
    }
};


void formGraph(int m, vector<pair<pair<int,int> , pair<int,int> > > & edges){
    int a,b,w;

    for(int i=0; i<m; i++){
        cin>>a>>b>>w;

        edges[i].first.first=a;
        edges[i].first.second=b;
        edges[i].second.first=w;

        edges[i].second.second=i;  //assigning the edge id
    }

}


//comparator function to sort the edges in ascending order of weight
bool compare(const pair<pair<int,int> , pair<int,int> > & p1, const pair<pair<int,int> , pair<int,int> > & p2)
{
    return p1.second.first<p2.second.first;
}

int distinctWeight(const vector<pair<pair<int,int> , pair<int,int> > > & edges){
    int count=0, prevWeight=-1;

    for(pair<pair<int,int> , pair<int,int> > p : edges){
        if(prevWeight!=p.second.first){
            count++;
            prevWeight=p.second.first;
        }
    }
    return count;
}

void formGraph2(const vector<pair<pair<int,int> , pair<int,int> > > & edges, 
               vector< vector<pair<pair<int,int> , pair<int,int> > > > & edges2)
{
    int count=-1, prevWeight=-1;
    for(pair<pair<int,int> , pair<int,int> > p : edges){
        int a=p.first.first;
        int b=p.first.second;
        int w=p.second.first;
        int id=p.second.second;
        if(prevWeight!=w){
            count++;
            prevWeight=w;
            edges2[count].push_back(make_pair(make_pair(a,b),make_pair(w,id)));
        }
        else{
            edges2[count].push_back(make_pair(make_pair(a,b),make_pair(w,id)));
        }
    }
}

void filter(int n, vector<vector<pair<pair<int,int> , pair<int,int> > > > & edges2, vector<string> & status)
{
    disjointSet set(n); //making a disjointSet data structure to keep track
    int edgeCount=0;

    for(int j=0; j<edges2.size();j++){
        if(edges2[j].size()==1){
                int u=edges2[j][0].first.first;
                int v=edges2[j][0].first.second;
                int id=edges2[j][0].second.second;

                if(set.findSet(u)!=set.findSet(v)){
                   set.Union(u,v);
                   edgeCount++;

                   status[id]="any";
                }
        }
        else{
            for(int i=0; i<edges2[j].size(); i++){
                int u=edges2[j][i].first.first;
                int v=edges2[j][i].first.second;
                int id=edges2[j][i].second.second;

                if(set.findSet(u)==set.findSet(v)){  //removal is linear
                    edges2[j].erase(edges2[j].begin()+i);
                }
                else{
                    status[id]="at least one";
                }
            }
            int count=0;
            for(int i=0; i<edges2[j].size(); i++){
                int u=edges2[j][i].first.first;
                int v=edges2[j][i].first.second;

                if(set.findSet(u)!=set.findSet(v)){
                    set.Union(u,v);
                    edgeCount++;

                    count++;
                }
            }

            if(count==edges2[j].size()){
                for(int i=0; i<edges2[j].size(); i++){
                    int id=edges2[j][i].second.second;

                    status[id]="any";
                }
            }
        }
        if (edgeCount+1==n){
            break;
        }
    }
}



int main()
{
    int n, m; //n=no. of vertices and m=no. of edges
    cin>>n>>m;

    vector<pair<pair<int,int> , pair<int,int> > > edges(m); //{{vertex1,vertex2},{weight,id}}
    formGraph(m,edges);

    sort(edges.begin(),edges.end(),compare);

    int dw= distinctWeight(edges);

    vector< vector<pair<pair<int,int> , pair<int,int> > > > edges2(dw);
    formGraph2(edges,edges2);

    vector<string> status(m,"none");

    filter(n,edges2, status);

    for(string s: status){
        cout<<s<<endl;
    }

    return 0;
}