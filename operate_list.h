#ifndef operate_list_H
#define operate_list_H
#include "double_link_list.h"
#include "lockfree_hashtable.h"

template <typename key_type, typename value_type>
class operate_list : public double_link_list<key_type, value_type> {
public:
    using Node = typename double_link_list<key_type, value_type>::Node;

    operate_list(int capacity) : double_link_list<key_type, value_type>(capacity) {}

    void move_to_top(Node *now) {
        this->remove_node(now);
        this->insert_node_to_top(now);
    }

    Node* insert_to_top(key_type key, value_type value, LockFreeHashTable<key_type, Node*> *hash) {
        if (this->get_size() >= this->capacity) {
            hash->Delete(this->tail->head->key);
            this->delete_node(this->tail->head);
        }
        return this->insert_kv_to_top(key, value);
    }
};
#endif