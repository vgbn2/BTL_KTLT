/**
 * @file Exceptions.h
 * Thanh vien phu trach: Trần Đức Anh (B24DCVT021)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

class AppException : public std::runtime_error {
public:
    //tao loi ung dung voi thong diep
    explicit AppException(const std::string& message) : std::runtime_error(message) {}
};

class DuplicateIdException : public AppException {
public:
    //tao loi trung ma dinh danh
    explicit DuplicateIdException(const std::string& id)
        : AppException("Loi: Ma dinh danh '" + id + "' da ton tai trong he thong!") {}
};

class NotFoundException : public AppException {
public:
    //tao loi khong tim thay ban ghi
    explicit NotFoundException(const std::string& id)
        : AppException("Loi: Khong tim thay ban ghi co ma '" + id + "'!") {}
};

class InvalidDateException : public AppException {
public:
    //tao loi ngay thang khong hop le
    explicit InvalidDateException(const std::string& message)
        : AppException("Loi ngay thang: " + message) {}
};

class InvalidLuhnException : public AppException {
public:
    //tao loi IMEI khong hop le
    explicit InvalidLuhnException(const std::string& imei)
        : AppException("Loi IMEI: Ma '" + imei + "' khong hop le theo chuan 3GPP (Luhn Checksum that bai)!") {}
};

class InvalidPhoneNumberException : public AppException {
public:
    //tao loi so dien thoai khong hop le
    explicit InvalidPhoneNumberException(const std::string& phone)
        : AppException("Loi so dien thoai: '" + phone + "' khong dung dinh dang thue bao di dong cua nha mang") {}
};
class InvalidCredentialException : public AppException {
public:
    //tao loi CCCD khong hop le
    explicit InvalidCredentialException(const std::string& credential)
        : AppException("Loi CCCD: '" + credential + "' khong dung dinh dang") {}
};
class FileIOException : public AppException {
public:
    //tao loi thao tac tep tin
    explicit FileIOException(const std::string& message)
        : AppException("Loi tep tin: " + message) {}
};

#endif // EXCEPTIONS_H
