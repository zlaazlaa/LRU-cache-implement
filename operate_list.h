#ifndef operate_list_H
#define operate_list_H
#include "double_link_list.h"
#include "lockfree_hashtable.h"

template <typename key_type, typename value_type>
class operate_list : public double_link_list<key_type, value_type> {
public:
    using double_link_list<key_type, value_type>::Node;
    using double_link_list<key_type, value_type>::double_link_list;
    using typename double_link_list<key_type, value_type>::Node;
    typedef typename double_link_list<key_type, value_type>::Node node_type;
    typedef LockFreeHashTable<key_type, node_type*> hash_type;
    
    operate_list(int capacity) : double_link_list(capacity) {

    }

    void move_to_top(Node *now) {
        remove_node(now);
        insert_node_to_top(now);
    }

    void insert_to_top(key_type key, value_type value, hash_type *hash) {
        if (get_size() >= capacity) {
            delete_node(tail);
            hash->Delete(key);
        }
        insert_kv_to_top(key, value);
    }
};

#endif