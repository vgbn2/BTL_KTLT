/**
 * @file BTSRegister.h
 * Thanh vien phu trach: Trần Đức Anh (B24DCVT021)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 * luu y cho rieng file nay, file nay la gimmick/thi nghiem ko that su 
 * duoc su dung o trong menu 
 */

#ifndef BTSRegister_H
#define BTSRegister_H
#include <vector>
#include <string>
struct BTSRecord{
    std::string btsId,
                ProvinceCode,   //tinh/tpho
                district,       //quan/huyen
                ward,           //phuong
                Adress,         //dia chi
                carrier,
                radiotech;
    double latitude;            //kinh tuyen vi tuyen 
    double longtitude;
};
class BTSRegistry{
    private:
        std::vector<BTSRecord>towers;   //la vector kph mang vi vector them moi bts va so luong de hon mang
    public:
    BTSRegistry();
   explicit BTSRegistry(const std::string& filePath);
    bool LoadFromFile(const std::string& filePath = "data/bts_vietnam.txt");
    //lay so luong tram phat song
    size_t size()const{ return towers.size(); };

    const BTSRecord* findById(const std::string& btsId)const;
    std::string suggestNearestBTS(const std::string& districtOrCity)const;
};

#endif // BTSRegister_H
