#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

template <typename keyType=int, typename valueType=int>
class Node{
    public:

    Node* parent;
    Node* child;

    vector<Node*> childList;
    Node* childListHead;
    Node* childListTail;

    //only these two not in the default constructor
    Node* left;  //If node y is an only child, then y.left=y.right=y
    Node* right;

    int degree;
    bool marked;
    
    keyType key;
    valueType value;

    Node(keyType key, valueType value, bool marked=false, Node* parent=NULL, Node* child=NULL, Node* childListHead=NULL, Node* childListTail=NULL){  //assign the other values manually
        this->key= key;
        this->value=value;

        this->marked=marked;

        this->parent=parent;
        this->child=child;

        this->childListHead=childListHead;
        this->childListTail=childListTail;

        left=right=this;
        degree=0;

        childList.clear();
    }
};


template <typename keyType=int, typename valueType=int>
class fibonacciHeap{
    public:
    vector<Node<keyType,valueType> * > rootList;
    Node<keyType,valueType> * maxRoot;   //max value in the heap

    Node<keyType,valueType> * rootListTail;
    Node<keyType,valueType> * rootListHead;

    int nodeNum; //number of nodes currently in the heap

    fibonacciHeap(){
        nodeNum=0;
        rootList.clear();

        maxRoot=rootListHead=rootListTail=NULL;
    }
};


template <typename keyType, typename valueType>
class priorityQueue{

    vector<Node<keyType,valueType> * > nodes;       //indexed according to the values

    void fibHeapLink(fibonacciHeap<keyType,valueType> & fibHeap, 
                     Node<keyType,valueType> *y, Node<keyType,valueType> *x){
        auto it=find(fibHeap.rootList.begin(), fibHeap.rootList.end(), y);
        if(it!=fibHeap.rootList.end()) fibHeap.rootList.erase(it);

        
        if(fibHeap.rootListHead==y) 
        {
            fibHeap.rootListHead=y->right;
        }
        if(fibHeap.rootListTail==y){
            fibHeap.rootListTail=y->left;
        }
            y->right->left=y->left;
            y->left->right=y->right;
        
        if((x->childList).empty()) 
        {
            x->childListHead=x->childListTail=x->child=y;
            y->right=y->left=y;
        }

        else{
            y->right=x->childListHead;
            x->childListHead->left=y;    
            x->childListHead=y;

            x->childListHead->left=x->childListTail;
            x->childListTail->right=x->childListHead;
        }

        x->childList.push_back(y);

        y->parent=x;

        (x->degree)++;

        y->marked=false;
    }
    
    void consolidate(fibonacciHeap<keyType,valueType> & fibHeap){

        int D=ceil(log2(fibHeap.nodeNum));

        vector<Node<keyType,valueType> * > A(D+1,NULL);  //assign all the elements to NULL

        Node<keyType,valueType> * w= fibHeap.maxRoot;
        Node<keyType,valueType> * prevToW=w->left;

        Node<keyType,valueType> * x=w;

        int currentRootListSize=fibHeap.rootList.size();
        for(int i=0;i<currentRootListSize;i++){
            int deg= x->degree;

            Node<keyType,valueType> * y;

            Node<keyType,valueType> * temp;
            Node<keyType,valueType> * next;

            next=x->right;
            while(A[deg]!=NULL){
                y=A[deg];

                if(x->key < y->key){
                    //exchange x and y
                    temp=x;
                    x=y;
                    y=temp;
                }

                //call fib link here
                fibHeapLink(fibHeap,y,x);

                A[deg]=NULL;
                deg++;

            }
            A[deg]=x;

            x=next;
        }

        fibHeap.maxRoot=NULL;

        for(int i=0; i<=D; i++){
            if(A[i]!=NULL){
                if(fibHeap.maxRoot==NULL){
                    fibHeap.rootList.clear();
                    fibHeap.rootList.push_back(A[i]);
                    fibHeap.maxRoot=A[i];
                }

                else{
                    fibHeap.rootList.push_back(A[i]);

                    if(A[i]->key>fibHeap.maxRoot->key){
                        fibHeap.maxRoot=A[i];
                    }
                }
            }
        }
    }

