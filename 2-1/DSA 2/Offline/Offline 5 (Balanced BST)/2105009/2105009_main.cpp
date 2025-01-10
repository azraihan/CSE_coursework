#include <iostream>
#include <string>

#include "2105009_map.hpp"

using namespace std;

int main()
{
    freopen("input.txt","r",stdin);
    //freopen("output.txt","w",stdout);
    
    map<int,string> m;

    string command;
    int commandCount=0;

    while(cin>>command){
        if(commandCount!=0) std::cout<<endl;
        std::cout<<"Cmd "<<++commandCount<<": ";
        
        if(command=="I"){
        int key;
        string value;

        cin>>key>>value;

        std::cout<<command<<" "<<key<<" "<<value<<endl;

        m.insert(key,value);
        m.print();

        std::cout<<endl;
        }

        else if(command=="E"){
        int key;
        cin>>key;

        std::cout<<command<<" "<<key<<" "<<endl;

        if(m.find(key)){
            m.erase(key);
            m.print();
        }
        else{
            std::cout<<key<<" not found";
        }
        std::cout<<endl;
        }

        else if(command=="Clr"){
            std::cout<<command<<endl;

            if(m.size()==0) std::cout<<"unsuccessful\n";
            else{
                m.clear();
                std::cout<<"successful\n";
            }
        }

        else if(command=="F"){
            int key;
            cin>>key;

            std::cout<<command<<" "<<key<<endl;

            m.find(key) ? std::cout<<key<< " found\n" : std::cout<<key<< " not found\n";
        }

        else if(command=="Em"){
            std::cout<<command<<endl;

            m.empty()? std::cout<<"yes\n" : std::cout<<"no\n";
        }

        else if(command=="S"){
            std::cout<<command<<endl;

            std::cout<<m.size()<<endl;
        }

        else if(command=="Itr"){
            std::cout<<command<<endl;

            m.iterate();
        }

        
    }

    //cout<<"\n\nTerminated...";

    return 0;
}