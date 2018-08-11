#pragma once

#include <string>
#include <map>
#include <memory>

// #include "util/value_ptr.hpp"

namespace tag {
    struct block_instance {
        const int type_id;
        int val_a, val_b, val_c;
        private:
        std::unique_ptr<std::map<std::string, std::string>> meta_data;
        
        public:    
        block_instance(int type_id, int val_a, int val_b, int val_c): type_id(type_id), val_a(val_a), val_b(val_b), val_c(val_c){
        }
        
        ~block_instance() = default;
        block_instance(const block_instance& other) = delete;
        block_instance(block_instance&& other) noexcept = default;
        block_instance& operator=(const block_instance& other) = delete;//{
        //     return *this = block_instance(other);
        // }
        block_instance& operator=(block_instance&& other) noexcept = default;// {
        //     type_id = other.type_id;
        //     val_a = other.val_a;
        //     val_b = other.val_b;
        //     val_c = other.val_c;
        //     meta_data = std::move(other.meta_data);
        //     return *this;
        // };
        
        void set_meta_value(std::string key, std::string value);
        
        std::string get_meta_value(std::string key) const;
        
        void erase_meta_value(std::string key);
    };
}