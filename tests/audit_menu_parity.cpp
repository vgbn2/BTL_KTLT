// Audit probe: link with the project library objects; run from an isolated directory.
#include "GoiCuocMenu.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: audit_menu_parity <repository-file> <create|menu|model> <name|price|data|minutes|cycle>\n";
        return 2;
    }
    const std::string path = argv[1];
    const std::string mode = argv[2];
    const std::string field = argv[3];
    if ((mode != "create" && mode != "menu" && mode != "model") ||
        (field != "name" && field != "price" && field != "data" &&
         field != "minutes" && field != "cycle")) return 2;
    try {
        Repository<GoiCuoc> store(path);
        if (mode != "create") store.add(GoiCuoc("AUDIT", "Original", 50000, 5, 100, 30));
        if (mode == "model") {
            GoiCuoc updated = *store.findById("AUDIT");
            if (field == "name") updated.setTenGoi("Renamed");
            if (field == "price") updated.setGiaCuoc(60000);
            if (field == "data") updated.setDungLuongData(10);
            if (field == "minutes") updated.setSoPhutGoi(250);
            if (field == "cycle") updated.setChuKyNgay(60);
            store.update("AUDIT", updated);
        } else {
            GoiCuocMenu menu(store);
            menu.showMenu();
        }
        Repository<GoiCuoc> reloaded(path);
        if (!reloaded.loadFromFile() || reloaded.size() != 1) return 2;
        const GoiCuoc* record = reloaded.findById("AUDIT");
        if (!record) return 2;
        std::cout << "\nPERSISTED " << record->toFileString() << '\n';
        bool correct = false;
        if (mode == "create") {
            correct = record->getTenGoi() == "Created" && record->getGiaCuoc() == 50000 &&
                      record->getDungLuongData() == 5 && record->getSoPhutGoi() == 100 &&
                      record->getChuKyNgay() == 30;
        } else {
            correct = record->getTenGoi() == (field == "name" ? "Renamed" : "Original") &&
                      record->getGiaCuoc() == (field == "price" ? 60000 : 50000) &&
                      record->getDungLuongData() == (field == "data" ? 10 : 5) &&
                      record->getSoPhutGoi() == (field == "minutes" ? 250 : 100) &&
                      record->getChuKyNgay() == (field == "cycle" ? 60 : 30);
        }
        if (!correct) std::cerr << "PARITY FAILURE: " << mode << '/' << field << '\n';
        return correct ? 0 : 1;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 2;
    }
}
