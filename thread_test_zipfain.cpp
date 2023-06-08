#include "LRUCache.h"
#include "double_link_list.h"
#include "lockfree_hashtable.h"
#include "create_zipfian_data.h"
#include "operate_list.h"
#include <iostream>
#include <random>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdio>
using namespace std;
using key_type = string;
using value_type = string;
value_type error_info = "-1";
bool operator!=(const value_type& lhs, const value_type& rhs) {
    return !(lhs == rhs);
}

typedef operate_list<key_type, value_type> list_type;
typedef list_type::Node node_type;
typedef LockFreeHashTable<key_type, node_type *> hash_type;

const int key_range = 1000; // [0, key_range] // Corpus size
const int job_num = 1000000;
const int client_num = 5;
const float read_rate = 0.8;
const int cache_capacity = 100;
const int key_len = 8;
const int value_len = 16;
std::atomic<int>get_sum(0);
std::atomic<int>hit_sum(0);
struct Task {
    key_type key;
    value_type value;
    bool job_type;
};
Task task[client_num + 1][job_num / client_num + 1];
std::thread threads[client_num];
int numStrings = key_range;  // 总共的字符串数量
double zipfianAlpha = 1.0;  // Zipfian 分布的参数
ZipfianStringGenerator generator_key(numStrings, zipfianAlpha, key_len);
ZipfianStringGenerator generator_value(numStrings, zipfianAlpha, value_len);
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, key_range);
std::uniform_real_distribution<> dis2(0, 1);
void start_task(int client_id, LRUCache<key_type, value_type, hash_type, list_type, node_type> *cache) {
    for (int i = 1;i <= job_num / client_num;i ++) {
        if (dis2(gen) > read_rate) { // get
            if (cache->get(generator_key.getRandomString()) != error_info) {
                hit_sum.fetch_add(1);
            }
            get_sum.fetch_add(1);
        } else {
            cache->put(generator_key.getRandomString(), generator_value.getRandomString());
        }
    }
}

int main() {
    list_type *list = new list_type(cache_capacity);
    hash_type *hash = new hash_type();
    LRUCache<key_type, value_type, hash_type, list_type, node_type> *lRUCache = new LRUCache<key_type, value_type, hash_type, list_type, node_type>(2, hash, list, &error_info);
    printf("任务生成完毕, 线程数:%d, 任务总量:%d, 读操作比重:%f\n", client_num, job_num, read_rate);
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 1;i <= client_num;i ++) {
        threads[i] = std::thread(start_task, i, lRUCache);
        threads[i].join();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << "任务执行完毕" << endl;
    std::cout << "程序运行时间：" << duration.count() << " 微秒" << std::endl;
    std::cout << "总get:" << get_sum.load() << ", 命中次数:" << hit_sum.load() << ", 命中率:" << (float)hit_sum.load()/get_sum.load() << endl;
    return 0;
}