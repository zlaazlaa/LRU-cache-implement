#ifndef test_H
#define test_H


#include"lockfree_hashtable.h"
#include"double_link_list.h"
#include<iostream>
#include<string>
using namespace std;

// template<typename T1, typename T2>
// class Node {
// public:
//     T1 key;
//     T2 value;
//     Node *next;
//     Node *prev;
//     Node(T1 k, T2 v) : key(k), value(v), next(nullptr), prev(nullptr) {}
//     Node() : key(0), value(0), next(nullptr), prev(nullptr) {}
// };

template<typename T1, typename T2, typename T3, typename T4, typename T5> // key_type, value_type, hash_type, list_type, node_type
class LRUCache {
public:
    LRUCache(int capacity, T3 *hash, T4 *list, T2 *error_info) {
        this->hash = hash;
        this->list = list;
        this->error_info = error_info;
    }
    T2 get(T1 key) {
        T5 *target;
        if (hash->Find(key, target)) { // find and move to top
            list->move_to_top(target);
            hash->Delete(key);
            hash->Insert(key, target);
        } else { // not find
            return *this->error_info;
        }
        return target->value;
    }

    void put(T1 key, T2 value) {
        T5 *target;
        if (hash->Find(key, target)) { // find and move to top
            list->move_to_top(target);
        } else { // not find
            T5 *new_node = list->insert_to_top(key, value, hash);
            hash->Insert(key, new_node);
        }
    }

private:
    T5 *head, *tail;
    T3 *hash = nullptr;
    T4 *list = nullptr;
    T2 *error_info = nullptr;
};
#endif