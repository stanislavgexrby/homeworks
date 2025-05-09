#include "hashtable.hpp"

HashTable::HashTable(size_t size) : _size(size) {
    chain.resize(size, nullptr);
}

HashTable::~HashTable() {
    for (auto& head : chain) {
        Node* curr = head;
        while (curr) {
            Node* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }
}

void HashTable::Put(int key, const Value& value) {
    size_t index = hash_fn(key) % _size;

    Node* new_node = new Node(key, value);
    Node* prev = nullptr;
    Node* curr;

    while (true) {
        curr = chain[index];
        if (!curr) {
            std::unique_lock<std::shared_mutex> lock;
            if (prev) {
                lock = std::unique_lock<std::shared_mutex>(prev->mut);
            }
            if (chain[index] == curr) {
                chain[index] = new_node;
                return;
            }
            continue;
        }

        std::unique_lock<std::shared_mutex> curr_lock(curr->mut);
        if (prev) {
            prev->mut.lock();
        }

        if (curr->key == key) {
            curr->value = value;
            delete new_node;
            if (prev) {
                prev->mut.unlock();
            }
            return;
        }

        if (!curr->next) {
            curr->next = new_node;
            if (prev) {
                prev->mut.unlock();
            }
            return;
        }

        Node* next = curr->next;
        std::unique_lock<std::shared_mutex> next_lock(next->mut);

        if (prev) {
            prev->mut.unlock();
        }

        prev = curr;
        curr = next;
        curr_lock.swap(next_lock);
    }
}

bool HashTable::Remove(int key) {
    size_t index = hash_fn(key) % _size;
    Node* prev = nullptr;
    Node* curr;

    while (true) {
        curr = chain[index];
        if (!curr) {
            return false;
        }

        std::unique_lock<std::shared_mutex> curr_lock(curr->mut);
        if (prev) {
            prev->mut.lock();
        }

        if (curr->key == key) {
            if (prev) {
                prev->next = curr->next;
                prev->mut.unlock();
            } else {
                chain[index] = curr->next;
            }
            curr_lock.unlock();
            delete curr;
            return true;
        }

        if (!curr->next) {
            if (prev) {
                prev->mut.unlock();
            }
            return false;
        }

        Node* next = curr->next;
        std::unique_lock<std::shared_mutex> next_lock(next->mut);
        if (curr->next != next) {
            curr_lock.unlock();
            if (prev) {
                prev->mut.unlock();
            }
            continue;
        }

        if (prev) {
            prev->mut.unlock();
        }

        prev = curr;
        curr = next;
        curr_lock.swap(next_lock);
    }
}

std::optional<Value> HashTable::Check(int key) const {
    size_t index = hash_fn(key) % _size;

    Node* curr;
    std::shared_lock<std::shared_mutex> lock;

    while (true) {
        curr = chain[index];
        if (!curr) {
            return std::nullopt;
        }

        lock = std::shared_lock<std::shared_mutex>(curr->mut);
        if (curr != chain[index]) {
            lock.unlock();
            continue;
        }

        while (curr) {
            if (curr->key == key) {
                return curr->value;
            }
            Node* next = curr->next;
            if (!next) {
                break;
            }

            std::shared_lock<std::shared_mutex> next_lock(next->mut);
            lock.unlock();
            curr = next;
            lock.swap(next_lock);
        }

        return std::nullopt;
    }
}