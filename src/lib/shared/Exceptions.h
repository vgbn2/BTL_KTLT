#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

class AppException : public std::runtime_error {
public:
    explicit AppException(const std::string& message) : std::runtime_error(message) {}
};

class DuplicateIdException : public AppException {
public:
    explicit DuplicateIdException(const std::string& id)
        : AppException("Loi: Ma dinh danh '" + id + "' da ton tai trong he thong!") {}
};

class NotFoundException : public AppException {
public:
    explicit NotFoundException(const std::string& id)
        : AppException("Loi: Khong tim thay ban ghi co ma '" + id + "'!") {}
};

class InvalidDateException : public AppException {
public:
    explicit InvalidDateException(const std::string& message)
        : AppException("Loi ngay thang: " + message) {}
};

class InvalidLuhnException : public AppException {
public:
    explicit InvalidLuhnException(const std::string& imei)
        : AppException("Loi IMEI: Ma '" + imei + "' khong hop le theo chuan 3GPP (Luhn Checksum that bai)!") {}
};

class InvalidPhoneNumberException : public AppException {
public:
    explicit InvalidPhoneNumberException(const std::string& phone)
        : AppException("Loi so dien thoai: '" + phone + "' khong dung dinh dang thue bao di dong cua nha mang VNPT") {}
};

class FileIOException : public AppException {
public:
    explicit FileIOException(const std::string& message)
        : AppException("Loi tep tin: " + message) {}
};

#endif // EXCEPTIONS_H
