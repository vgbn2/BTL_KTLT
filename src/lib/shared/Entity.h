#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <iostream>

class Entity {
protected:
    std::string id;

public:
    Entity() : id("") {}
    explicit Entity(const std::string& entityId) : id(entityId) {}
    virtual ~Entity() = default;

    std::string getId() const { return id; }
    void setId(const std::string& entityId) { id = entityId; }

    virtual void displayHeader() const = 0;
    virtual void displayRow() const = 0;
    virtual void displayDetail() const = 0;

    virtual std::string toFileString() const = 0;
    virtual bool fromFileString(const std::string& line) = 0;
};

#endif // ENTITY_H
