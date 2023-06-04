#include"lockfree_hashtable.h"
#include<iostream>
using namespace std;
using key_type = int;
using value_type = int;
typedef struct Node {
    key_type key;
    value_type value;
    Node *next;
    Node *prev;
    Node(key_type k, value_type v) : key(k), value(v), next(nullptr), prev(nullptr) {}
    Node() : key(0), value(0), next(nullptr), prev(nullptr) {}
} Node;
class LRUCache {
public:
    LRUCache(int capacity, LockFreeHashTable<key_type, Node*> *hash) {
        this->cap = capacity;
        this->hash = hash;
        head = new Node();
        tail = new Node();
        head->next = tail;
        tail->prev = head;
    }
    value_type get(key_type key) {
        Node *target;
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

    void put(key_type key, value_type value) {
        Node *target;
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
                Node *tmp = tail->prev;
                tmp->prev->next = tail;
                tail->prev = tmp->prev;
                hash->Delete(tmp->key);
                delete tmp;
                size--;
            }
            Node *new_node = new Node(key, value);
            new_node->next = head->next;
            head->next->prev = new_node;
            head->next = new_node;
            new_node->prev = head;
            hash->Insert(key, new_node);
            size++;
        }
    }

private:
    Node *head, *tail;
    LockFreeHashTable<key_type, Node*> *hash = nullptr;
    int cap, size;
};


int main() {
    LockFreeHashTable<key_type, Node*> *hash = new LockFreeHashTable<key_type, Node*>();
    LRUCache *lRUCache = new LRUCache(2, hash);
    lRUCache->put(1, 1); // 缓存是 {1=1}
    lRUCache->put(2, 2); // 缓存是 {1=1, 2=2}
    cout << lRUCache->get(1) << endl;    // 返回 1
    lRUCache->put(3, 3); // 该操作会使得关键字 2 作废，缓存是 {1=1, 3=3}
    cout << lRUCache->get(2) << endl;    // 返回 -1 (未找到)
    lRUCache->put(4, 4); // 该操作会使得关键字 1 作废，缓存是 {4=4, 3=3}
    cout << lRUCache->get(1) << endl;    // 返回 -1 (未找到)
    cout << lRUCache->get(3) << endl;    // 返回 3
    cout << lRUCache->get(4) << endl;    // 返回 4
    return 0;
}