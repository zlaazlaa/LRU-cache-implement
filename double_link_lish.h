#include <mutex>
#include <shared_mutex>
#include <atomic>
template <typename key_type, typename value_type>
class double_link_list
{
public:
    void move_to_top(Node *now)
    {
        key_type key = now->key;
        value_type value = now->value;
        delete_node(now);
        insert_to_top(key, value);
    }

    void delete_node(Node *now)
    {
        while (true)
        {
            std::shared_lock<std::mutex> lock2(*now->mut, std::try_to_lock);
            while (!lock2.owns_lock())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                std::shared_lock<std::mutex> lock2(*now->mut, std::try_to_lock);
            }
            std::unique_lock<std::mutex> lock1(*now->head->mux, std::try_to_lock);
            // deadlock prevention, if faild, unlock the mutex above
            if (!lock1.owns_lock())
            {
                lock2.unlock();
                continue;
            }
            std::unique_lock<std::mutex> lock3(*now->tail->mut, std::try_to_lock);
            while (!lock3.owns_lock())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                std::unique_lock<std::mutex> lock3(*now->tail->mut, std::try_to_lock);
            }
            lock2.unlock();
            std::unique_lock<std::mutex> lock2(*now->mut, std::try_to_lock);
            while (!lock2.owns_lock())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                std::unique_lock<std::mutex> lock2(*now->mut, std::try_to_lock);
            }
            // delete and rebuild
            now->head->tail = now->tail;
            now->tail->head = now->head;
            size.fetch_sub(1, std::memory_order_relaxed);
            delete now;
            break;
        }
    }

    void insert_to_top(key_type key, value_type value)
    {
        if (get_size() >= capacity)
        {
            delete_node(tail);
        }
        Node *now = new Node(key, value);
        now->head = head;
        std::unique_lock<std::mutex> lockhead1(*head->mut, std::try_to_lock);
        while (!lockhead1.owns_lock())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::unique_lock<std::mutex> lockhead1(*head->mut, std::try_to_lock);
        }
        std::unique_lock<std::mutex> lockhead2(*head->tail->mut, std::try_to_lock);
        while (!lockhead2.owns_lock())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::unique_lock<std::mutex> lockhead2(*head->tail->mut, std::try_to_lock);
        }
        now->tail = head->tail;
        head->tail->head = now;
        head->tail = now;
        size.fetch_add(1, std::memory_order_relaxed);
    }

    int get_size() const
    {
        return size.load(std::memory_order_relaxed);
    }

    class Node
    {
        key_type key;
        value_type value;
        Node *tail;
        Node *prev;
        std::mutex mutex_;
    } private : Node *head;
    Node *tail;
    std::mutex *mut;
    std::atomic<int> size; // use atomic to avoid mutex cost
    int capacity;
};