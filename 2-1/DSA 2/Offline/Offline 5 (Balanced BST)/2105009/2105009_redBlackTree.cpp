#include "2105009_redBlackTree.hpp"

//#include <iostream>
//using namespace std;



//functions of Node Class

template<typename keyType, typename valueType>

Node<keyType,valueType>::Node (int color, Node* parent, keyType key, valueType value, Node* left, Node* right){
        //only the color has non de

    this->key=key;
    this->value=value;
    this->color=color;

    this->parent=parent;
    this->left=left;
    this->right=right;
}



//functions for red black tree

//private
template<typename nodeType>
    void redBlackTree<nodeType>::leftRotate(nodeType* x){
        nodeType* y= x->right;
        x->right=y->left;

        if(y->left!=nilNode){
            y->left->parent=x;
        }

        y->parent=x->parent;

        if(x->parent==nilNode){
            root=y;
        }
        else if(x==x->parent->left){
            x->parent->left=y;
        }
        else{
            x->parent->right=y;
        }

        y->left=x;
        x->parent=y;
    }

template<typename nodeType>
    void redBlackTree<nodeType>::rightRotate(nodeType* x){
        nodeType* y= x->left;
        x->left=y->right;

        if(y->right!=nilNode){
            y->right->parent=x;
        }

        y->parent=x->parent;

        if(x->parent==nilNode){
            root=y;
        }
        else if(x==x->parent->right){
            x->parent->right=y;
        }
        else{
            x->parent->left=y;
        }

        y->right=x;
        x->parent=y;
    }


template<typename nodeType>
    void redBlackTree<nodeType>::insertFix(nodeType* newNode){
        while(newNode->parent->color == RED){
            if(newNode->parent == newNode->parent->parent->left){
                nodeType* y= newNode->parent->parent->right;

                if(y->color==RED){
                    newNode->parent->color=BLACK;
                    y->color=BLACK;
                    newNode->parent->parent->color=RED;
                    newNode=newNode->parent->parent;
                }

                else{
                    if(newNode==newNode->parent->right){
                        newNode=newNode->parent;
                        leftRotate(newNode);
                    }
                    newNode->parent->color=BLACK;
                    newNode->parent->parent->color=RED;
                    rightRotate(newNode->parent->parent);
                }
            }
            else{
                nodeType* y= newNode->parent->parent->left;

                if(y->color==RED){
                    newNode->parent->color=BLACK;
                    y->color=BLACK;
                    newNode->parent->parent->color=RED;
                    newNode=newNode->parent->parent;
                }

                else{
                    if(newNode==newNode->parent->left){
                        newNode=newNode->parent;
                        rightRotate(newNode);
                    }
                    newNode->parent->color=BLACK;
                    newNode->parent->parent->color=RED;
                    leftRotate(newNode->parent->parent);
                }
            }
        }

        root->color=BLACK;
    }


template<typename nodeType>
    void redBlackTree<nodeType>:: eraseFixUp(nodeType* x){
        while(x!=root && x->color==BLACK){
            if(x==x->parent->left){
                nodeType* w= x->parent->right;
                if(w->color==RED){
                    w->color=BLACK;
                    x->parent->color=RED;
                    leftRotate(x->parent);
                    w=x->parent->right;
                }
                if(w->left->color==BLACK && w->right->color==BLACK){
                    w->color=RED;
                    x=x->parent;
                }
                else{
                    if(w->right->color==BLACK){
                        w->left->color=BLACK;
                        w->color=RED;
                        rightRotate(w);
                        w=x->parent->right;
                    }
                    w->color=x->parent->color;
                    x->parent->color=BLACK;
                    w->right->color=BLACK;
                    leftRotate(x->parent);
                    x=root;
                } 
                
                
            }
            else{
                nodeType* w= x->parent->left;
                if(w->color==RED){
                    w->color=BLACK;
                    x->parent->color=RED;
                    rightRotate(x->parent);
                    w=x->parent->left;
                }
                if(w->right->color==BLACK && w->left->color==BLACK){
                    w->color=RED;
                    x=x->parent;
                }
                else{
                    if(w->left->color==BLACK){
                        w->right->color=BLACK;
                        w->color=RED;
                        leftRotate(w);
                        w=x->parent->left;
                    }
                    w->color=x->parent->color;
                    x->parent->color=BLACK;
                    w->left->color=BLACK;
                    rightRotate(x->parent);
                    x=root;
                } 
            }
        }
        x->color=BLACK;
    }

    template<typename nodeType>
        void redBlackTree<nodeType>:: transplant(nodeType* u, nodeType* v){
        if(u->parent==nilNode){
            root=v;
        }
        else if(u==u->parent->left){
            u->parent->left=v;
        }
        else{
            u->parent->right=v;
        }
        v->parent=u->parent;
    }

    template<typename nodeType>
        nodeType* redBlackTree<nodeType>:: findMin(nodeType* root){
        if(root->left==nilNode){
            return root;
        }

        root=root->left;
    }

    template<typename nodeType>
    void redBlackTree<nodeType>:: printHelper(nodeType* root){ 
        if(root==nilNode) return;

        if(root->color==RED){
            cout<<"\x1b[31m"<<root->key<<"_"<<root->value<<"\x1b[0m";
        }   
        else cout<<root->key<<"_"<<root->value;

        if(root->left!=nilNode || root->right!=nilNode)cout<<"(";
        printHelper(root->left);
        if(root->left!=nilNode || root->right!=nilNode)cout<<",";
        printHelper(root->right);
        if(root->left!=nilNode || root->right!=nilNode)cout<<")";
    }

   template<typename nodeType>

    void redBlackTree<nodeType>:: inOrder(nodeType* root){
        if(root==nilNode) return;

        inOrder(root->left);
        if(root->color==RED){
            cout<<"\x1b[31m"<<root->key<<"\x1b[0m"<<" => "<<"\x1b[31m"<<root->value<<"\x1b[0m"<<endl;
        }  
        else cout<<root->key<<" => "<<root->value<<endl;
        inOrder(root->right);
    }

 template<typename nodeType>
