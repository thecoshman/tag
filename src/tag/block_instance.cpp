#include "tag/block_instance.hpp"

namespace tag {    
    void block_instance::set_meta_value(std::string key, std::string value){
        if(!meta_data){
            meta_data = std::make_unique<std::map<std::string, std::string>>();
        }
        meta_data.get()->insert_or_assign(key, value);
    }
    
    std::string block_instance::get_meta_value(std::string key) const {
        if(!meta_data){
            return ""; // If map doesn't exist, then there's no value to return
        }
        auto search = meta_data.get()->find(key);
        if (search != meta_data.get()->end()) {
            return search->second;
        }
        return "";
    }
    
    void block_instance::erase_meta_value(std::string key){
        if(!meta_data){
            return; // If map doesn't exist, there's nothing to erase
        }
        meta_data.get()->erase(key);
        if(meta_data.get()->empty()){
            // TODO decide if I want to delete the map once it's empty
        }
    }
}