    void cut(fibonacciHeap<keyType,valueType> & fibHeap, Node<keyType,valueType> *x, Node<keyType,valueType> *y){
            if(y->childListHead==x) y->childListHead=x->right;
            if(y->childListTail==x) y->childListTail=x->left;   
              
            Node<keyType,valueType> * leftOfX= x->left;
            Node<keyType,valueType> * rightOfX= x->right;

            leftOfX->right=rightOfX;
            rightOfX->left=leftOfX; 

        
        if(y->child==x){
            if((y->childList).size()>1){
                y->child=y->child->right;
            }
            else{
                y->childListHead=y->childListTail=y->child=NULL;
                y->childList.clear();
            }
        }
        
        auto it=find(y->childList.begin(), y->childList.end(), x);

        if(it!=y->childList.end()){
            y->degree -- ;
            (y->childList).erase(it);
        

        }

        //add x to the root list
        x->right=fibHeap.rootListHead;
        fibHeap.rootListHead->left=x;

        x->left=fibHeap.rootListTail;
        fibHeap.rootListTail->right=x;

        fibHeap.rootListHead=x;

        x->parent=NULL;
        x->marked=false;

        fibHeap.rootList.insert(fibHeap.rootList.begin(),x);
    }

    void cascadingCut(fibonacciHeap<keyType,valueType> & fibHeap, Node<keyType,valueType> *y){
        Node<keyType,valueType>* z= y->parent;

        if(z!=NULL){
            if(!y->marked) y->marked=true;

            else{
                cut(fibHeap,y,z);
                cascadingCut(fibHeap,z);
            }
        }
    }

    void printHelper(vector<Node<keyType,valueType>* > childList){
        if(childList.empty()) return;

        else{
            for(Node<keyType,valueType>* child: childList){
                cout<<"("<<child->key<<","<<child->value<<"), ";
            }
            cout<<endl;
            for(Node<keyType,valueType>* child: childList){
                cout<<"("<<child->key<<","<<child->value<<")-> ";
                if(child->childList.size()==0) cout<<endl;
                else printHelper(child->childList);
            }
            cout<<endl;
        }
    }

    
    public:
    priorityQueue(){
        nodes= vector<Node<keyType,valueType> * >(100001,NULL);  //since 0 <= value <= 100000
    }

    fibonacciHeap<keyType,valueType> makeHeap(){
        return fibonacciHeap<keyType,valueType>();
    }

    bool isEmpty(const fibonacciHeap<keyType,valueType> & fibHeap){
        return fibHeap.nodeNum==0 ? true:false;
    }

    void insert(fibonacciHeap<keyType,valueType> & fibHeap, keyType key, valueType value){
       Node<keyType,valueType>* newNode= new Node<keyType,valueType>(key,value);

       nodes[newNode->value]=newNode;

       newNode->degree=0;  // although already assigned as zero in the constructor
       //auto assigns child and parent as NULL. And marked attribute as false

       if(fibHeap.maxRoot==NULL){
        //constructor already has been called
        fibHeap.rootList.push_back(newNode);  //write this line only once??
        fibHeap.maxRoot=newNode;

        fibHeap.rootListHead=fibHeap.rootListTail=newNode;
       }
       else{
        fibHeap.rootList.push_back(newNode);  //write this line only once??

        //pointer exchanges
        fibHeap.rootListTail->right=newNode;
        newNode->left=fibHeap.rootListTail;

        fibHeap.rootListHead->left=newNode;
        newNode->right=fibHeap.rootListHead;

        fibHeap.rootListTail=newNode;


        if(newNode->key>fibHeap.maxRoot->key){
            fibHeap.maxRoot=newNode;
        }
       }
       fibHeap.nodeNum++;
    }

