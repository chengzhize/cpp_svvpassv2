#include "md5.cpp"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <thread>
#include <atomic>
#include <chrono>

using namespace std;

const std::string asciis = "0123456789";
//" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

const string hugo =MD5("hugo").toStr();

static std::atomic<long long> g_progress{0};
static std::atomic<bool> g_found{false};

string calc(string str){
	return MD5(MD5(str).toStr()+hugo).toStr().substr(8, 16);
}

void show_progress(long long total) {
    auto start = std::chrono::steady_clock::now();
    while (!g_found.load(std::memory_order_relaxed) &&
           g_progress.load(std::memory_order_relaxed) < total) {
        long long cur = g_progress.load(std::memory_order_relaxed);
        int pct = (int)(cur * 100LL / total);
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - start).count();
        char eta[32] = "--s";
        if (elapsed > 0.5 && cur > 0) {
            double rate = cur / elapsed;
            double remain_sec = (total - cur) / rate;
            if (remain_sec >= 3600)
                snprintf(eta, sizeof(eta), "%dh%dm", (int)remain_sec/3600, ((int)remain_sec%3600)/60);
            else if (remain_sec >= 60)
                snprintf(eta, sizeof(eta), "%dm%ds", (int)remain_sec/60, (int)remain_sec%60);
            else
                snprintf(eta, sizeof(eta), "%ds", (int)remain_sec);
        }
        char bar[22];
        int fill = pct / 5;
        memset(bar, '#', fill);
        memset(bar + fill, ' ', 20 - fill);
        bar[20] = '\0';
        printf("\rProgress: [%s] %3d%%  %s  %lld/%lld", bar, pct, eta, cur, total);
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::seconds(15));
    }
    printf("\r%99s\r", "");
}

string trypwd (int k,string hash ,int id){
    int* num = new int[k+1];char* str = new char[k+1] ; str[k] = '\0';
    int n = asciis.size();num[k]=0;num[k-1]=id;str[k-1]=asciis[id];
    for(int i=0;i<k-1;i++){
        num[i] = 0;
        str[i] =asciis[0];}
    long long local = 0;
    while (1){
            if(calc(str) == hash){
                g_progress.fetch_add((local & 0xFF) + 1, std::memory_order_relaxed);
                string ans=str;delete[] num;delete[] str;return ans;}
            local++;
            if ((local & 0xFF) == 0){
                g_progress.fetch_add(256, std::memory_order_relaxed);
                if ((local & 0xFFF) == 0 && g_found.load(std::memory_order_relaxed)){
                    g_progress.fetch_add(local & 0xFF, std::memory_order_relaxed);
                    delete[] num;delete[] str;return "";}
            }
            num[0]+=1; str[0] = asciis[num[0]];
            for(int i=0;num[i]>=n;i++){
                    num[i]=0;num[i+1]+=1;
                    str[i]=asciis[0];str[i+1]=asciis[num[i+1]];
                    if(num[k-1]>id){
                        g_progress.fetch_add(local & 0xFF, std::memory_order_relaxed);
                        delete[] num;delete[] str;return "";}
            }
    }
}

void wrapper(int k, string hash,int id, string* ptr, int* sta) {
    string result = trypwd(k,hash,id);
    if(result!=""){
        *(ptr+id)=result;
        *(sta+id)=1;
        g_found.store(true, std::memory_order_relaxed);
    }else{
        *(sta+id)=2;
    }
}

string thrMan(int k,string hash){
    int n =asciis.size();
    long long total = 1;
    for (int i = 0; i < k; i++) total *= n;
    g_progress.store(0, std::memory_order_relaxed);
    g_found.store(false, std::memory_order_relaxed);
    string* results =new string[n];
    int* status = new int[n];
    thread* ths =new thread[n];
    for(int i=0;i<n;i++){
        status[i]=0;
        ths[i]=thread(wrapper,k,hash,i,results,status);
    }
    thread monitor(show_progress, total);
    for(int i=0;i<n;i++){
        ths[i].join();
    }
    monitor.join();
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
    string ans = thrMan(length,hash);
    cout<<ans;
    return 0;
}
