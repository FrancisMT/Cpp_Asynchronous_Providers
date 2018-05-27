#pragma once

#include<vector>
#include<string>
#include<fstream>
#include<iostream>

namespace HelperFunctions {
    template<class T>
    std::tuple<std::vector<T>, std::vector<T>> split_vector(std::vector<T> const& original_vector, const size_t split_at_index) {
        
        auto split_left = std::vector<T>(std::begin(original_vector), std::begin(original_vector)+split_at_index);
        auto split_right = std::vector<T>(std::begin(original_vector) + split_at_index, std::end(original_vector));

        return std::make_tuple(split_left, split_right);
    }

    std::vector<char> readFile(std::string const& inPath) 
    { 
        std::ifstream file(inPath, std::ios::binary | std::ios::ate); 
        size_t length = (size_t)file.tellg(); 
        
        std::vector<char> buffer(length); 
        file.seekg(0, std::ios::beg); 
        file.read(&buffer[0], length); 
        
        return buffer; 
    } 
  
    size_t writeFile(std::vector<char> const& buffer, std::string const& outPath) 
    { 
        std::ofstream file(outPath, std::ios::binary); 
        file.write(&buffer[0], buffer.size()); 
        
        return (size_t)file.tellp(); 
    } 
}