    fibonacciHeap<keyType,valueType> meld(fibonacciHeap<keyType,valueType> fibHeap1, fibonacciHeap<keyType,valueType> fibHeap2){
        fibonacciHeap<keyType,valueType> fibHeap= fibonacciHeap<keyType,valueType>();

        fibHeap.maxRoot=fibHeap1.maxRoot;

        //concatenate the root lists
        fibHeap1.rootListTail->right=fibHeap2.rootListHead;
        fibHeap2.rootListHead->left=fibHeap1.rootListTail;

        fibHeap2.rootListTail->right=fibHeap1.rootListHead;
        fibHeap1.rootListHead->left=fibHeap2.rootListTail;

        fibHeap.rootListHead=fibHeap1.rootListHead;
        fibHeap.rootListTail=fibHeap2.rootListTail;

        if(fibHeap1.maxRoot==NULL || (fibHeap2.maxRoot!=NULL && fibHeap2.maxRoot->key> fibHeap1.maxRoot->key)){
            fibHeap.maxRoot=fibHeap2.maxRoot;
        }

        fibHeap.nodeNum=fibHeap1.nodeNum+fibHeap2.nodeNum;
        fibHeap1.rootList.insert(fibHeap1.rootList.end(), fibHeap2.rootList.begin(), fibHeap2.rootList.end());
        fibHeap.rootList=fibHeap1.rootList;

        return fibHeap;
    }

