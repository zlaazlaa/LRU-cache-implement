#include "LRUCache.h"
#include "double_link_list.h"
#include "lockfree_hashtable.h"
#include "operate_list.h"
#include <iostream>
#include <random>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdio>
using namespace std;
using key_type = int;
using value_type = int;

typedef operate_list<key_type, value_type> list_type;
typedef list_type::Node node_type;
typedef LockFreeHashTable<key_type, node_type *> hash_type;

const int key_range = 100; // [0, key_range]
const int job_num = 1000;
const int client_num = 10;
const float read_rate = 0.8;
const int cache_capacity = 100;
struct Task {
    key_type key;
    value_type value;
    bool job_type;
};
Task task[client_num + 1][job_num / client_num + 1];
std::thread threads[client_num];
void start_task(int client_id, LRUCache<key_type, value_type, hash_type, list_type, node_type> *cache) {
    for (int i = 1;i <= job_num / client_num;i ++) {
        Task now = task[client_id][i];
        if (now.job_type) { // get
            cache->get(now.key);
        } else {
            cache->put(now.key, now.value);
        }
    }
}

int main() {
    list_type *list = new list_type(cache_capacity);
    hash_type *hash = new hash_type();
    LRUCache<key_type, value_type, hash_type, list_type, node_type> *lRUCache = new LRUCache<key_type, value_type, hash_type, list_type, node_type>(2, hash, list);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, key_range);
    std::uniform_real_distribution<> dis2(0, 1);
    for (int i = 1; i <= client_num; i++) {
        for (int j = 1; j <= job_num / client_num; j++) {
            task[i][j] = {dis(gen), dis(gen), (dis2(gen) > read_rate) ? false : true}; // false -> put, true -> get;
        }
    }
    printf("任务生成完毕, 线程数:%d, 任务总量:%d, 读操作比重:%f", client_num, job_num, read_rate);
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 1;i <= client_num;i ++) {
        threads[i] = std::thread(start_task, i, lRUCache);
        threads[i].join();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << "任务执行完毕" << endl;
    std::cout << "程序运行时间：" << duration.count() << " 微秒" << std::endl;
    return 0;
}