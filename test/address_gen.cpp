#include "libraries.h"

using namespace std;

int main(int argc,char *argv[]){
    srand(time(NULL));
    int s;
    ofstream file("test4.txt");
    for(int i=0;i<atoi(argv[1]);i++){
        file<<"0x";
        for(int j=0;j<5;j++){
           s=rand()%16;
           file<<hex<<s;
        }
        file<<endl;
    }
    file.close();
}