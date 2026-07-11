#include "md5.cpp"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <thread>
#include <atomic>
#include <chrono>

using namespace std;

const std::string asciis = "0123456789";
//" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

static std::atomic<long long> g_progress{0};
static std::atomic<bool> g_found{false};

static void hex_decode(const char* src, byte* out, int out_len) {
    auto digit = [](char c) -> byte {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return 0;
    };
    for (int i = 0; i < out_len; i++) {
        out[i] = (digit(src[i*2]) << 4) | digit(src[i*2+1]);
    }
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
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    printf("\r%99s\r", "");
}

string trypwd (int k, const byte* target, int id, const char* hugo_hex){
    int* num = new int[k+1];char* str = new char[k+1] ; str[k] = '\0';
    int n = asciis.size();num[k]=0;num[k-1]=id;str[k-1]=asciis[id];
    for(int i=0;i<k-1;i++){
        num[i] = 0;
        str[i] =asciis[0];}

    MD5 md5_inner, md5_outer;
    static const char hex[] = "0123456789abcdef";
    char hex_buf[64];
    memcpy(hex_buf + 32, hugo_hex, 32);

    long long local = 0;
    while (1){
            md5_inner.reset();
            md5_inner.update((const byte*)str, k);
            const byte* inner = md5_inner.finalize();

            for (int i = 0; i < 16; i++) {
                hex_buf[i*2]   = hex[inner[i] >> 4];
                hex_buf[i*2+1] = hex[inner[i] & 0xF];
            }

            md5_outer.reset();
            md5_outer.update((const byte*)hex_buf, 64);
            const byte* outer = md5_outer.finalize();

            if (memcmp(outer + 4, target, 8) == 0){
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

void wrapper(int k, const byte* target, int id, const char* hugo_hex, string* ptr, int* sta) {
    string result = trypwd(k, target, id, hugo_hex);
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

    byte target[8];
    hex_decode(hash.c_str(), target, 8);

    MD5 md5_hugo("hugo");
    const byte* hugo_raw = md5_hugo.getDigest();
    static const char hex[] = "0123456789abcdef";
    char hugo_hex[32];
    for (int i = 0; i < 16; i++) {
        hugo_hex[i*2]   = hex[hugo_raw[i] >> 4];
        hugo_hex[i*2+1] = hex[hugo_raw[i] & 0xF];
    }

    string* results =new string[n];
    int* status = new int[n];
    thread* ths =new thread[n];
    for(int i=0;i<n;i++){
        status[i]=0;
        ths[i]=thread(wrapper,k,target,i,hugo_hex,results,status);
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

int main(int argc, char* argv[]){
    string hash;
    int length = 0;
    string output_file;
    bool has_hash = false, has_length = false, has_output = false;

    for (int i = 1; i < argc; i++) {
        if ((!strcmp(argv[i], "-h") || !strcmp(argv[i], "--hash")) && i+1 < argc)
            { hash = argv[++i]; has_hash = true; }
        else if ((!strcmp(argv[i], "-l") || !strcmp(argv[i], "--length")) && i+1 < argc)
            { length = atoi(argv[++i]); has_length = true; }
        else if ((!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output")) && i+1 < argc)
            { output_file = argv[++i]; has_output = true; }
    }

    if (!has_hash) {
        std::cout<<"hash: " ;
        std::cin>>hash;
    }
    if (!has_length) {
        std::cout<<"length: " ;
        std::cin>>length;
    }
    cout<<hash<<"    "<<length<<endl;
    string ans = thrMan(length,hash);
    if (has_output) {
        ofstream ofs(output_file);
        ofs << ans;
        ofs.close();
    } else {
        cout<<ans;
    }
    return 0;
}