    valueType extractMax(fibonacciHeap<keyType,valueType>& fibHeap){
        // for(Node <keyType, valueType> * n: fibHeap.rootList){
        //     cout<<n->key<<" "<<n->value<<endl; 
        // }
        // cout<<endl;
        
        keyType key=fibHeap.maxRoot->key;
        valueType value=fibHeap.maxRoot->value;

        nodes[value]=NULL;

        if(fibHeap.maxRoot !=NULL){
            auto iteratorToPrevMax= find(fibHeap.rootList.begin(), fibHeap.rootList.end(), fibHeap.maxRoot);
            if(iteratorToPrevMax!=fibHeap.rootList.end()) fibHeap.rootList.erase(iteratorToPrevMax);

        // for(Node <keyType, valueType> * n: fibHeap.rootList){
        //     cout<<n->key<<" "<<n->value<<endl; 
        // }
        //cout<<endl;



            Node<keyType,valueType>* nodePrevToMax=fibHeap.maxRoot->left;
            Node<keyType,valueType>* nodeNextToMax=fibHeap.maxRoot->right;

            //cout<<"Prev: "<<nodePrevToMax->key<<" "<<nodePrevToMax->value<<endl;
            //cout<<"Next: "<<nodeNextToMax->key<<" "<<nodeNextToMax->value<<endl;

            //removing from cdll
            nodePrevToMax->right=nodeNextToMax;
            nodeNextToMax->left=nodePrevToMax;

        // Node<keyType,valueType>* m=fibHeap.rootListHead;
        // cout<<"Rootlist: \n";
        // do{
        //     cout<<"("<<m->key<<","<<m->value<<")\n";
        //     m=m->right;
        // }while(m!=fibHeap.rootListHead);

            if(fibHeap.maxRoot==fibHeap.rootListHead){
                fibHeap.rootListHead=fibHeap.rootListHead->right;
                //cout<<"New root list head: "<<fibHeap.rootListHead->key<<" "<<fibHeap.rootListHead->value<<endl;
            }
            if(fibHeap.maxRoot==fibHeap.rootListTail) 
            {
                fibHeap.rootListTail=fibHeap.rootListTail->left;
                //cout<<"New root list tail: "<<fibHeap.rootListTail->key<<" "<<fibHeap.rootListTail->value<<endl;
            }

            Node<keyType,valueType>* lastAddedNode=NULL;

        //     Node<keyType,valueType>* p=fibHeap.rootListHead;
        //         cout<<"Rootlist: \n";
        // do{
        //     cout<<"("<<p->key<<","<<p->value<<")\n";
        //     p=p->right;
        // }while(p!=fibHeap.rootListHead);


        // for(Node <keyType, valueType> * n: fibHeap.rootList){
        //     cout<<n->key<<" "<<n->value<<endl; 
        // }

            
            if(nodeNextToMax!=fibHeap.maxRoot && nodePrevToMax!=fibHeap.maxRoot){
                //cout<<"Gets in\n\n";
                for(int i=0; i<(fibHeap.maxRoot->childList).size(); i++){
                fibHeap.rootList.insert(iteratorToPrevMax,(fibHeap.maxRoot->childList)[i]);
                iteratorToPrevMax++;

                // for(Node<keyType,valueType>* n: fibHeap.rootList){
                //     cout<<n->key<<" "<<n->value<<endl;
                // }

                //cout<<i<<endl;

                if(i==0){
                    //cout<<"yooooo1\n";
                    nodePrevToMax->right=(fibHeap.maxRoot->childList)[i];
                    (fibHeap.maxRoot->childList)[i]->left=nodePrevToMax;

                    if(fibHeap.maxRoot->childList.size()==1){
                        nodeNextToMax->left=(fibHeap.maxRoot->childList)[i];
                        (fibHeap.maxRoot->childList)[i]->right=nodeNextToMax;
                    }
                }

                if(lastAddedNode!=NULL && i==(fibHeap.maxRoot->childList).size()-1 ){
                    //cout<<"yooooo2\n";
                    lastAddedNode->right=(fibHeap.maxRoot->childList)[i];
                    (fibHeap.maxRoot->childList)[i]->left=lastAddedNode;
                    nodeNextToMax->left=(fibHeap.maxRoot->childList)[i];
                    (fibHeap.maxRoot->childList)[i]->right=nodeNextToMax;
                }
                

                else if(i!=0 && i!=(fibHeap.maxRoot->childList).size()-1){
                    lastAddedNode->right=(fibHeap.maxRoot->childList)[i];
                    (fibHeap.maxRoot->childList)[i]->left=lastAddedNode;
                }

                lastAddedNode=(fibHeap.maxRoot->childList)[i];

                (fibHeap.maxRoot->childList)[i]->parent=NULL;
             }
            }
            

            else{
                //cout<<"Gets in 2\n\n";
                for(int i=0; i<(fibHeap.maxRoot->childList).size(); i++){
                fibHeap.rootList.push_back((fibHeap.maxRoot->childList)[i]);

                (fibHeap.maxRoot->childList)[i]->parent=NULL;

                nodeNextToMax=(fibHeap.maxRoot->childList)[i];
             }
            }

        //print(fibHeap);

                fibHeap.maxRoot=nodeNextToMax;

                //consolidate here
                consolidate(fibHeap);

            fibHeap.nodeNum--;
        }

        return value;
    }


    void increaseKey(fibonacciHeap<keyType,valueType>& fibHeap, valueType value, keyType newKey){
        Node<keyType,valueType>* currentNode= nodes[value];

        if(currentNode==NULL){
            cout<<"No such value exists\n\n";
            return;
        }

        if(newKey < currentNode->key) 
        {
            cout<<"Error! New key is less than the current key.\n";
            return;
        }

        // Node<keyType,valueType>* q=fibHeap.rootListHead;
        // cout<<"Old Rootlist: \n";
        // do{
        //     cout<<"("<<q->key<<","<<q->value<<")\n";
        //     q=q->right;
        // }while(q!=fibHeap.rootListHead);

        currentNode->key=newKey;

        Node<keyType,valueType>* y=currentNode->parent;

        if(y!=NULL && currentNode->key > y->key){
            //cout<<"Cutting and Cascading\n\n"<<endl;
            //cut
            cut(fibHeap,currentNode,y);
            //cascade cut
            cascadingCut(fibHeap,y);
        }

        if(currentNode->key > fibHeap.maxRoot->key){
            fibHeap.maxRoot=currentNode;
        }

        Node<keyType,valueType>* p=fibHeap.rootListHead;

        // cout<<"Rootlist: \n";
        // do{
        //     cout<<"("<<p->key<<","<<p->value<<")\n";
        //     p=p->right;
        // }while(p!=fibHeap.rootListHead);

    }

