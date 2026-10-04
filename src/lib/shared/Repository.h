#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <functional>
#include <cstdio>
#include "fileio.h"
#include "Exceptions.h"

namespace RepositoryConstants {
    const std::string TEMP_FILE_EXTENSION = ".tmp";
}

template <typename T>
class Repository {
private:
    std::string filePath;
    std::vector<T> items;

public:
    explicit Repository(const std::string& path) : filePath(path) {}

    const std::string& getFilePath() const { return filePath; }
    size_t size() const { return items.size(); }
    bool empty() const { return items.empty(); }

    std::vector<T>& getAll() { return items; }
    const std::vector<T>& getAll() const { return items; }

    bool loadFromFile() {
        items.clear();
        FileIO::ensureDirectoryExists(filePath);
        std::ifstream inFile(filePath);
        if (!inFile.is_open()) {
            return false;
        }

        std::string line;
        while (std::getline(inFile, line)) {
            std::string trimmedLine = FileIO::trim(line);
            if (trimmedLine.empty() || trimmedLine[0] == FileConstants::COMMENT_PREFIX) {
                continue;
            }
            T item;
            if (item.fromFileString(trimmedLine)) {
                items.push_back(item);
            }
        }
        inFile.close();
        return true;
    }

    bool saveToFile() const {
        FileIO::ensureDirectoryExists(filePath);
        std::string tempPath = filePath + RepositoryConstants::TEMP_FILE_EXTENSION;
        std::ofstream outFile(tempPath);
        if (!outFile.is_open()) {
            throw FileIOException("Khong the mo tep tin tam de ghi: " + tempPath);
        }

        for (const auto& item : items) {
            outFile << item.toFileString() << "\n";
        }
        outFile.close();

        std::remove(filePath.c_str());
        if (std::rename(tempPath.c_str(), filePath.c_str()) != 0) {
            throw FileIOException("Khong the ghi de tep tin: " + filePath);
        }
        return true;
    }

    bool add(const T& item) {
        if (findById(item.getId()) != nullptr) {
            throw DuplicateIdException(item.getId());
        }
        items.push_back(item);
        return saveToFile();
    }

    bool update(const std::string& id, const T& updatedItem) {
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i].getId() == id) {
                items[i] = updatedItem;
                return saveToFile();
            }
        }
        throw NotFoundException(id);
    }

    bool remove(const std::string& id) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->getId() == id) {
                items.erase(it);
                return saveToFile();
            }
        }
        throw NotFoundException(id);
    }

    T* findById(const std::string& id) {
        for (auto& item : items) {
            if (item.getId() == id) {
                return &item;
            }
        }
        return nullptr;
    }

    const T* findById(const std::string& id) const {
        for (const auto& item : items) {
            if (item.getId() == id) {
                return &item;
            }
        }
        return nullptr;
    }

    std::vector<T> filter(std::function<bool(const T&)> predicate) const {
        std::vector<T> result;
        for (const auto& item : items) {
            if (predicate(item)) {
                result.push_back(item);
            }
        }
        return result;
    }

    void sort(std::function<bool(const T&, const T&)> comparator) {
        std::sort(items.begin(), items.end(), comparator);
    }

    void clear() {
        items.clear();
        saveToFile();
    }
};

#endif // REPOSITORY_H
