#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace util {
    template<typename T>
    struct registry {
        // Ensures a given name is registered
        int register_name(const std::string& name) {
            auto next_index = static_cast<int>(name_mapping.size());
            auto insertion_result = name_mapping.insert({name, next_index});
            // insertion_result.first is iterator to new/existing item with this name
            // insertion_result.second is boolean value saying if it was a new item
            if(insertion_result.second) {
                // A new name was added to the mapping
                data.push_back(std::nullopt);
            }
            return insertion_result.first->second; // return the ID the new/existing
        }

        std::optional<T> get(const std::string& name) const {
            return get(get_id(name));
        }
        
        std::optional<T> get(const std::optional<int>& id) const {
            if(!id) {
                return std::nullopt;
            }
            auto index = static_cast<size_t>(*id);
            if(index < 0 || index > data.size()) {
                return std::nullopt;
            }
            auto item = data.at(index);
            if(!item) {
                return std::nullopt;
            }
            
            return *item;
        }
        
        std::optional<int> get_id(const std::string& name) const {
            auto search = name_mapping.find(name);
            if (search == name_mapping.end()) {
                return std::nullopt;
            }
            return search->second;
        }
        
        void set(const std::string& name, T item) {
            set(register_name(name), item);
        }

        private:
        std::unordered_map<std::string, int> name_mapping;
        std::vector<std::optional<T>> data;

        void set(const std::optional<int>& id, T item) {
            if(!id) {
                return;
            }
            auto index = static_cast<size_t>(*id);
            if(index < 0 || index > data.size()) {
                return;
            }
            
            data[index] = item;
        }
    };
}