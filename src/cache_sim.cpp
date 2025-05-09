#include "libraries.h"

using namespace std;

#define Main_mem (1<<20) //1 MB Main Memory

class cache{

public:
long int cache_size;
long int cache_line_size;
long int no_lines;
long int no_blocks;
long int block_offset_bits;
long int index_bits;
long int tag_bits;
long int set_bits;
int set_associativity;

public:
cache(long int cache_size,long int cache_line_size,int mapping,int set_associativity);
void mapping(int mapping,string address_file,int L2_config);
void cache_info_display(int mapping);
};

string hex2bin(string s){
    string hex,bin="";
    hex=s.substr(2,5);
    while(hex.length()<(log2(Main_mem)/4)){
       hex="0"+hex; //Append the address with 0 when length is less than required bits
    }
    for(int i=0;i<hex.length();i++){
        switch(hex[i]){
            case '0':
            bin+="0000";
            break;
            case '1':
            bin+="0001";
            break;
            case '2':
            bin+="0010";
            break;
            case '3':
            bin+="0011";
            break;
            case '4':
            bin+="0100";
            break;
            case '5':
            bin+="0101";
            break;
            case '6':
            bin+="0110";
            break;
            case '7':
            bin+="0111";
            break;
            case '8':
            bin+="1000";
            break;
            case '9':
            bin+="1001";
            break;
            case 'a':
            bin+="1010";
            break;
            case 'b':
            bin+="1011";
            break;
            case 'c':
            bin+="1100";
            break;
            case 'd':
            bin+="1101";
            break;
            case 'e':
            bin+="1110";
            break;
            case 'f':
            bin+="1111";
            break;

            default:
            cout<<"Hex not valid!!";
            bin=" ";
            break;
        }
    }
    return bin;
}

long int bin2dec(string bin){
    long int dec=0;
    for(int i=0;i<bin.size();i++){
        if(bin[i]=='1'){
        dec+=pow(2,bin.size()-i-1);
        }
    }
    return dec;
}

void replaceLRU(vector<long int> &cash,long int tag_dec,deque<long int> &LRU,long int no_lines){
    long int address;
    address=LRU.front();  //Accessing the Frontmost address from the LRU deque
    for(int i=0;i<no_lines;i++){
       if(cash[i]==address){
        cash[i]=tag_dec;
        break;
       }
    }
    LRU.pop_front();
    LRU.push_back(tag_dec); //Adding the recently accessed address to the back of the deque
}

void LRU_pop(vector<deque<long int>> &LRU){
    for(int i=0;i<LRU.size();i++){
        if(!LRU[i].empty()){
           LRU[i].pop_back();
        }
     }
}

void Stats_display(int hits,int miss){
    float hitrate=0.0,missrate=0.0;
    hitrate=(float)hits/(hits+miss);
    missrate=(float)miss/(hits+miss);
    cout<<"Hit Rate: "<<hitrate*100<<"%"<<endl;
    cout<<"Miss Rate: "<<missrate*100<<"%"<<endl;
}

void cache_hit(vector<long int> &cash,deque<long int> &LRU,int set_associativity,long int set_placed,long int tag_dec
               ,int &hits){
        hits++;
        for(auto it=LRU.begin();it!=LRU.end();it++){
            if(*it==tag_dec){
                LRU.erase(it);
                return;
            }
        }
}

cache::cache(long int cache_size,long int cache_line_size,int mapping,int set_associativity){
    this->set_associativity=set_associativity;
    this->cache_size=cache_size;
    this->cache_line_size=cache_line_size;
    no_blocks=Main_mem/cache_line_size;
    no_lines=(cache_size<<10)/cache_line_size;
    block_offset_bits=(int)log2(cache_line_size);
    if(mapping==1){
       index_bits=(int)log2(no_lines); 
       tag_bits=(int)log2(Main_mem)-index_bits-block_offset_bits;
    }else if(mapping==2){
       tag_bits=log2(Main_mem)-block_offset_bits;
    }else if(mapping==3){
       set_bits=log2(no_lines/set_associativity);
       tag_bits=log2(Main_mem)-set_bits-block_offset_bits;
    }
}

