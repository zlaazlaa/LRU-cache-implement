#include"lockfree_hashtable.h"
#include<iostream>
using namespace std;
using key_type = int;
using value_type = int;

template<typename T1, typename T2>
class Node {
public:
    T1 key;
    T2 value;
    Node *next;
    Node *prev;
    Node(T1 k, T2 v) : key(k), value(v), next(nullptr), prev(nullptr) {}
    Node() : key(0), value(0), next(nullptr), prev(nullptr) {}
};

template<typename T1, typename T2>
class LRUCache {
public:
    LRUCache(int capacity, LockFreeHashTable<T1, Node<T1, T2>*> *hash, Node<T1, T2> *head, Node<T1, T2> *tail) {
        this->cap = capacity;
        this->hash = hash;
        this->head = head;
        this->tail = tail;
        head->next = tail;
        tail->prev = head;
    }
    T2 get(T1 key) {
        Node<T1, T2> *target;
        if (hash->Find(key, target)) { // find and move to top
            target->prev->next = target->next;
            target->next->prev = target->prev;
            target->next = head->next;
            head->next->prev = target;
            head->next = target;
            target->prev = head;
        } else { // not find
            return -1;
        }
        return target->value;
    }

    void put(T1 key, T2 value) {
        Node<T1, T2> *target;
        if (hash->Find(key, target)) { // find and move to top
            target->prev->next = target->next;
            target->next->prev = target->prev;
            target->next = head->next;
            head->next->prev = target;
            head->next = target;
            target->prev = head;
            target->value = value;
        } else { // not find
            if (size == cap) { // full
                Node<T1, T2> *tmp = tail->prev;
                tmp->prev->next = tail;
                tail->prev = tmp->prev;
                hash->Delete(tmp->key);
                delete tmp;
                size--;
            }
            Node<T1, T2> *new_node = new Node(key, value);
            new_node->next = head->next;
            head->next->prev = new_node;
            head->next = new_node;
            new_node->prev = head;
            hash->Insert(key, new_node);
            size++;
        }
    }

private:
    Node<T1, T2> *head, *tail;
    LockFreeHashTable<T1, Node<T1, T2>*> *hash = nullptr;
    int cap, size;
};