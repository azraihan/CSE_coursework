#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <set>

using namespace std;


#define baseOfHashing 31
#define baseOfAuxiliaryHashing 29

#define customConstant1 37
#define customConstant2 41

#define WORD_NUMBER 10000
#define MAX_CHAIN_LENGTH 3
#define MAX_LOAD_FACTOR 0.6

#define BEFORE_REHASH true
#define AFTER_REHASH false

#define SEED 69

enum hashingType
{
    CHAIN,
    DOUBLE,
    CUSTOM
};

enum hashFunctionType
{
    type1,
    type2
};

class listOfPrimes
{
    vector<int> flagged, primes;
    int maxLimit;

public:
    listOfPrimes(int n)
    {
        flagged=vector<int>(n + 5, 0);
        maxLimit = sqrt(n + 1);
        flagged[1] = -1;

        primes.push_back(2);
        flagged[2] = 2;

        for (int i = 4; i <= n; i += 2) flagged[i] = 2;   //flagging all multiples of 2
        
        for (int i = 3; i <= n; i += 2)  //sieve
        {
            if (!flagged[i])
            {
                flagged[i] = i;
                primes.push_back(i);

                if (i > maxLimit) continue;

                for (int j = i * i; j <= n; j += i * 2)
                    flagged[j] = i;
            }
        }
    }
    int nextPrime(int n)
    {
        if (n > primes.back()) return n;
        return *lower_bound(primes.begin(), primes.end(), n);
    }
};

listOfPrimes primes (10000000);

class hashObject{
    public:
        string key;
        int value;
        hashObject(string key = "", int value = 0)
        {
            this->key = key;
            this->value = value;
        }

        bool operator==(const hashObject& other) const {
        return key == other.key && value == other.value;
        }
};


class hashTable{
    int tableSize;
    int initialTableSize;

    hashingType typeOfHashing;

    vector<vector<hashObject> > table;

    double loadFactor;

    
    vector<int> chainLength;
    int maxChainLength;
    int currentMaxChainLength;

    hashFunctionType typeOfHashFunction;

    vector<hashObject> tempHashObjectList;
    hashObject dummyObject;

    int hashBase, auxHashBase;

    int numberOfInsertions;
    int numberOfDeletions;

    int count;

    int C1=customConstant1, C2= customConstant2;

    bool insideRehash;
    bool insideRehash2;
    bool insideDelete;
    bool insideInsert;
    bool insideReportForHash;

    int itemsSearched;


int hash1(const std::string& str) {
    unsigned long hash = 5381; // Start with a large prime number
    for (char c : str) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c); // hash * 33 + c
    }
    return (static_cast<int>(hash % INT_MAX))%tableSize; // Ensure result is positive and fits in an int
}

