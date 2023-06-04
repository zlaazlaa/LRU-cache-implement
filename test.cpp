#include"lockfree_hashtable.h"
#include"test.h"
#include<iostream>
using namespace std;
using key_type = int;
using value_type = int;
typedef Node<key_type, value_type> node;

int main() {
    node *head = new node();
    node *tail = new node();
    LockFreeHashTable<key_type, node*> *hash = new LockFreeHashTable<key_type, node*>();
    LRUCache<key_type, value_type> *lRUCache = new LRUCache<key_type, value_type>(2, hash, head, tail);
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