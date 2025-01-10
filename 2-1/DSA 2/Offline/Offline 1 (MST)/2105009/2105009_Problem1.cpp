#include <iostream>
#include <vector>
#include <set>

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

void printOneMST(const vector<pair<pair<int,int> , int> > & MST){
    cout<<"[";
    for(int i=0; i<MST.size(); i++){
        cout<<"[";
        cout<<MST[i].first.first<<", "<<MST[i].first.second<<", "<<MST[i].second;
        cout<<"]";
        if(i!=MST.size()-1) cout<<", ";
    }
    cout<<"]\n";
}

void printAllMSTs(const vector< vector<pair<pair<int,int> , int> > > & allMSTs){
    cout<<"MST:\n";
    for(int i=1; i<=allMSTs.size(); i++){
        cout<<i<<" : ";
        printOneMST(allMSTs[i-1]);
    }
}


void formGraph(int m, vector<pair<pair<int,int> , int> > & edges){
    int a,b,w;

    for(int i=0; i<m; i++){
        cin>>a>>b>>w;

        edges[i].first.first=a;
        edges[i].first.second=b;
        edges[i].second=w;
    }

}

//comparator function to sort the edges in ascending order of weight
bool compare(const pair<pair<int,int> , int> & p1, const pair<pair<int,int> , int> & p2)
{
    return p1.second<p2.second;
}


void combRecur(const vector<pair<pair<int,int> , int> > & edges , vector< vector<pair<pair<int,int> , int> > > & ans,
    vector<pair<pair<int,int> , int> > & tmp, int n, int left, int k)
{
    if (k == 0) {
        ans.push_back(tmp);
        return;
    }

    for (int i = left; i < n; ++i)
    {
        tmp.push_back(edges[i]);
        combRecur(edges, ans, tmp, n, i + 1, k - 1);
        tmp.pop_back();
    }
}
vector< vector<pair<pair<int,int> , int> > > getCombination(const vector<pair<pair<int,int> , int> > & edges, int n, int k)
{
    vector< vector<pair<pair<int,int> , int> > > ans;
    vector<pair<pair<int,int> , int> > tmp;
    combRecur(edges, ans, tmp, n, 0, k);
    return ans;
}

//passing edges2 by copy
void findMST(int n, vector<pair<pair<int,int> , int> > edges, vector<pair<pair<int,int> , int> > & MST){
    disjointSet set(n); //making a disjointSet data structure to keep track
    int edgeCount=0;
    
    for(pair<pair<int,int> , int> p: edges){
        int u=p.first.first;
        int v=p.first.second;

        if(set.findSet(u)!=set.findSet(v)){
            MST.push_back(p);
            set.Union(u,v);
        }
    }
}

bool filter(vector< pair< pair<int, int>, int> >& graph, int n) {
    
    disjointSet set(n); 
    int edgeCount = 0;

    for (pair<pair<int, int>, int> p : graph) {
        int u = p.first.first;
        int v = p.first.second;

        int uSet = set.findSet(u);
        int vSet = set.findSet(v);
        if (uSet != vSet) {
            set.Union(u, v);
            edgeCount++;
        } else {
            return false;
        }
    }
    return (edgeCount == n - 1); 
}


int findWeight(const vector< pair< pair<int,int>, int> > & tree)
{
    int weight=0;
    for(pair< pair<int,int>, int> p : tree){
        weight+=p.second;
    }
    return weight;
}

vector< vector< pair< pair<int,int>, int> > > findAllMST(const vector< vector< pair< pair<int,int>, int> > > & combinations, int minWeight, int n)
{
    vector< vector< pair< pair<int,int>, int> > > allMST;
    for(vector< pair< pair<int,int>, int> > graph: combinations){
        if(filter(graph, n) && findWeight(graph)==minWeight){
                allMST.push_back(graph);
        }
    }
    return allMST;
}


int main()
{
    int n, m; //m=no. of vertices and n=no. of edges

    cin>>n>>m;

    vector<pair<pair<int,int> , int> > edges(m);

    formGraph(m,edges);

    sort(edges.begin(),edges.end(),compare);

    vector< vector< pair< pair<int,int>, int> > > combinations= getCombination(edges, m, n-1);

    vector<pair<pair<int,int> , int> > MST;
    findMST(n, edges ,MST);
    
    int minWeight= findWeight(MST);

    vector< vector< pair< pair<int,int>, int> > > allMST= findAllMST(combinations, minWeight, n);

    printAllMSTs(allMST);

    
    return 0;
}