int hash2(const std::string& str) {
    unsigned long hash = 0; // Start with 0
    for (char c : str) {
        hash = static_cast<unsigned char>(c) + (hash << 6) + (hash << 16) - hash; // Similar to hash * 65599
    }
    return (static_cast<int>(hash % INT_MAX))%tableSize; // Ensure result is positive and fits in an int
}


    int hash(const string& key)
    {
        if (typeOfHashFunction==type1) return hash1(key);
        
        return hash2(key);
    }

    int auxHash(string key)
    {
        int ahash = 0;
        for (char c : key)
            ahash = ((ahash + c - 'a') * auxHashBase);
        ahash = table.size() - ahash % table.size();
        return ahash;
    }

    int doubleHash(const string & key, int i){ //i=probe number
        int hashKey=(hash(key)+i*auxHash(key))%tableSize;

        if(hashKey<0) hashKey+=tableSize;

        return hashKey;
    }

    int customHash(const string & key, int i){
        int hashKey=(hash(key)+ C1*i*auxHash(key)+ C2*i*i)%tableSize;
\
        if(hashKey<0) hashKey+=tableSize;

        return hashKey;
    }

    void rehash(){  //done only for chaining
        if(typeOfHashing==CHAIN){
            cout<<"Rehash Triggered (While Inserting)\n";
            reportForRehash(BEFORE_REHASH);
        }
        
        insideRehash=true;

        this->tableSize= tableSize*2;

        table.clear();
        table=vector<vector<hashObject> >(primes.nextPrime(tableSize),tempHashObjectList);

        this->chainLength=vector<int>(table.size(),0);
        this->currentMaxChainLength=0;
        

        int value=1;
        for(string s: currentWords){
            insert(s,value);
            value++;
        }

        loadFactor=loadFactor/2;

        insideRehash=false;

        if(typeOfHashing==CHAIN){
            reportForRehash(AFTER_REHASH); 
            cout<<endl<<endl;
        }
    }

        void rehash2(){  //done only for chaining
        if(typeOfHashing==CHAIN){
            cout<<"Rehash Triggered (While Deleteing)\n";
            reportForRehash(BEFORE_REHASH); 
        }

        insideRehash2=true;

        this->tableSize= tableSize/2;

        table.clear();
        table=vector<vector<hashObject> >(primes.nextPrime(tableSize),tempHashObjectList);

        this->chainLength=vector<int>(table.size(),0);
        this->currentMaxChainLength=0;
        

        int value=1;
        for(string s: currentWords){
            insert(s,value);
            value++;
        }

        loadFactor=loadFactor*2;

        insideRehash2=false;

        if(typeOfHashing==CHAIN){
            reportForRehash(AFTER_REHASH);
            cout<<endl<<endl; 
        }
    }

    public:
    int countProbes, countCollisions;

    vector<string> currentWords;
    //double loadFactor;

    hashTable(int tableSize, hashingType typeOfHashing, hashFunctionType typeOfHashFunction, int maxChainLength=0, int hbase = 31, int ahbase = 29)
    {
        this-> tableSize= this->initialTableSize=tableSize;

        this->typeOfHashing = typeOfHashing;

        tempHashObjectList.push_back(dummyObject);
        table=vector<vector<hashObject> >(primes.nextPrime(tableSize),tempHashObjectList);

        this->maxChainLength=maxChainLength;
        this->chainLength=vector<int>(table.size(),0);
        this->currentMaxChainLength=0;
        

        countProbes= countCollisions= 0;

        this->typeOfHashFunction = typeOfHashFunction;

        this->numberOfInsertions=this->numberOfDeletions=0;

        this->insideRehash=this->insideDelete=this->insideInsert=this->insideRehash2=this->insideReportForHash=false;

        this->itemsSearched=0;
        this->loadFactor=0;
    }

    bool insert(const string& key, int value){
        //cout<<"Inserting "<<key<< ", "<<value<<endl;
        insideInsert=true;
        
        if(search(key).first){
            return false;
        }

        if(typeOfHashing==CHAIN){
            int index= hash(key);

            if(table[index][0].key=="" && table[index][0].value==0){
                table[index].clear();
            }
            else{
                if(!insideRehash &&!insideRehash2) countCollisions++;
            }
            table[index].push_back(hashObject(key,value));
            numberOfInsertions++;

            currentWords.push_back(key);

            chainLength[index]++;

            if(!insideRehash && !insideRehash2) loadFactor=(loadFactor*tableSize+1)/tableSize;

            if(currentMaxChainLength<chainLength[index]){
                currentMaxChainLength=chainLength[index];
            }

            if(numberOfInsertions%100==0 && currentMaxChainLength>maxChainLength && !insideRehash && !insideRehash2){
                rehash();
            }

            insideInsert=false;
            return true;
        }


            int i=-1;
            int index;
            bool collisionOccured=false;
            do{
                i++;
                if(typeOfHashing==DOUBLE) index=doubleHash(key,i);
                else index= customHash(key,i);
            }while((table[index][0].key!="" && table[index][0].value!=0) && i<tableSize);

            if(i>0) {
                if(!insideRehash && !insideRehash2) countCollisions++;
            }

            if(i==tableSize && (table[index][0].key!="" && table[index][0].value!=0)){
                insideInsert=false;
                return false;
            }

            table[index].clear();
            table[index].push_back(hashObject(key,value));
            currentWords.push_back(key);

            if(!insideRehash && !insideRehash2) loadFactor=(loadFactor*tableSize+1)/tableSize;

            if(loadFactor>MAX_LOAD_FACTOR && !insideRehash &&!insideRehash2){
                rehash();
            }

            insideInsert=false;
            return true;

    }

    pair<bool,int> search(const string & key){
        if(!insideDelete && !insideInsert) itemsSearched++;
        if(insideReportForHash) {
            itemsSearched++;
        }

        if(typeOfHashing==CHAIN){
            int index= hash(key);
            
            if(table[index][0].key=="" && table[index][0].value==0){
                return make_pair(false,-1);
            }

            for(hashObject h: table[index]){
                if(!insideDelete && ! insideInsert) countProbes++;
                if(insideReportForHash) countProbes++;
                if(h.key==key){
                    return make_pair(true,index);
                }
            }

            return make_pair(false,-1);
        }
            int i=-1;
            int index;
            do{
                i++;
                if(typeOfHashing==DOUBLE) index=doubleHash(key,i);
                else index= customHash(key,i);

                if(!insideDelete && !insideInsert) countProbes++;
                if(insideReportForHash) countProbes++;

                if(table[index][0].key==key) return make_pair(true,index);


            }while((table[index][0].key!="" && table[index][0].value!=0) && i<=tableSize);

            return make_pair(false,-1);
    }
    

    bool deleteKey(const string & key){
        insideDelete=true;
        pair<bool,int> result=search(key);
        if(result.first){
            int index=result.second;

            if(typeOfHashing==CHAIN){
                for(auto it=table[index].begin(); it!=table[index].end(); it++){
                    if((*it).key==key){
                         table[index].erase(it);
                         if (table[index].empty()){
                            table[index].push_back(dummyObject);
                         }
                         numberOfDeletions++;
                         insideDelete=false;
                         chainLength[index]--;

                         loadFactor=(loadFactor*tableSize-1)/tableSize;

                         if(numberOfInsertions%100==0 && tableSize/2>initialTableSize){
                                currentMaxChainLength=-1;
                                for(int len: chainLength){
                                    if(currentMaxChainLength<len){
                                        currentMaxChainLength=len;
                                    }
                                }

                                if((double)currentMaxChainLength< 0.8*MAX_CHAIN_LENGTH){
                                    cout<<(double)currentMaxChainLength<<" "<<0.8*MAX_CHAIN_LENGTH<<endl;
                                    
                                    rehash2();
                                }
                         }
                         return true;
                    }
                }
            }

            else{//rehashing not done here
                table[index].clear();
                numberOfDeletions++;
                table[index].push_back(dummyObject);
                table[index][0].key=-1;             ///////////////

                loadFactor=(loadFactor*tableSize-1)/tableSize;
                
                insideDelete=false;
                return true;
            }
        }

        insideDelete=false;

        return false;
    }

    double countAverageProbes(){
        return (1.0*countProbes)/itemsSearched;
    }

    void reportForRehash(bool before){
        insideReportForHash=true;
        
        string status;
        if(before) status="Old";
        else status="New";

        cout<<status<<" Load Factor: "<<loadFactor<<endl;
        cout<<status<<" Max Chain Length: "<<currentMaxChainLength<<endl;
        

        //getting the probe count
        for(int i=0; i<currentWords.size()/100; i++){
            search(currentWords[rand()%currentWords.size()]);
        }
        cout<<status<<" Probe Count: "<<countAverageProbes()<<endl<<endl;;

        insideReportForHash=false;
    }

    int getSize(){
        return tableSize;
    }
};


