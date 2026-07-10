#include "md5.cpp"
#include <iostream>
#include <cstring>
#include <thread>

using namespace std;

const std::string asciis = "0123456789";
//" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

const string hugo =MD5("hugo").toStr(); 
string calc(string str){
	return MD5(MD5(str).toStr()+hugo).toStr().substr(8, 16);
}
string trypwd (int k,string hash ,int id){
    int* num = new int[k+1];char* str = new char[k+1] ; str[k] = '\0';
    int n = asciis.size();num[k]=0;num[k-1]=id;str[k-1]=asciis[id];
    for(int i=0;i<k-1;i++){
        num[i] = 0;
        str[i] =asciis[0];}
    while (1){
            if(calc(str) == hash){string ans=str;delete[] num;delete[] str;return ans;}
            num[0]+=1; str[0] = asciis[num[0]];
            for(int i=0;num[i]>=n;i++){
                    num[i]=0;num[i+1]+=1;
                    str[i]=asciis[0];str[i+1]=asciis[num[i+1]];
                    if(num[k-1]>id){delete[] num;delete[] str;return "";}
            }  
    }
}

void wrapper(int k, string hash,int id, string* ptr, int* sta) {
	string result = trypwd(k,hash,id);
	if(result!=""){
		*(ptr+id)=result;
		*(sta+id)=1;
	}else{*(sta+id)=2;
	}
	
}

string thrMan(int k,string hash){
	int n =asciis.size();
	string* results =new string[n];
	int* status = new int[n];
	thread* ths =new thread[n];
	for(int i=0;i<n;i++){
		status[i]=0;
		ths[i]=thread(wrapper,k,hash,i,results,status);
	}
	for(int i=0;i<n;i++){
		ths[i].join();
	}
	for(int i=0;i<n;i++){
		if(status[i]==1) return results[i];
	}
	delete[] results;
	delete[] status;
	delete[] ths;
	return "Nothing!!!";
}

int main(){
    string hash;
    std::cout<<"hash: " ;
    std::cin>>hash;
    std::cout<<"length: " ;
    int length;
    std::cin>>length;
    cout<<hash<<"    "<<length<<endl;
    string ans = calc(hash);
    cout<<ans;
    return 0;
}
