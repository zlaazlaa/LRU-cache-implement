#ifndef lrucache_single_H
#define lrucache_single_H
#include<unordered_map>
#include "iostream"
using namespace std;

template<typename key_type, typename value_type>
class LRUCache {
public:
    value_type error_info = "-1";
    explicit LRUCache(int capacity) {
        this->capacity = capacity;
        this->size = 0;
        head = new Node();
        tail = new Node();
        head->head = nullptr;
        head->tail = tail;
        tail->head = head;
        tail->tail = nullptr;
    }

    class Node {
    public:
        key_type key;
        value_type value;
        Node *tail;
        Node *head;
    };

    value_type get(key_type key) {
        if (hash_table.find(key) != hash_table.end()) {
            move_to_top(hash_table[key]);
            return hash_table[key]->value;
        }
        return error_info;
    }

    void put(key_type key, value_type value) {
        if (hash_table.find(key) != hash_table.end()) {
            hash_table[key]->value = value;
            move_to_top(hash_table[key]);
        } else {
            if (size == capacity) {
                hash_table.erase(tail->head->key);
                remove(tail->head);
            }
            Node *new_node = new Node();
            new_node->key = key;
            new_node->value = value;
            insert_to_top(new_node);
            hash_table[key] = new_node;
        }
    }

private:
    unordered_map<key_type, Node *> hash_table;
    Node *head, *tail;
    int capacity;
    int size;

    void insert_to_top(Node *now) {
        now->head = head;
        now->tail = head->tail;
        head->tail->head = now;
        head->tail = now;
        size++;
    }

    void remove(Node *now) {
        if (now->head != nullptr) now->head->tail = now->tail;
        if (now->tail != nullptr) now->tail->head = now->head;
        delete now;
        size--;
    }

    void move_to_top(Node *now) {
        if (now->head != nullptr) now->head->tail = now->tail;
        if (now->tail != nullptr) now->tail->head = now->head;
        now->head = head;
        now->tail = head->tail;
        head->tail->head = now;
        head->tail = now;
    }
};
#endif