string getRandomWord()
{
    string word = "";
    int len = 6 + rand() % 5;
    for (int i = 0; i < len; i++)
    {
        word.push_back((rand() % 26) + 'a');
    }
    return word;
}

vector<string> generateRandomWords(int n) //generates n random words
{
    vector<string> words;
    for(int i=0; i<n; i++){
        words.push_back(getRandomWord());
    }

    return words;
}





int main()
{
    srand(SEED);
    
    freopen("out.txt","w",stdout);
    vector<string> words= generateRandomWords(WORD_NUMBER);

    vector<int> tableSize(3);
    tableSize[0]=5000;
    tableSize[1]=10000;
    tableSize[2]=20000;

    for(int size: tableSize){
        cout<<".....................TABLE SIZE "<<size<<".....................\n\n";
        cout<<".....................INSERTING.....................\n\n";

        hashTable table1_chain=hashTable(size,CHAIN,type1,MAX_CHAIN_LENGTH);  //type1=hash1, type2=hash2
        hashTable table1_double=hashTable(size,DOUBLE,type1);
        hashTable table1_custom=hashTable(size,CUSTOM,type1);

        hashTable table2_chain=hashTable(size,CHAIN,type2,MAX_CHAIN_LENGTH); 
        hashTable table2_double=hashTable(size,DOUBLE,type2);
        hashTable table2_custom=hashTable(size,CUSTOM,type2);

        vector<int> wordsInserted(6,0); //which is also the value inserted along with the string key
        
        bool successful=false;
        for(string word:words){
                        successful=table1_chain.insert(word,wordsInserted[0]);  //checking whether the word exists is done inside the insert function
                        if(successful) wordsInserted[0]++;

                        successful=table1_double.insert(word,wordsInserted[1]);
                        if(successful) wordsInserted[1]++;

                        successful=table1_custom.insert(word,wordsInserted[2]);
                        if(successful) wordsInserted[2]++;

                        successful=table2_chain.insert(word,wordsInserted[3]);
                        if(successful) wordsInserted[3]++;

                        successful=table2_double.insert(word,wordsInserted[4]);
                        if(successful) wordsInserted[4]++;

                        successful=table2_custom.insert(word,wordsInserted[5]);
                        if(successful) wordsInserted[5]++;
        }
        

        for(int i=0; i<(size/100); i++){

            table1_chain.search(table1_chain.currentWords[rand()%(table1_chain.currentWords.size())]);
            table1_double.search(table1_double.currentWords[rand()%(table1_double.currentWords.size())]);
            table1_custom.search(table1_custom.currentWords[rand()%(table1_custom.currentWords.size())]);

            table2_chain.search(table2_chain.currentWords[rand()%(table2_chain.currentWords.size())]);
            table2_double.search(table2_double.currentWords[rand()%table2_double.currentWords.size()]);
            table2_custom.search(table2_custom.currentWords[rand()%(table2_custom.currentWords.size())]);
        }


        cout<<"TABLE SIZE: "<<size<<endl;
        cout << "SEPERATE_CHAIN: \n"
             << "Table 1: # of collision = " << table1_chain.countCollisions <<", Average probes = " << table1_chain.countAverageProbes() <<endl;
        cout << "Table 2: # of collision = " << table2_chain.countCollisions <<", Average probes = " << table2_chain.countAverageProbes()  <<endl;

        cout << "DOUBLE_HASHING: \n"
             << "Table 1: # of collision = " << table1_double.countCollisions <<", Average probes = " << table1_double.countAverageProbes()<<endl;
        cout << "Table 2: # of collision = " << table2_double.countCollisions  <<", Average probes = " << table2_double.countAverageProbes()<<endl;

        cout << "CUSTOM_PROBING: \n"
             << "Table 1: # of collision = " << table1_custom.countCollisions <<", Average probes = " << table1_custom.countAverageProbes()<< endl;
        cout << "Table 2: # of collision = " << table2_custom.countCollisions  <<", Average probes = " << table2_custom.countAverageProbes()<< endl;

        cout<<endl;


        cout<<"\n\n..............DELETING..............\n\n";

        for(int i=0; i<size; i++){
            table1_chain.deleteKey(table1_chain.currentWords[rand()%(table1_chain.currentWords.size())]);
            table2_chain.deleteKey(table1_chain.currentWords[rand()%(table1_chain.currentWords.size())]);
        }
    }

    return 0;
}
