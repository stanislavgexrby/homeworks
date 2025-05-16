#include <vector>
#include <mutex>
#include <shared_mutex>
#include <optional>
#include <functional>
#include <string>

struct Value {
    std::string str;
    int item;
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
    ~Chain();

    void put(int key, const Value& value);
    bool remove(int key);
    std::optional<Value> check(int key) const;

    void clear();
};

class HashTable {
private:
    std::vector<Chain> container;
    std::hash<int> hash_fn;

public:
    HashTable(size_t size = 10000) : container(size) {};
    ~HashTable() = default;

    HashTable(HashTable &&other) noexcept = default;
    HashTable & operator=(HashTable &&other) noexcept = default;

    void put(int key, const Value& value) {
        size_t index = hash_fn(key) % container.size();
        container[index].put(key, value);
    }

    bool remove(int key) {
        size_t index = hash_fn(key) % container.size();
        return container[index].remove(key);
    }

    std::optional<Value> check(int key) const {
        size_t index = hash_fn(key) % container.size();
        return container[index].check(key);
    }
};

bool test();