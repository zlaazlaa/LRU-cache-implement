#ifndef double_link_list_H
#define double_link_list_H

#include <atomic>
#include <mutex>
#include <shared_mutex>

template <typename key_type, typename value_type>
class double_link_list {
public:
    class Node {
    public:
        key_type key;
        value_type value;
        Node *tail;
        Node *head;
        std::shared_mutex mut;
        Node(key_type k, value_type v) : key(k), value(v), tail(nullptr), head(nullptr) {}
        Node() : tail(nullptr), head(nullptr) {}
    };

    double_link_list(int capacity) {
        this->head = new Node();
        this->tail = new Node();
        this->head->tail = tail;
        this->tail->head = head;
        this->capacity = capacity;
        this->size.store(0);
    }

    void move_to_top(Node *now) { // remove(without deleting) and insert(without creating node)
        key_type key = now->key;
        value_type value = now->value;
        remove_node(now);
        insert_node_to_top(now);
    }

    void delete_node(Node *now) { // remove with deleting
        remove_node(now);
        delete now;
    }

    void remove_node(Node *now) { // remove without deleting
        while (true) {
            std::shared_lock lock2(now->mut, std::try_to_lock);
            while (!lock2.owns_lock()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                std::shared_lock lock2(now->mut, std::try_to_lock);
            }
            std::unique_lock lock1(now->head->mut, std::try_to_lock);
            // deadlock prevention, if faild, unlock the mutex above
            if (!lock1.owns_lock()) {
                lock2.unlock();
                continue;
            }
            std::unique_lock lock3(now->tail->mut, std::try_to_lock);
            while (!lock3.owns_lock()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                std::unique_lock lock3(now->tail->mut, std::try_to_lock);
            }
            lock2.unlock();
            std::unique_lock lock2_unique(now->mut, std::try_to_lock);
            while (!lock2_unique.owns_lock()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                std::unique_lock lock2_unique(now->mut, std::try_to_lock);
            }
            // delete and rebuild
            now->head->tail = now->tail;
            now->tail->head = now->head;
            size.fetch_sub(1, std::memory_order_relaxed);
            break;
        }
    }

    Node *insert_kv_to_top(key_type key, value_type value) { // insert (key, value), with creating node
        Node *now = new Node(key, value);
        insert_node_to_top(now);
        return now;
    }

    void insert_node_to_top(Node *now) { // insert node, without creating node
        std::unique_lock lockhead1(head->mut, std::try_to_lock);
        while (!lockhead1.owns_lock()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::unique_lock lockhead1(head->mut, std::try_to_lock);
        }
        std::unique_lock lockhead2(head->tail->mut, std::try_to_lock);
        while (!lockhead2.owns_lock()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::unique_lock lockhead2(head->tail->mut, std::try_to_lock);
        }
        now->head = head;
        now->tail = head->tail;
        head->tail->head = now;
        head->tail = now;
        size.fetch_add(1, std::memory_order_relaxed);
    }

    int get_size() const { return size.load(std::memory_order_relaxed); }
    int capacity;
    std::atomic<int> size; // use atomic to avoid mutex cost
    Node *head;
    Node *tail;
private :     
    
};
#endif