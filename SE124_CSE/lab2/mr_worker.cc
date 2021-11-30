#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <assert.h>

#include "rpc.h"
#include "mr_protocol.h"

using namespace std;

struct KeyVal {
    KeyVal(const string &key, const string &val) : key(key), val(val) {}
    string key;
    string val;
};

ostream &operator<<(ostream &os, const KeyVal &keyVal) {
    cout << "(" << keyVal.key << ", " << keyVal.val << ")" << endl;
    return os;
}


int strHash(const string &str) {
    unsigned int hashVal = 0;
    for (char ch:str) {
        hashVal = hashVal * 131 + (int) ch;
    }
    return hashVal % REDUCER_COUNT;
}


// √
vector<KeyVal> Map(const string &filename, const string &content)
{
    // Copy your code from mr_sequential.cc here.
    map<string, int> wordCount;
    vector <KeyVal> keyVals;
    string w="";
    for (char ch:content) {
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
            w += ch;
        else if (w.size() > 0) {
            wordCount[w]++;
            w="";
        }
    }
    for(auto it = wordCount.begin();it != wordCount.end(); it++){
        string s=it->first;
        keyVals.emplace_back(it->first,to_string(it->second));
    }
    
    return keyVals;
}

//
// The reduce function is called once for each key generated by the
// map tasks, with a list of all the values created for that key by
// any map task.
//
string Reduce(const string &key, const vector < string > &values)
{
    // Copy your code from mr_sequential.cc here.
    unsigned long long sum=0;
    for(long unsigned int i=0;i<values.size();i++){
        unsigned long long count = atoll(values[i].c_str());
        sum+=count;
    }
    return to_string(sum);
}


typedef vector<KeyVal> (*MAPF)(const string &key, const string &value);

typedef string (*REDUCEF)(const string &key, const vector <string> &values);

class Worker {
public:
    Worker(const string &dst, const string &dir, MAPF mf, REDUCEF rf);

    void doWork();

private:
    void doMap(int index, const string &filename);

    void doReduce(int index, int nfiles);

    void doSubmit(mr_tasktype taskType, int index);

    void askTask(mr_protocol::AskTaskResponse &res);

    mutex mtx;
    int id;
    rpcc *cl;
    std::string basedir;
    MAPF mapf;
    REDUCEF reducef;
};


Worker::Worker(const string &dst, const string &dir, MAPF mf, REDUCEF rf) {
    this->basedir = dir;
    this->mapf = mf;
    this->reducef = rf;

    sockaddr_in dstsock;
    make_sockaddr(dst.c_str(), &dstsock);
    this->cl = new rpcc(dstsock);
    if (this->cl->bind() < 0) {
        printf("mr worker: call bind error\n");
    }
}

void Worker::askTask(mr_protocol::AskTaskResponse &res) {
    cl->call(mr_protocol::asktask, id, res);
}

void Worker::doMap(int index, const string &filename)
{
  // 调用map, 得到改文本的keyVal， 给一个针对不同的字符串(key)进行hash， 然后把他们的keyVal 分配给不同的reducer.对每个reducer写一份任务文件（basedir-mr-mapperId-reducerId)。
    // Lab2: Your code goes here.
    // intermediate files is mr-X-Y, where X is the Map task number,and Y is the reduce task number.
    // The worker's map task code will need a way to store intermediate key/value pairs in files in a way that can be correctly read back during reduce tasks.
    ifstream infile;
    string content;
    infile.open(filename,ios::in);
    ostringstream tmp;
    tmp << infile.rdbuf();
    content = tmp.str();
    infile.close();
    string prefix= basedir + "mr-" + to_string(index)+"-";
    vector <KeyVal> keyVals = Map(filename, content);
    //  hash function to distribute the intermediate key-values to different files intended for different Reduce tasks.
    vector <string> contents(REDUCER_COUNT);
    for (const KeyVal &keyVal:keyVals) {
        int reducerId = strHash(keyVal.key);
        contents[reducerId] += keyVal.key + ' ' + keyVal.val + '\n';
    }
    for (int i = 0; i < REDUCER_COUNT; i++) {
            string content = contents[i];
            if (!content.empty()) {
                string filePath = prefix + to_string(i);
                ofstream outfile(filePath, ios::out);
                outfile << content;
                outfile.close();
            }
        }

}


//√
void Worker::doReduce(int index, int nfiles)
{
  // index 表示worker 的reduce工号。
  // 打开所有自己的任务文件, 对每一个key统计value的总和
    // Lab2: Your code goes here.
    // readback intermediate file.
    map<string, unsigned long long> frequency;
    cout<<" Worker: doReduce ";
    for(int m=0;m<nfiles;m++){
        string filename= basedir+ "mr-" + to_string(m) + "-" + to_string(index);
        ifstream infile(filename,ios::in);
        if(!infile)continue;
        string key=""; string valStr="";
        while(infile>>key>>valStr){
            frequency[key]+=atoll(valStr.c_str());
        }
        infile.close();
    }
    // write to the file out.
    string result;
    for (auto it : frequency) result += it.first + ' ' + to_string(it.second) + '\n';
    ofstream outfile( basedir+"mr-out", ios::out | ios::app);
    outfile << result << endl;
    outfile.close();
    
}



// √
void Worker::doSubmit(mr_tasktype taskType, int index)
{
    bool b;
    
    mr_protocol::status ret = this->cl->call(mr_protocol::submittask, taskType, index, b);
    if (ret != mr_protocol::OK) {
        fprintf(stderr, "submit task failed\n");
        exit(-1);
    }
}

void Worker::doWork()
{
    mr_protocol::AskTaskResponse reply;
    for (;;) {
        
        //
        // Lab2: Your code goes here.
        // Hints: send asktask RPC call to coordinator
        // if mr_tasktype::MAP, then doMap and doSubmit
        // if mr_tasktype::REDUCE, then doReduce and doSubmit
        // if mr_tasktype::NONE, meaning currently no work is needed, then sleep
        //
        cl->call(mr_protocol::asktask, id, reply);
        cout<<"A worker get task, type:"<<reply.tasktype<<endl;
        if(reply.tasktype == MAP){
            doMap(reply.index, reply.filename);
            doSubmit(MAP, reply.index);
        }else if(reply.tasktype== REDUCE){
            doReduce(reply.index, reply.nfiles);
            doSubmit(REDUCE, reply.index);
        }else{
          sleep(1);
        }
    }
}


int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <coordinator_listen_port> <intermediate_file_dir> \n", argv[0]);
        exit(1);
    }
    MAPF mf = Map;
    REDUCEF rf = Reduce;
    Worker w(argv[1], argv[2], mf, rf);
    w.doWork();
    return 0;
}
