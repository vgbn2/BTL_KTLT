#ifndef BTSRegister_H
#define BTSRegister_H
#include <vector>
#include <string>
struct BTSRecord{
    std::string btsId,
                ProvinceCode,   //tinh/tpho
                district,       //quan/huyen
                ward,           //phuong
                Adress,         
                carrier,
                radiotech;
    double latitude;
    double longtitude;
};
class BTSRegistry{
    private:
        std::vector<BTSRecord>towers;
    public:
    BTSRegistry();
   explicit BTSRegistry(const std::string& filePath);
    bool LoadFromFile(const std::string& filePath = "data/bts_vietnam.txt");
    size_t size()const{ return towers.size(); };

    const BTSRecord* findById(const std::string& btsId)const;
    std::string suggestNearestBTS(const std::string& districtOrCity)const;
};

#endif // BTSRegister_H