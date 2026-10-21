#ifndef DISPLAY_HELPER_H
#define DISPLAY_HELPER_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

namespace DisplayHelper {
namespace detail {

//lay do rong cot theo cau hinh hoac do dai noi dung
inline int widthFor(const std::vector<int>& widths, size_t index, const std::string& cell) {
    return index < widths.size() ? widths[index] : static_cast<int>(cell.size()) + 2;
}

//xac dinh cot co can phai hay khong
inline bool isRightAligned(const std::vector<bool>& rightAlign, size_t index) {
    return index < rightAlign.size() && rightAlign[index];
}

} // namespace detail

//in duong vien bang theo do rong cac cot
inline void printBorder(const std::vector<int>& widths, std::ostream& os = std::cout) {
    os << '+';
    for (int w : widths) os << std::string(w > 0 ? w : 1, '-') << '+';
    os << '\n';
}

//in mot dong du lieu voi can le cot
inline void printRow(const std::vector<std::string>& cells,
                     const std::vector<int>& widths,
                     const std::vector<bool>& rightAlign = {},
                     std::ostream& os = std::cout) {
    os << '|';
    for (size_t i = 0; i < cells.size(); ++i) {
        const int width = detail::widthFor(widths, i, cells[i]);
        if (detail::isRightAligned(rightAlign, i))
            os << std::right << std::setw(width - 1) << cells[i] << " |";
        else
            os << ' ' << std::left << std::setw(width - 1) << cells[i] << '|';
    }
    os << '\n';
}

//in tieu de bang kem duong vien
inline void printHeader(const std::vector<std::string>& headers,
                        const std::vector<int>& widths,
                        const std::vector<bool>& rightAlign = {},
                        std::ostream& os = std::cout) {
    printBorder(widths, os);
    printRow(headers, widths, rightAlign, os);
    printBorder(widths, os);
}

//in thong tin chi tiet dang the
inline void printCard(const std::vector<std::pair<std::string, std::string>>& fields,
                      int labelWidth = 16,
                      std::ostream& os = std::cout) {
    const std::string line = "  " + std::string(50, '-') + '\n';
    os << line;
    for (const auto& field : fields)
        os << "  " << std::left << std::setw(labelWidth) << field.first << ": " << field.second << '\n';
    os << line;
}

template <typename T>
//in tieu de va cac dong cua danh sach doi tuong
inline void printTable(const std::vector<T>& items, std::ostream& os = std::cout) {
    if (items.empty()) return;
    items[0].displayHeader(os);
    for (const auto& item : items) {
        item.displayRow(os);
    }
}

//dinh dang so tien kem hau to hien thi
inline std::string formatCurrency(double amount, const std::string& suffix = "") {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << amount;
    if (!suffix.empty()) oss << " " << suffix;
    return oss.str();
}

} // namespace DisplayHelper

#endif // DISPLAY_HELPER_H