nodeType* redBlackTree<nodeType>:: findHelper(int key){
        nodeType* temp=root;
        while(temp!=nilNode){
        if(key<temp->key){
            temp=temp->left;
            //cout<<"left key: "<<temp->key;
        }
        else if(key>temp->key){
            temp=temp->right;
            //cout<<"right key: "<<temp->key;
        }
        // else if(temp==nilNode){
        //     cout<<"Nilnode returned\n\n";
        //     return nilNode;
        // }
        else{
            //cout<<"Found: "<<temp->key<<", "<<temp->value<<endl;
            return temp;
        }

        }
    }

//public functions

template<typename nodeType>
redBlackTree<nodeType>:: redBlackTree(){
          nilNode=new nodeType(BLACK);
          //root=new nodeType(color=BLACK,parent=nilNode);
          
          root =nilNode;    /////

          nodeNum=0;
}


template<typename nodeType>
void redBlackTree<nodeType>:: insert(nodeType* newNodeVal){

        nodeNum++;
        
        
        // cout<<"\nRoot: "<<getRoot()->key<<endl<<endl;

        nodeType* y= nilNode;
        nodeType* x= root;

        nodeType* newNode= newNodeVal;

        while(x!=nilNode){
            y=x;
            if(newNode->key < x->key){
                x=x->left;
            }
            else if(newNode->key > x->key){  //to avoid duplicate keys (???)
                x=x->right;
            }
            else{
                x->value=newNode->value;
                return;
            }
        }
        newNode->parent= y;
        if(y==nilNode){
            root=newNode;  

            //newNode->parent=nilNode;
        }
        else if(newNode->key < y->key){
            y->left=newNode;
        }
        else{  //to avoid duplicate keys (??????)
            y->right=newNode;
        }

        newNode->left= newNode->right= nilNode;
        newNode->color=RED;

        insertFix(newNode);

        // cout<<"\nRoot after Fix: "<<root->key<<endl<<endl;
        // cout<<"Left: "<<root->left->key<<endl;
        // cout<<"Right: "<<root->right->key<<endl;
        // cout<<"Parent: "<<root->parent->key<<endl;
}

template<typename nodeType>
void redBlackTree<nodeType>:: erase(int key){
        if(!find(key)) return;   //checking whether the key exists
        
        nodeType*z = findHelper(key);  //returns pointer to the key to be deleted

        nodeNum--;

        nodeType* y =z;
        int yOriginalColor=y->color;

        nodeType* x;

        if(z->left==nilNode){
            x=z->right;

            transplant(z,z->right);
        }
        else if(z->right==nilNode){
            x=z->left;

            transplant(z,z->left);
        }

        else{
            y=findMin(z->right);
            yOriginalColor=y->color;
            x=y->right;

            if(y->parent==z){
                x->parent=y;
            }
            else{
                transplant(y,y->right);
                y->right=z->right;
                y->right->parent=y;

            }

            transplant(z,y);
            y->left=z->left;
            y->left->parent=y;
            y->color=z->color;
        }

        
        if(yOriginalColor==BLACK){
            eraseFixUp(x);
        }

}

template<typename nodeType>
bool redBlackTree<nodeType>:: find(int key){
        nodeType* temp=root;
        while(temp!=nilNode){
        if(key<temp->key){
            temp=temp->left;
            //cout<<"left key: "<<temp->key;
        }
        else if(key>temp->key){
            temp=temp->right;
            //cout<<"right key: "<<temp->key;
        }
        // else if(temp==nilNode){
        //     cout<<"Nilnode returned\n\n";
        //     return nilNode;
        // }
        else{
            return true;
        }

        }

        return false;
    }

template<typename nodeType>
int redBlackTree<nodeType>:: size(){
        return nodeNum;
}


template<typename nodeType>
bool redBlackTree<nodeType>:: empty(){
        return nodeNum==0 ? true : false;
}

template<typename nodeType>
void redBlackTree<nodeType>:: iterate(){
        inOrder(root);
}

template<typename nodeType>
void redBlackTree<nodeType>:: clear(){
        free(nilNode);

          nilNode=new nodeType(BLACK);
          //root=new nodeType(color=BLACK,parent=nilNode);
          
          root =nilNode;    /////

          nodeNum=0;
}

template<typename nodeType>
void redBlackTree<nodeType>:: print(){
        printHelper(root);
}