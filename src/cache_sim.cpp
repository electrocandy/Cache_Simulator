#include "libraries.h"

using namespace std;

#define Main_mem (1<<20) //1 MB Main Memory

class cache{

private:
long int cache_size;
long int cache_line_size;
long int no_lines;
long int no_blocks;
long int block_offset_bits;
long int index_bits;
long int tag_bits;
long int set_bits;
int hits;
int miss;
int set_associativity;
float hitrate;
float missrate;

public:
cache(long int cache_size,long int cache_line_size,int mapping);
void mapping(int mapping,string address_file,int L2_config);
void cache_info_display(int mapping);
void Stats_display();
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

cache::cache(long int cache_size,long int cache_line_size,int mapping){
    hits=0;
    miss=0;
    hitrate=0.0;
    missrate=0.0;
    set_associativity=4;
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

    string address,bin,index,tag,block_offset,set;
    long int index_dec,block_offset_dec,tag_dec,set_dec;
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
    }else if(mapping==3){ //N-Way Set Associative Cache
       long int no_sets=1<<set_bits;
       long int set_placed;
       int count=0;
       
       vector<vector<long int>> cash(no_sets,vector<long int>(set_associativity,-1)); //L1 Cache
       vector<deque<long int>> LRU(no_sets);

       //L2 Cache
       int no_sets1=no_lines/set_associativity;
       long int no_lines1=L2_config<<10/cache_line_size;
       vector<vector<long int>> cash(no_lines1,vector<long int>(set_associativity,-1));
       vector<deque<long int>> LRU1(no_sets1);

       LRU_pop(LRU); //Remove garbage value from each deque in the LRU vector
       LRU_pop(LRU1);

       //cout<<"Tag|"<<"Set|"<<"Block_Offset"<<endl;
       while(getline(txt,address)){
            count=0;
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
           long int tag_dec1,set_dec1,tag_bits1,set_bits1;
           set_bits1=log2(no_sets1);
           tag_bits1=log2(Main_mem)-set_bits1-block_offset_bits;
           tag=bin.substr(0,tag_bits1);
           set=bin.substr(tag_bits1-1,set_bits1);


            for(int i=0;i<set_associativity;i++){
                if(cash[set_placed][i]==-1){
                  miss++;
                  cash[set_placed][i]=tag_dec;
                  LRU[set_placed].push_back(tag_dec);
                  break;
               }else if(cash[set_placed][i]==tag_dec){
                  hits++;
                  for(auto it=LRU[set_placed].begin();it!=LRU[set_placed].end();it++){
                        if(*it==tag_dec){
                           LRU[set_placed].erase(it);
                           break;
                       }
                  }
               }else{
                    count++;
                    if(count==set_associativity){
                      miss++;
                      replaceLRU(cash[set_placed],tag_dec,LRU[set_placed],set_associativity);
                      break;
                }
                }
           }
       }
    txt.close();
    }
    /*for(int i=0;i<cash.size();i++){
        cout<<cash[i]<<endl;
    }*/
}

void cache::Stats_display(){
    hitrate=(float)hits/(hits+miss);
    missrate=(float)miss/(hits+miss);

    cout<<"Hit Rate: "<<hitrate*100<<"%"<<endl;
    cout<<"Miss Rate: "<<missrate*100<<"%"<<endl;
}

void cache::cache_info_display(int mapping){
    cout<<"Cache Size "<<cache_size<<" Kilo Bytes"<<endl;
    cout<<"Cache Line Size "<<cache_line_size<<" Bytes"<<endl;
    cout<<"Numbe of Blocks in Main Memory "<<no_blocks<<endl;
    cout<<"No of Lines in Cache "<<no_lines<<endl;
    cout<<"Block Offset bits "<<block_offset_bits<<endl;
    if(mapping==1){
        cout<<"Index bits "<<index_bits<<endl;
        cout<<"Tag bits "<<tag_bits<<endl;
    }else if(mapping==2){
        cout<<"Tag bits "<<tag_bits<<endl;
    }else if(mapping==3){
        cout<<"Set bits "<<set_bits<<endl;
        cout<<"Tag bits "<<tag_bits<<endl;
    }
}

int main(int argc, char * argv[]){
    int arr[3],count=0;
    int L2_config;
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
    cache c(arr[0],arr[1],arr[2]); //Cache Size in kB and cache line size in B
    cout<<"******************************************************************"<<endl;
    cout<<"                       CACHE SIMULATOR                            "<<endl;
    cout<<"******************************************************************"<<endl;
    cout<<"Make Changes in the Cache_config.txt file for required config !!"<<endl;
    cout<<"Is information about the cache required? (y/n)"<<endl;
    cin>>info;
    if(info[0]=='y' || info[0]=='Y'){
       c.cache_info_display(arr[2]);
    }
    cout<<"Is Level 2 cache required (4-Way Set Associative Cache) ? (y/n)";
    cin>>info;
    if(info[0]=='y' || info[0]=='Y'){
        cout<<"L2: Cache Size (kB) =?";
        cin>>L2_config;
    }
    c.mapping(arr[2],address_file,L2_config);
    cout<<"*******************************************************************"<<endl;
    cout<<"                       Cache Statistics                            "<<endl;
    cout<<"*******************************************************************"<<endl;
    cout<<"Display Cache Statistics? (y/n)"<<endl;
    cin>>info;
    if(info[0]=='y' || info[0]=='Y'){
       c.Stats_display();
    }
}