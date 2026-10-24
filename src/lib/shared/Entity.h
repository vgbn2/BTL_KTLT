/**
 * @file Entity.h
 * Thanh vien phu trach: Trần Đức Anh (B24DCVT021)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <iostream>

class Entity {
protected:
    std::string id;

public:
    //tao thuc the rong
    Entity() : id("") {}
    //tao thuc the voi ma dinh danh
    explicit Entity(const std::string& entityId) : id(entityId) {}
    //huy thuc the da tao
    virtual ~Entity() = default;

    //lay ma dinh danh thuc the
    std::string getId() const { return id; }
    //cap nhat ma dinh danh thuc the
    void setId(const std::string& entityId) { id = entityId; }

    //in tieu de bang cua thuc the
    virtual void displayHeader(std::ostream& os = std::cout) const = 0;
    //in mot dong du lieu cua thuc the
    virtual void displayRow(std::ostream& os = std::cout) const = 0;
    //in thong tin chi tiet cua thuc the
    virtual void displayDetail(std::ostream& os = std::cout) const = 0;

    //chuyen thuc the thanh dong du lieu
    virtual std::string toFileString() const = 0;
    //nap thuc the tu dong du lieu
    virtual bool fromFileString(const std::string& line) = 0;
};

#endif // ENTITY_H