void cache::mapping(int mapping,string address_file,int L2_config){

    string address,info,bin,index,tag,block_offset,set;
    long int index_dec,block_offset_dec,tag_dec,set_dec;
    int hits=0,miss=0;
    ifstream txt(address_file);

    if(mapping==1){
        vector<int> cash(no_lines,-1);
        //cout<<"tag"<<"|index"<<"|block_offset"<<endl;
        while(getline(txt,address)){
           bin=hex2bin(address);  
           tag=bin.substr(0,tag_bits);
           index=bin.substr(tag_bits-1,index_bits);
           block_offset=bin.substr(tag_bits+index_bits-1,block_offset_bits);

           //cout<<"|"<<tag<<"|"<<index<<"|"<<block_offset<<endl;

           index_dec=bin2dec(index);
           block_offset_dec=bin2dec(block_offset);
           tag_dec=bin2dec(tag);
           //cout<<"|"<<tag_dec<<"|"<<index_dec<<"|"<<block_offset_dec<<"|";
           if(cash[index_dec]==-1){
              miss++;
              cash[index_dec]=tag_dec;
              //cout<<" : Miss"<<endl;
           }else if(cash[index_dec]==tag_dec){
              hits++;
              //cout<<" : Hits"<<endl;
           }else{
              miss++;
              cash[index_dec]=tag_dec;
              //cout<<" : Miss"<<endl;
        }    
    }
        cout<<"Display Cache Statistics? (y/n)"<<endl;
        cin>>info;
        if(info[0]=='y' || info[0]=='Y'){
           cout<<"*******************************************************************"<<endl;
           cout<<"                       Cache Statistics                            "<<endl;
           cout<<"*******************************************************************"<<endl;
           Stats_display(hits,miss);
        }
    }else if(mapping==2){
        vector<long int> cash(no_lines,-1);
        deque<long int> LRU;

        int count=0;
        //cout<<"Tag |"<<"Block Offset"<<endl;
        while(getline(txt,address)){
            count=0;
            bin=hex2bin(address);
            tag=bin.substr(0,tag_bits);
            block_offset=bin.substr(tag_bits-1,block_offset_bits);
            //cout<<tag<<"|"<<block_offset<<endl;

            tag_dec=bin2dec(tag);
            block_offset_dec=bin2dec(block_offset);
            //cout<<tag_dec<<"|"<<block_offset_dec<<endl;
            for(int i=0;i<no_lines;i++){
                if(cash[i]==-1){
                    cash[i]=tag_dec;
                    LRU.push_back(tag_dec);
                    miss++;
                    break;
                }else if(cash[i]==tag_dec){
                    hits++;
                    auto it=find(LRU.begin(),LRU.end(),tag_dec); //Even when there is a hits, the address is erased from its original
                    if(it!=LRU.end()){                           //position in deque and added at the back to indicate its recency
                        LRU.erase(it);
                    }
                    LRU.push_back(tag_dec);
                    break;
                }else{
                    count+=1;
                    if(count==no_lines){
                        miss++;
                        replaceLRU(cash,tag_dec,LRU,no_lines);
                        break;
                    }
                }
            }
        }
        cout<<"Display Cache Statistics? (y/n)"<<endl;
        cin>>info;
        if(info[0]=='y' || info[0]=='Y'){
           cout<<"*******************************************************************"<<endl;
           cout<<"                       Cache Statistics                            "<<endl;
           cout<<"*******************************************************************"<<endl;
           Stats_display(hits,miss);
        }
    }else if(mapping==3){ //N-Way Set Associative Cache
       long int no_sets=1<<set_bits;
       long int set_placed,set_placed1;
       int hits1=0,miss1=0;
       int count=0,count1=0,count2=0;
       
       vector<vector<long int>> cash(no_sets,vector<long int>(set_associativity,-1)); //L1 Cache
       vector<deque<long int>> LRU(no_sets);

       //L2 Cache
       long int tag_dec1,set_dec1,tag_bits1,set_bits1;
       long int no_lines1=(L2_config<<10)/cache_line_size;
       int no_sets1=no_lines1/set_associativity;
       set_bits1=log2(no_sets1);
       tag_bits1=log2(Main_mem)-set_bits1-block_offset_bits;
       cout<<endl;
       cout<<"***************** L2 Stats **********************"<<endl;
       cout<<"No of Line in L2 cache = "<<no_lines1<<endl;
       cout<<"Block Offset bits in L2 cache = "<<block_offset_bits<<endl;
       cout<<"Set bits in L2 cache = "<<set_bits1<<endl;
       cout<<"Tag bits in L2 cache = "<<tag_bits1<<endl;
       vector<vector<long int>> cash1(no_sets1,vector<long int>(set_associativity,-1));
       vector<deque<long int>> LRU1(no_sets1);

       LRU_pop(LRU); //Remove garbage value from each deque in the LRU vector
       LRU_pop(LRU1);

       //cout<<"Tag|"<<"Set|"<<"Block_Offset"<<endl;
       while(getline(txt,address)){
            count=0;
            count1=0;
            count2=0;
            bin=hex2bin(address);
            tag=bin.substr(0,tag_bits);
            set=bin.substr(tag_bits-1,set_bits);
            block_offset=bin.substr(tag_bits+set_bits-1,block_offset_bits);
            tag_dec=bin2dec(tag);
            set_dec=bin2dec(set);
            block_offset_dec=bin2dec(block_offset);
            //cout<<tag_dec<<"|"<<set_dec<<"|"<<block_offset_dec<<"|";
            set_placed=set_dec%no_sets;
            //cout<<set_placed<<endl;

           //L2 Cache
           tag=bin.substr(0,tag_bits1);
           set=bin.substr(tag_bits1-1,set_bits1);
          
           tag_dec1=bin2dec(tag);
           set_dec1=bin2dec(set);

           set_placed1=set_dec1%no_sets1;

            for(int i=0;i<set_associativity;i++){
                if(cash[set_placed][i]==-1){       //L1 compulsory miss
                  miss++;
                  cash[set_placed][i]=tag_dec;
                  LRU[set_placed].push_back(tag_dec);
                  
                  bool l2_up=false;
                  for(int j=0;j<set_associativity;j++){       //L2 compulsory miss
                     if(cash1[set_placed1][j]==-1){
                        l2_up=true;
                        cash1[set_placed1][j]=tag_dec1;
                        LRU1[set_placed1].push_back(tag_dec1);
                        break;
                    }
                  }
                  if(!l2_up){ //If L2 is filled, then LRU is used
                    miss1++;
                    replaceLRU(cash1[set_placed1],tag_dec1,LRU1[set_placed1],set_associativity);
                  }
                  break;
               }else if(cash[set_placed][i]==tag_dec){ //L1 hit
                  hits++;
                  for(auto it=LRU[set_placed].begin();it!=LRU[set_placed].end();it++){
                        if(*it==tag_dec){
                           LRU[set_placed].erase(it);
                           break;
                       }
                  }
                  LRU[set_placed].push_back(tag_dec);
                  break;
               }
               else if(i+1==set_associativity){ //If L1 miss then check L2
                    bool L2_hit=false;
                    for(int j=0;j<set_associativity;j++){
                        if(cash1[set_placed1][j]==tag_dec1){
                            hits1++;
                            cash1[set_placed1][j]=tag_dec1;
                            for(auto it=LRU1[set_placed1].begin();it!=LRU1[set_placed1].end();it++){
                                if(*it==tag_dec1){
                                LRU1[set_placed1].erase(it);
                                break;
                                }
                            }
                            LRU1[set_placed1].push_back(tag_dec1);
                            L2_hit=true;
                            replaceLRU(cash[set_placed],tag_dec,LRU[set_placed],set_associativity); //Move the data tag to L1
                            break;
                        }
                    }
                    if(!L2_hit){ //L2 miss
                        miss1++;
                        replaceLRU(cash1[set_placed1],tag_dec1,LRU1[set_placed1],set_associativity);
                    }
                }
        }
    /*for(int i=0;i<cash.size();i++){
        cout<<cash[i]<<endl;
    }*/
    }
    cout<<"Display Cache Statistics? (y/n)"<<endl;
    cin>>info;
    if(info[0]=='y' || info[0]=='Y'){
        cout<<"*******************************************************************"<<endl;
        cout<<"                       Cache Statistics                            "<<endl;
        cout<<"*******************************************************************"<<endl<<"\n";
        cout<<"L1 Cache :"<<endl;
        Stats_display(hits,miss);
        cout<<"L2 Cache :"<<endl;
        Stats_display(hits1,miss1);
    }
    txt.close();
    }
}

