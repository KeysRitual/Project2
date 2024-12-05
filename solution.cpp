#include "FileAVL.hpp"
#include "File.hpp"
#include "FileTrie.hpp"

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

// ALL YOUR CODE SHOULD BE IN THIS FILE. NO MODIFICATIONS SHOULD BE MADE TO FILEAVL / FILE CLASSES
// You are permitted to make helper functions (and most likely will need to)
// You must declare them "inline" | declare & implement them at the top of this file, before query()
// Below query(), implement and document all methods declared in FileTrie.hpp

//Helper function -> make lowercase
inline std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

//Helper function collect
inline void collectFilesFromNode(FileTrieNode* node, std::unordered_set<File*>& result) {
    if (!node) return;

    // Add all current node
    result.insert(node->matching.begin(), node->matching.end());

    // Recursively collect files from child
    for (const auto& entry : node->next) { collectFilesFromNode(entry.second, result); }
}

//Helper function to delete 
inline void deleteNodes(FileTrieNode* node) {
    if (node) {
        for (auto& entry : node->next) { deleteNodes(entry.second); }
        delete node;
    }
}

/**
 * @brief Retrieves all files in the FileAVL whose file sizes are within [min, max]
 *
 * @param min The min value of the file size query range.
 * @param max The max value of the file size query range.
 * @return std::vector<File*> storing pointers to all files in the tree within the given range.
 * @note If the query interval is in descending order (ie. the given parameters min >= max),
        the interval from [max, min] is searched (since max >= min)
 */
std::vector<File*> FileAVL::query(size_t min, size_t max) {
    std::vector<File*> result;
    // Your code here.
    if (min > max) { std::swap(min, max); }
    if (root_) { collectFilesInRange(root_, min, max, result); }

    return result;
}


//Traverse
inline void collectFilesInRange(Node* node, size_t min, size_t max, std::vector<File*>& result) {
    if (!node) return;

    // Traverse the left subtree 
    if (node->size_ > min) { collectFilesInRange(node->left_, min, max, result); }

    // Add files in the current node (if in range)
    if (node->size_ >= min && node->size_ <= max) { result.insert(result.end(), node->files_.begin(), node->files_.end()); }

    // Traverse the right subtree 
    if (node->size_ < max) { collectFilesInRange(node->right_, min, max, result); }
}


//ADDFILE
void FileTrie::addFile(File* f) {
    std::string filename = toLower(f->getName());
    FileTrieNode* currentNode = head;
    currentNode->matching.insert(f);

    //Traverse the trie, adding nodes as necessary
    for (char ch : filename) {
        if (currentNode->next.find(ch) == currentNode->next.end()) { currentNode->next[ch] = new FileTrieNode(ch); }
        currentNode = currentNode->next[ch];
    }
}

//GETFILES
std::unordered_set<File*> FileTrie::getFilesWithPrefix(const std::string& prefix) const {
    std::unordered_set<File*> result;
    std::string lowerPrefix = toLower(prefix);
    FileTrieNode* currentNode = head;

    //Traverse the trie prefix
    for (char ch : lowerPrefix) {
        if (currentNode->next.find(ch) == currentNode->next.end()) { return result; }
        currentNode = currentNode->next[ch];
    }

    //Collect all files
    collectFilesFromNode(currentNode, result);
    return result;
}

//Destructor
FileTrie::~FileTrie() { deleteNodes(head); }
FileTrie::FileTrie() : head(new FileTrieNode()
