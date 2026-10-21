#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <functional>
#include <cstdio>
#include "fileio.h"
#include "Normalized.h"
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
    //tao kho du lieu gan voi tep
    explicit Repository(const std::string& path) : filePath(path) {}

    //lay duong dan tep du lieu
    const std::string& getFilePath() const { return filePath; }
    //lay so luong ban ghi
    size_t size() const { return items.size(); }
    //kiem tra kho co rong
    bool empty() const { return items.empty(); }

    //lay danh sach ban ghi co the sua
    std::vector<T>& getAll() { return items; }
    //lay danh sach ban ghi chi doc
    const std::vector<T>& getAll() const { return items; }

    //nap danh sach tu tep
    bool loadFromFile() {
        FileIO::ensureDirectoryExists(filePath);
        std::ifstream inFile(filePath);
        if (!inFile.is_open()) {
            return false;
        }

        std::vector<T> loadedItems;
        std::string line;
        while (std::getline(inFile, line)) {
            std::string trimmedLine = Normalized::trim(line);
            if (trimmedLine.empty() || trimmedLine[0] == FileConstants::COMMENT_PREFIX) continue;
            T item;
            if (!item.fromFileString(trimmedLine)) continue;
            loadedItems.push_back(item);
        }
        if (inFile.bad()) {
            return false;
        }
        inFile.close();
        items.swap(loadedItems);
        return true;
    }
    //luu danh sach vao tep theo cach an toan
    bool saveToFile() const {
        FileIO::ensureDirectoryExists(filePath);
        std::string tempPath = FileIO::getTempStagingPath(filePath);
        std::ofstream outFile(tempPath);
        if (!outFile.is_open()) {
            throw FileIOException("Khong the mo tep tin tam tren tmps/ de ghi: " + tempPath);
        }

        for (const auto& item : items) {
            outFile << item.toFileString() << "\n";
        }
        outFile.flush();
        if (!outFile.good()) {
            outFile.close();
            std::remove(tempPath.c_str());
            throw FileIOException("Khong the ghi day du tep tin tam: " + tempPath);
        }
        outFile.close();
        if (outFile.fail()) {
            std::remove(tempPath.c_str());
            throw FileIOException("Khong the dong tep tin tam: " + tempPath);
        }

        //viet trong ram truoc,roi commit sang o cung
        if (!FileIO::commitFromTmp(tempPath, filePath)) {
            std::remove(tempPath.c_str());
            throw FileIOException("Khong the ghi de tep tin: " + filePath);
        }
        return true;
    }

    //them ban ghi moi va luu tep
    bool add(const T& item) {
        if (findById(item.getId()) != nullptr) {
            throw DuplicateIdException(item.getId());
        }
        items.push_back(item);
        try {
            return saveToFile();
        } catch (...) {
            items.pop_back();
            throw;
        }
    }

    //cap nhat ban ghi theo ma va luu tep
    bool update(const std::string& id, const T& updatedItem) {
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i].getId() == id) {
                T previous = items[i];
                items[i] = updatedItem;
                try {
                    return saveToFile();
                } catch (...) {
                    items[i] = previous;
                    throw;
                }
            }
        }
        throw NotFoundException(id);
    }

    //xoa ban ghi theo ma va luu tep
    bool remove(const std::string& id) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->getId() == id) {
                const size_t index = static_cast<size_t>(it - items.begin());
                T previous = *it;
                items.erase(it);
                try {
                    return saveToFile();
                } catch (...) {
                    items.insert(items.begin() + index, previous);
                    throw;
                }
            }
        }
        throw NotFoundException(id);
    }

    //tim ban ghi co the sua theo ma
    T* findById(const std::string& id) {
        for (auto& item : items) {
            if (item.getId() == id) {
                return &item;
            }
        }
        return nullptr;
    }

    //tim ban ghi chi doc theo ma
    const T* findById(const std::string& id) const {
        for (const auto& item : items) {
            if (item.getId() == id) {
                return &item;
            }
        }
        return nullptr;
    }

    //loc ban ghi theo dieu kien
    std::vector<T> filter(std::function<bool(const T&)> predicate) const {
        std::vector<T> result;
        for (const auto& item : items) {
            if (predicate(item)) {
                result.push_back(item);
            }
        }
        return result;
    }

    //sap xep danh sach theo ham so sanh
    void sort(std::function<bool(const T&, const T&)> comparator) {
        std::sort(items.begin(), items.end(), comparator);
    }

    //xoa toan bo ban ghi va luu tep
    void clear() {
        std::vector<T> previous = items;
        items.clear();
        try {
            saveToFile();
        } catch (...) {
            items.swap(previous);
            throw;
        }
    }
};

#endif // REPOSITORY_H
