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

class HashTable {
private:
    struct Node {
        int key;
        Value value;
        Node* next;
        mutable std::shared_mutex mut;

        Node(int k, const Value& v) : key(k), value(v), next(nullptr) {}
    };

    std::vector<Node*> chain;
    size_t _size;

    std::hash<int> hash_fn;

    Node* get_chain(int key) const {
        return chain[hash_fn(key) % _size];
    }

public:
    HashTable(size_t size = 5);
    ~HashTable();

    void Put(int key, const Value& value);
    bool Remove(int key);
    std::optional<Value> Check(int key) const;
};

bool test();