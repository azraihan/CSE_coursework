#ifndef _2105009_MAP
#define _2105009_MAP

#include "2105009_redBlackTree.hpp"
#include <iostream>

using namespace std;


template <typename keyType, typename valueType>
class map{
    redBlackTree<Node<keyType,valueType> > rbTree;

    public:

    map(){
        rbTree=redBlackTree<Node<keyType,valueType> >();
    }

    void insert(keyType key, valueType value){
        Node<keyType,valueType>* newNode=new Node<int,string>(RED,NULL,key,value,NULL,NULL);

        rbTree.insert(newNode);
    }

    void erase (keyType key){
        if(rbTree.find(key)) rbTree.erase(key);
    }

    bool find(keyType key){
        return rbTree.find(key);
    }

    int size(){
        return rbTree.size();
    }

    void clear(){
        rbTree.clear();
    }

    bool empty(){
        return rbTree.empty();
    }

    void iterate(){
        rbTree.iterate();
    }

    void print(){
        rbTree.print();
    }
};


#endif