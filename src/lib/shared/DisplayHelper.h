#ifndef DISPLAY_HELPER_H
#define DISPLAY_HELPER_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <utility>

namespace DisplayHelper {

inline void printBorder(const std::vector<int>& widths, std::ostream& os = std::cout) {
    os << "+";
    for (int w : widths) {
        os << std::string(w > 0 ? w : 1, '-') << "+";
    }
    os << "\n";
}

inline void printHeader(const std::vector<std::string>& headers,
                        const std::vector<int>& widths,
                        const std::vector<bool>& rightAlign = {},
                        std::ostream& os = std::cout) {
    printBorder(widths, os);
    os << "|";
    for (size_t i = 0; i < headers.size(); ++i) {
        int w = (i < widths.size()) ? widths[i] : static_cast<int>(headers[i].length() + 2);
        bool alignRight = (i < rightAlign.size()) && rightAlign[i];
        if (alignRight) {
            os << std::right << std::setw(w - 1) << headers[i] << " |";
        } else {
            os << " " << std::left << std::setw(w - 1) << headers[i] << "|";
        }
    }
    os << "\n";
    printBorder(widths, os);
}

inline void printRow(const std::vector<std::string>& cells,
                     const std::vector<int>& widths,
                     const std::vector<bool>& rightAlign = {},
                     std::ostream& os = std::cout) {
    os << "|";
    for (size_t i = 0; i < cells.size(); ++i) {
        int w = (i < widths.size()) ? widths[i] : static_cast<int>(cells[i].length() + 2);
        bool alignRight = (i < rightAlign.size()) && rightAlign[i];
        if (alignRight) {
            os << std::right << std::setw(w - 1) << cells[i] << " |";
        } else {
            os << " " << std::left << std::setw(w - 1) << cells[i] << "|";
        }
    }
    os << "\n";
}

inline void printCard(const std::vector<std::pair<std::string, std::string>>& fields,
                      int labelWidth = 16,
                      std::ostream& os = std::cout) {
    os << "  --------------------------------------------------\n";
    for (const auto& kv : fields) {
        os << "  " << std::left << std::setw(labelWidth) << kv.first << ": " << kv.second << "\n";
    }
    os << "  --------------------------------------------------\n";
}

} // namespace DisplayHelper

#endif // DISPLAY_HELPER_H
