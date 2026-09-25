#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace airline {

template <typename T>
class Repository {
protected:
    std::unordered_map<std::string, std::shared_ptr<T>> items_;

public:
    virtual ~Repository() = default;

    void add(const std::string& key, std::shared_ptr<T> item) {
        items_[key] = std::move(item);
    }

    std::shared_ptr<T> get(const std::string& key) const {
        auto it = items_.find(key);
        return it != items_.end() ? it->second : nullptr;
    }

    bool contains(const std::string& key) const {
        return items_.find(key) != items_.end();
    }

    bool remove(const std::string& key) {
        return items_.erase(key) > 0;
    }

    std::size_t size() const { return items_.size(); }

    std::vector<std::shared_ptr<T>> getAll() const {
        std::vector<std::shared_ptr<T>> result;
        result.reserve(items_.size());
        for (const auto& [key, item] : items_) {
            result.push_back(item);
        }
        return result;
    }

    // lambdas goes brrrrrrrrr
    // Returns the FIRST instance that matches the predicate (or nullptr if none match)
    std::shared_ptr<T> findIf(std::function<bool(const T&)> predicate) const {
        for (const auto& [key, item] : items_) {
            if (item && predicate(*item)) {
                return item; // Found the first match, return it immediately
            }
        }
        return nullptr; // No matching item found
    }
    
    // generic search hook, every specialized query below is built on this
    std::vector<std::shared_ptr<T>> findAll(std::function<bool(const T&)> predicate) const {
        std::vector<std::shared_ptr<T>> result;
        for (const auto& [key, item] : items_) {
            if (item && predicate(*item)) {
                result.push_back(item);
            }
        }
        return result;
    }
};

}  // namespace airline