const string asciis=("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890")
# include <cstring>
string calc(string str){return str}
string trypwd (k,hash){
    int* num = new int[k+1];char* str = new char[k+1] ; str[k] = '\0'
    int n = asciis.size();num[k]=0
    for(int i<k; i++){
        num[i] = 0;
        str[i] =asciis[0]}
    while (true){
            if(calc(str) == hash){return str}
            num[0]+=1; str[0] = asciis[num[0]];
            for(int i=0;num[i]>=n;i++){
                    num[i]=0;num[i+1]+=1;
                    str[i]=asciis[0];str[i+1]=asciis[num[i+1]];
                    if(num[k]){return ""}
            }  
    }
}