    void deleteValue(fibonacciHeap<keyType,valueType>& fibHeap, valueType value){
        if(nodes[value]!=NULL){
            increaseKey(fibHeap,value,(valueType)INT_MAX);
            extractMax(fibHeap);
        }
    }

    valueType findMax(fibonacciHeap<keyType,valueType>& fibHeap){
        return fibHeap.maxRoot->value;
    }

    void print(const fibonacciHeap<keyType,valueType>& fibHeap){
        for(int i=0; i<fibHeap.rootList.size();i++){
            int treeIndex=i+1;

            cout<<"\nTree "<<treeIndex<<":\n";

            cout<<"("<<fibHeap.rootList[i]->key<<","<<fibHeap.rootList[i]->value<<")-> ";

            if(fibHeap.rootList[i]->childList.size()==0) cout<<endl;
            else
            {
                printHelper(fibHeap.rootList[i]->childList);
                cout<<endl;
            }
        }
    }

    void test()
    {
        fibonacciHeap<keyType,valueType> heap1 = makeHeap();
        fibonacciHeap<keyType,valueType> heap2 = makeHeap();
        // testing duplicate keys
        
        cout << (isEmpty(heap1) ? "empty" : "not empty") << endl;
        insert(heap1, 1, 5);
        insert(heap1, 1, 6);
        insert(heap1, 1, 7);
        print(heap1);
        cout << extractMax(heap1)<< endl;
        print(heap1);
        cout << (isEmpty(heap1) ? "empty" : "not empty") << endl;
        //(1, 6) => (8, 6)
        increaseKey(heap1, 6, 8);
        print(heap1);
        cout << findMax(heap1) << endl;
        print(heap1);
        // deleting values that don't exist
        deleteValue(heap1, 10);
        print(heap1);
        // increasing values that don't exist
        increaseKey(heap1, 12, 9);
        print(heap1);
        // valid increase_key
        increaseKey(heap1, 7, 12);
        print(heap1);
        // valid deletion
        deleteValue(heap1, 7);
        deleteValue(heap1, 6);
        print(heap1);
        cout << "Tree state: " << (isEmpty(heap1) ? "empty" : "not empty") << endl;
        // insertion at null tree
        insert(heap1, 1, 5);
        insert(heap1, 2, 6);
        print(heap1);


        //mass insertion
        insert(heap1,31,101);
        insert(heap1,-19,102);
        insert(heap1,42,103);
        insert(heap1,2,104);

        print(heap1);

        //extracting max after mass insertion
        cout<<extractMax(heap1)<<endl;
        print(heap1);

        //inxrease key after mass insertion
        increaseKey(heap1,104,29);
        print(heap1);

        //deletion after mass insertion
        deleteValue(heap1,101);
        print(heap1);

        //second Heap
        for (int i = 1; i <= 12; i++)
        {
            insert(heap2, i, i + 190 + (200 * i));
        }

        cout << "MELDING TEST" << endl;

        heap2 = meld(heap2, heap1);
        print(heap2);
        cout << extractMax(heap2) << endl;
        print(heap2);
        //print(heap1);
        insert(heap2, 100, 500);
        print(heap2);

        cout<<"\n\n\nPassed\n";

    }
};


int main()
{

    priorityQueue<int,int> pq;

    pq.test();

    return 0;
}