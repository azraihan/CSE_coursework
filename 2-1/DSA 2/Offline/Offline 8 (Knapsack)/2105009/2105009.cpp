#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
using namespace std;
const int INF = 1e9;

int main()
{
    freopen("in.txt","r",stdin);
    
    int n, W;  //n=number of available items, W=weight of knapsack
    cin >> n >> W;

    vector<int> value(n+1), weight(n+1);  //value=array of values corresponding to items, weight=array of weights corresponding to items
                                          //take the capacity as n+1 to make the array 1-indexed

    vector<double> e(4) ;  //e=epsilon (precision parameter)
    e[0]=0.5, e[1]=0.2, e[2]=0.1, e[3]=0.05;
    
    
    
    cin>>value[1]>>weight[1];
    int maxValue = value[1];
    
    for(int i = 2; i <= n; i++)      //take input of the (value,weight) pair for the items and the calculate v_max(that is maxValue)
    {
        cin>>weight[i]>>value[i];
        if(maxValue<value[i]) maxValue=value[i];
    }


    vector<vector<int> > dpTable(n+1, vector<int>(n*maxValue+1));  //the dpTable is 1-indexed hence this is (n+1)X(n*maxValue+1)
    
    
    
    //////running algorithmII
    for(int i = 0; i <= n; i++)
    {
        dpTable[i][0] = 0;
    }
    for(int i = 1; i <= n*maxValue; i++)
    {
        dpTable[0][i] = INF;
    }
    for(int i = 1; i <= n; i++)
    {
        for(int j = 1; j <= n*maxValue; j++)
        {
            if(j < value[i])
            {
                dpTable[i][j] = dpTable[i-1][j];
            }
            else
            {
                dpTable[i][j] = min(dpTable[i-1][j], dpTable[i-1][j-value[i]] + weight[i]);
            }
        }
    }


 
 
    vector<int> originalInstanceResultIndices; //index of items taken
    int originalRealWeight; //total weight filled in knapsack
    int originalMaxValue;   //total value


    
    //calculating total value i.e. originalMaxValue and total weight filled in knapsack i.e. originalRealWeight
    for(int i = n*maxValue; i >= 0; i--)
    {
        if(dpTable[n][i] <= W)
        {
            originalRealWeight = dpTable[n][i];
            originalMaxValue = i;
            break;
        }
    }


    int originalIndex = originalMaxValue;
    for(int i = n; i > 0; i--)   //backtracking along the dpTable to get the indices of the items taken
    {
        if(dpTable[i][originalIndex] != dpTable[i-1][originalIndex])
        {
            originalInstanceResultIndices.push_back(i);
            originalIndex -= value[i];
        }
    }


    //////Outputting for original instance
    cout << "Original Instance:\n";
    cout << "Answer: " << originalMaxValue << endl;
    cout << "Used Weight: " << originalRealWeight << endl;
    cout << "Indices: ";
    for(int i = 0; i < originalInstanceResultIndices.size(); i++)
    {
        cout << originalInstanceResultIndices[i] << " ";
    }
    cout << "\n\n";
    
    
    
    ///////Calculations for precision parameters e(epsilon)=0.5,0.2,0.1,0.05
    for(int k = 0; k < e.size(); k++)
    {
        double theta = (e[k] * maxValue) / (2 * n);  //scaling factor


        int new_v_max = ceil(maxValue / theta) + 1;  //new value of v_max

        vector<int> newValues(n+1);  //array of new scaled values. This is 1-indexed
        
        
        //calculating the newly scaled values
        for(int i = 1; i <= n; i++)
        {
            newValues[i] = ceil(value[i] / theta);
            if(value[i] == maxValue)
            {
                ++newValues[i];  ///////////***********
            }
        }

        
        //////perform alogorithmII on the new dpTable consisting of scaled values
        vector<vector<int> > dpTable2(n+1, vector<int>(n*new_v_max+1));  //(n+1)* (n*new_v_max+1) table. And is 1 indexed.




        ////////runnning algorithmII
        for(int i = 0; i <= n; i++)
        {
            dpTable2[i][0] = 0;
        }
        for(int i = 1; i <= n*new_v_max; i++)
        {
            dpTable2[0][i] = INF;
        }
        for(int i = 1; i <= n; i++)
        {
            for(int j = 1; j <= n*new_v_max; j++)
            {
                if(j < newValues[i])
                {
                    dpTable2[i][j] = dpTable2[i-1][j];
                }
                else
                {
                    dpTable2[i][j] = min(dpTable2[i-1][j], dpTable2[i-1][j-newValues[i]] + weight[i]);
                }
            }
        }



        vector<int> newResultIndices;   ///array of the indices of the picked items
        int newTotalWeight;   ///the total weight of the items taken in the reduced instance
        int newMaxValue;      ///the total values of the taken items in the reduced instance
        
        
        
        //calculating total weight and total values of the items taken in the reduced instance
        //cout<<new_v_max<<endl;
        for(int i = n*new_v_max; i >= 0; i--)
        {
            //cout<<"Index: "<<i<<", "<<dpTable2[n][i]<<endl;
            if(dpTable2[n][i] <= W)
            {
                //cout<<dpTable2[n][i]<<" "<<W<<endl;
                newTotalWeight = dpTable2[n][i];
                //cout<<newTotalWeight<<endl;
                newMaxValue = i;
                break;
            }
        }
        //cout<<newTotalWeight<<endl;


        //calculating the indices of the items taken in the reduced instance
        int newIndex = newMaxValue;
        for(int i = n; i > 0; i--)
        {
            if(dpTable2[i][newIndex] != dpTable2[i-1][newIndex])
            {
                newResultIndices.push_back(i);
                newIndex -= newValues[i];
            }
        }

        //calculating rounded up value for the original instance
        int roundedUpValueForOriginalInstance = 0;
        for(int i = 0; i < newResultIndices.size(); i++)
        {
            roundedUpValueForOriginalInstance += value[newResultIndices[i]];
        }
        
        /////outputing results for reduced instance and rounded values for original instance
        
        cout << fixed << setprecision(16);
        cout << "Rounded Instance with Eps: " << e[k] << endl;
        cout << "Theta: " << theta << '\n';
        cout << "Answer of reduced instance: " << newMaxValue << endl;
        cout << "Answer of reduced instance multiplied by theta: " << newMaxValue * theta << endl;
        cout << "Indices: ";
        for(int i = 0; i < newResultIndices.size(); i++)
        {
            cout << newResultIndices[i] << " ";
        }
        cout << endl;
        

        cout << "Answer of original instance(rounded up): " << roundedUpValueForOriginalInstance << endl;
        cout << "Used Weight: " << newTotalWeight << endl;
        cout << "Ratio: " << (1.0 * originalMaxValue) / roundedUpValueForOriginalInstance << "\n\n";
    }

    return 0;
}