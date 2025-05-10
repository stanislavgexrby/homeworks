#include <vector>
#include <mutex>
#include <shared_mutex>
#include <optional>
#include <functional>
#include <string>

struct Value {
    std::string str;
    int item;

    bool operator==(const Value &other) const {
      return str == other.str && item == other.item;
    }
};

class Chain {
private:
    struct Node {
        int key;
        Value value;
        Node* next;
        mutable std::mutex node_mtx;

        Node(int k, const Value& v) : key(k), value(v), next(nullptr) {}
    };

    Node* head = nullptr;
    mutable std::shared_mutex chain_mtx;
public:
    ~Chain() {
        clear();
    }

    void Put(int key, const Value& value) {
        Node* new_node = new Node(key, value);
        std::unique_lock<std::shared_mutex> chain_lock(chain_mtx);

        if (!head) {
            head = new_node;
            return;
        }

        Node* prev = nullptr;
        Node* curr = head;
        std::unique_lock<std::mutex> curr_lock(curr->node_mtx);
        chain_lock.unlock();

        while (true) {
            if (curr->key == key) {
                curr->value = value;
                delete new_node;
                return;
            }

            if (!curr->next) {
                curr->next = new_node;
                return;
            }

            Node* next = curr->next;
            std::unique_lock<std::mutex> next_lock(next->node_mtx);
            if (prev) {
                prev->node_mtx.unlock();
            }

            prev = curr;
            curr = next;
            curr_lock.swap(next_lock);
        }
    }

    bool Remove(int key) {
        std::unique_lock<std::shared_mutex> chain_lock(chain_mtx);
        if (!head) {
            return false;
        }

        Node* prev = nullptr;
        Node* curr = head;
        std::unique_lock<std::mutex> curr_lock(curr->node_mtx);
        if (curr->key == key) {
            head = curr->next;
            curr_lock.unlock();
            delete curr;
            return true;
        }

        chain_lock.unlock();

        while (curr->next) {
            Node* next = curr->next;
            std::unique_lock<std::mutex> next_lock(next->node_mtx);

            if (next->key == key) {
                curr->next = next->next;
                next_lock.unlock();
                delete next;
                return true;
            }

            if (prev) {
                prev->node_mtx.unlock();
            }

            prev = curr;
            curr = next;
            curr_lock.swap(next_lock);
        }

        return false;
    }

    std::optional<Value> Check(int key) const {
        std::shared_lock<std::shared_mutex> chain_lock(chain_mtx);
        Node* curr = head;

        if (!curr) {
            return std::nullopt;
        }

        std::unique_lock<std::mutex> curr_lock(curr->node_mtx);
        chain_lock.unlock();

        while (curr) {
            if (curr->key == key) {
                return curr->value;
            }

            Node* next = curr->next;
            if (!next) {
                break;
            }

            std::unique_lock<std::mutex> next_lock(next->node_mtx);
            curr_lock.unlock();
            curr = next;
            curr_lock.swap(next_lock);
        }

        return std::nullopt;
    }

    void clear() {
        std::unique_lock<std::shared_mutex> chain_lock(chain_mtx);
        Node* curr = head;
        while(head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

class HashTable {
private:
    std::vector<Chain> container;
    std::hash<int> hash_fn;

public:
    HashTable(size_t size = 1000) : container(size) {};
    ~HashTable();

    HashTable(HashTable &&other) noexcept = default;
    HashTable & operator=(HashTable &&other) noexcept = default;

    void Put(int key, const Value& value) {
        size_t index = hash_fn(key) % container.size();
        container[index].Put(key, value);
    }
    bool Remove(int key) {
        size_t index = hash_fn(key) % container.size();
        return container[index].Remove(key);
    }

    std::optional<Value> Check(int key) const {
        size_t index = hash_fn(key) % container.size();
        return container[index].Check(key);
    }
};

bool test();