void cache::cache_info_display(int mapping){
    cout<<endl;
    cout<<"****************** L1 Stats **********************"<<endl;
    cout<<"L1 Cache Size = "<<cache_size<<" Kilo Bytes"<<endl;
    cout<<"Cache Line Size = "<<cache_line_size<<" Bytes"<<endl;
    cout<<"Numbe of Blocks in Main Memory = "<<no_blocks<<endl;
    cout<<"No of Lines in L1 Cache = "<<no_lines<<endl;
    cout<<"Block Offset bits in L1 cache = "<<block_offset_bits<<endl;
    if(mapping==1){
        cout<<"Index bits in L1 cache = "<<index_bits<<endl;
        cout<<"Tag bits in L1 cache = "<<tag_bits<<endl;
    }else if(mapping==2){
        cout<<"Tag bits in L1 cache = "<<tag_bits<<endl;
    }else if(mapping==3){
        cout<<"Set bits in L1 cache = "<<set_bits<<endl;
        cout<<"Tag bits in L1 cache = "<<tag_bits<<endl;
    }
}

int main(int argc, char * argv[]){
    int arr[5],count=0;
    string str,address_file,info;
    ifstream cache_config(argv[1]);
    while(getline(cache_config,str)){
        if(str[0]=='#'){
            continue;
        }else{
            arr[count]=stoi(str);
            ++count;
        }
    }
    address_file=argv[2];
    cache c(arr[0],arr[2],arr[3],arr[4]);  //Cache Size in kB and cache line size in B
    cout<<"******************************************************************"<<endl;
    cout<<"                       CACHE SIMULATOR                            "<<endl;
    cout<<"******************************************************************"<<endl;
    cout<<"Make Changes in the Cache_config.txt file for required config !!"<<endl;
    cout<<"Is information about the cache required? (y/n)"<<endl;
    cin>>info;
    if(info[0]=='y' || info[0]=='Y'){
       c.cache_info_display(arr[3]);
    }
    c.mapping(arr[3],address_file,arr[1]);
}