#include "BTSRegister.h"
#include "fileio.h"
#include "Normalized.h"
#include <fstream>
#include <sstream>

//tao danh ba tram tu tep mac dinh
BTSRegistry::BTSRegistry() {
    LoadFromFile();
}

//tao danh ba tram tu tep chi dinh
BTSRegistry::BTSRegistry(const std::string& filePath) {
    LoadFromFile(filePath);
}

//nap danh sach tram phat song tu tep
bool BTSRegistry::LoadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    towers.clear();
    std::string line;
    while (std::getline(file, line)) {
        line = Normalized::trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        auto tokens = FileIO::split(line, '|');
        if (tokens.size() >= 13) {
            BTSRecord rec;
            rec.btsId = tokens[0];
            rec.ProvinceCode = tokens[5];
            rec.district = tokens[6];
            rec.ward = tokens[7];
            rec.Adress = tokens[8];
            rec.radiotech = tokens[11];
            rec.carrier = tokens[12];
            try {
                rec.latitude = std::stod(tokens[9]);
                rec.longtitude = std::stod(tokens[10]);
            } catch (...) {
                rec.latitude = 0.0;
                rec.longtitude = 0.0;
            }
            towers.push_back(rec);
        }
    }
    return !towers.empty();
}

//tim tram phat song theo ma
const BTSRecord* BTSRegistry::findById(const std::string& btsId) const {
    for (const auto& tower : towers) {
        if (tower.btsId == btsId) {
            return &tower;
        }
    }
    return nullptr;
}

//goi y tram phat song theo khu vuc
std::string BTSRegistry::suggestNearestBTS(const std::string& districtOrCity) const {
    if (towers.empty()) return "BTS-HN-001";
    std::string query = Normalized::toLower(Normalized::trim(districtOrCity));
    for (const auto& tower : towers) {
        std::string dist = Normalized::toLower(tower.district);
        std::string prov = Normalized::toLower(tower.ProvinceCode);
        if (dist.find(query) != std::string::npos || query.find(dist) != std::string::npos ||
            prov == query) {
            return tower.btsId;
        }
    }
    return towers.front().btsId;
}
