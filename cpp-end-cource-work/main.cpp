#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <limits>
#include <unordered_set>
#include <array>

using namespace std;

// Обёртка для безопасного чтения переменных
template <typename T>
bool safeInput(T& value) {
    cin >> value;
    if (!cin) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

// Хранит информацию о товаре
struct Product {
    unsigned code;   // Первичный ключ
    string name;     // Название товара
    string type;     // Тип товара
    float basePrice; // Базовая цена
    float discount;  // Скидка (%)
    float salePrice; // Итоговая цена

    void recalcSalePrice() {
        salePrice = basePrice * (1.0f - discount / 100.0f);
        if (salePrice < 0) salePrice = 0;
    }
};

// Управляет операциями с базой данных
class PriceDB {
private:
    vector<Product> data;

    bool codeExists(unsigned c) const {
        for (auto& p : data)
            if (p.code == c) return true;
        return false;
    }

public:
    // Загружает базу из CSV (5 полей), пропуская ошибки и дубли
    void loadFromFile(const string& filename) {
        ifstream fin(filename);
        if (!fin.is_open()) {
            cerr << "Ошибка открытия файла: " << filename << endl;
            return;
        }
        data.clear();
        unordered_set<unsigned> seenCodes;
        int lineNumber = 0;
        string line;
        while (getline(fin, line)) {
            lineNumber++;
            if (line.empty()) continue;

            stringstream ss(line);
            vector<string> tokens;
            string token;
            while (getline(ss, token, ';')) {
                tokens.push_back(token);
            }
            if (tokens.size() != 5) {
                cerr << "Неверный формат в строке " << lineNumber << ", пропуск.\n";
                continue;
            }

            Product p;
            try {
                p.code = stoul(tokens[0]);
                p.name = tokens[1];
                p.type = tokens[2];
                p.basePrice = stof(tokens[3]);
                p.discount = stof(tokens[4]);
            } catch (...) {
                cerr << "Ошибка парсинга в строке " << lineNumber << ", пропуск.\n";
                continue;
            }
            if (p.basePrice < 0 || p.discount < 0 || p.discount > 100) {
                cerr << "Недопустимые значения в строке " << lineNumber << ", пропуск.\n";
                continue;
            }
            if (seenCodes.count(p.code)) {
                cerr << "Дублирующийся код " << p.code << " в строке " 
                     << lineNumber << ", пропуск.\n";
                continue;
            }

            p.recalcSalePrice();
            data.push_back(p);
            seenCodes.insert(p.code);
        }
        fin.close();
        cout << "База загружена: " << data.size() << " записей.\n";
    }

    // Сохраняет базу в CSV (5 полей)
    void saveToFile(const string& filename) const {
        ofstream fout(filename);
        if (!fout.is_open()) {
            cerr << "Ошибка записи в файл: " << filename << endl;
            return;
        }
        for (auto& p : data) {
            fout << p.code << ";"
                 << p.name << ";"
                 << p.type << ";"
                 << p.basePrice << ";"
                 << p.discount << "\n";
        }
        fout.close();
        cout << "База сохранена в файл: " << filename << endl;
    }

    static size_t utf8Len(const std::string& s) {
        size_t n = 0;
        for (unsigned char c : s)
            if ((c & 0b1100'0000) != 0b1000'0000) ++n; // не продолжение
        return n;
    }

    static std::string fmtFloat(float v) {          // две цифры после запятой
        std::ostringstream os; os.setf(std::ios::fixed); os.precision(2); os << v;
        return os.str();
    }

    static void printRow(const std::array<std::string,6>& r,
                     const size_t maxW[6])
    {
        for (int i = 0; i < 6; ++i) {
            size_t pad = maxW[i] - PriceDB::utf8Len(r[i]) + 2;
            std::cout << r[i] << std::string(pad, ' ');
        }
        std::cout << '\n';
    }

    // Печатает все записи
    void display() const {
        using Row = std::array<std::string,6>;
        std::vector<Row> rows;
        rows.emplace_back( Row{"Код","Наименование","Тип","Баз.цена","Скидка","Цена"} );
        for (auto& p : data)
            rows.emplace_back( Row{ std::to_string(p.code), p.name, p.type,
                                    fmtFloat(p.basePrice), fmtFloat(p.discount),
                                    fmtFloat(p.salePrice) } );

        size_t maxW[6]{};
        for (auto& r : rows)
            for (int i=0;i<6;++i)
                maxW[i] = std::max(maxW[i], utf8Len(r[i]));

        for (auto& r : rows) printRow(r, maxW);
    }


    // Добавляет запись с проверкой уникальности кода и корректности ввода
    void addRecord() {
        Product p;
        cout << "Введите код товара: ";
        if (!safeInput(p.code)) {
            cout << "Некорректный ввод.\n";
            return;
        }
        if (codeExists(p.code)) {
            cout << "Товар с кодом " << p.code << " уже существует.\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Название товара: ";
        getline(cin, p.name);
        cout << "Тип товара: ";
        getline(cin, p.type);

        cout << "Базовая цена: ";
        if (!safeInput(p.basePrice) || p.basePrice < 0) {
            cout << "Цена не может быть отрицательной или некорректной.\n";
            return;
        }

        cout << "Скидка (%): ";
        if (!safeInput(p.discount) || p.discount < 0 || p.discount > 100) {
            cout << "Скидка должна быть в диапазоне [0..100].\n";
            return;
        }

        p.recalcSalePrice();
        data.push_back(p);
        cout << "Товар добавлен.\n";
    }

    // Удаляет запись по коду
    void deleteRecord() {
        unsigned delCode;
        cout << "Введите код товара для удаления: ";
        if (!safeInput(delCode)) {
            cout << "Некорректный ввод.\n";
            return;
        }
        auto it = remove_if(data.begin(), data.end(),
                            [delCode](auto& p){ return p.code == delCode; });
        if (it != data.end()) {
            data.erase(it, data.end());
            cout << "Товар(ы) с кодом " << delCode << " удалён(ы).\n";
        } else {
            cout << "Товар с кодом " << delCode << " не найден.\n";
        }
    }

    // Сортирует товары по коду
    void sortByCode() {
        sort(data.begin(), data.end(),
             [](auto& a, auto& b){ return a.code < b.code; });
        cout << "Отсортировано по коду.\n";
    }

    // Сортирует товары по итоговой цене
    void sortBySalePrice() {
        sort(data.begin(), data.end(),
             [](auto& a, auto& b){ return a.salePrice < b.salePrice; });
        cout << "Отсортировано по итоговой цене.\n";
    }

    // Ищет запись по коду
    void searchByCode() {
        unsigned id;
        cout << "Введите код для поиска: ";
        if (!safeInput(id)) { 
            cout << "Некорректный ввод.\n"; 
            return; 
        }

        for (auto& p : data) {
            if (p.code == id) {
                using Row = std::array<std::string,6>;
                Row header {"Код","Наименование","Тип","Баз.цена","Скидка","Цена"};
                Row row { std::to_string(p.code), p.name, p.type,
                        fmtFloat(p.basePrice), fmtFloat(p.discount),
                        fmtFloat(p.salePrice) };

                size_t maxW[6]{};
                for (int i=0;i<6;++i)
                    maxW[i] = std::max( utf8Len(header[i]), utf8Len(row[i]) );

                printRow(header, maxW);
                printRow(row,    maxW);
                return;
            }
        }
        cout << "Товар с кодом " << id << " не найден.\n";
    }


    // Ищет подстроку в названии или типе
    void searchBySubstring()
    {
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Введите подстроку (поиск в названии и типе): ";
        string sub;
        getline(cin, sub);

        using Row = std::array<std::string, 6>;
        std::vector<Row> rows;
        rows.emplace_back( Row{ "Код", "Наименование", "Тип",
                                "Баз.цена", "Скидка", "Цена" } );

        for (const auto& p : data) {
            if (p.name.find(sub) != string::npos ||
                p.type.find(sub) != string::npos) {
                rows.emplace_back( Row{
                    std::to_string(p.code), p.name, p.type,
                    fmtFloat(p.basePrice), fmtFloat(p.discount),
                    fmtFloat(p.salePrice) } );
            }
        }

        if (rows.size() == 1) {
            cout << "Ничего не найдено.\n";
            return;
        }

        size_t maxW[6]{};
        for (auto& r : rows)
            for (int i = 0; i < 6; ++i)
                maxW[i] = std::max(maxW[i], utf8Len(r[i]));

        for (auto& r : rows) printRow(r, maxW);
    }

    // Применяет дополнительную скидку p% к товарам заданных типов
    void applyExtraDiscount() {
        float extra;
        cout << "Введите дополнительную скидку (p%): ";
        if (!safeInput(extra)) {
            cout << "Некорректный ввод.\n";
            return;
        }
        if (extra < 0 || extra > 100) {
            cout << "Скидка вне диапазона [0..100].\n";
            return;
        }

        cout << "Сколько типов товаров в акции? ";
        int countTypes;
        if (!safeInput(countTypes) || countTypes < 1) {
            cout << "Некорректное количество.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        unordered_set<string> targetTypes;
        for (int i = 0; i < countTypes; i++) {
            cout << "Введите тип " << (i+1) << ": ";
            string t;
            getline(cin, t);
            targetTypes.insert(t);
        }

        for (auto& p : data) {
            if (targetTypes.count(p.type)) {
                p.discount  = extra; //+= extra;
                if (p.discount > 100) p.discount = 100;
                p.recalcSalePrice();
            }
        }
        cout << "Акционная скидка " << extra << "% применена.\n";
    }

    // Удаляет все товары с ценой ниже заданного порога
    void removeBelowThreshold() {
        float threshold;
        cout << "Введите порог цены (P): ";
        if (!safeInput(threshold)) {
            cout << "Некорректный ввод.\n";
            return;
        }
        auto it = remove_if(data.begin(), data.end(),
                            [threshold](auto& p){ return p.salePrice < threshold; });
        if (it != data.end()) {
            data.erase(it, data.end());
            cout << "Товары с ценой ниже " << threshold << " удалены.\n";
        } else {
            cout << "Нет товаров с ценой ниже " << threshold << ".\n";
        }
    }
};

// Вывод пунктов меню
void showMenu() {
    cout << "\n--- Прайс-лист ---\n"
         << "1. Загрузить БД из файла\n"
         << "2. Просмотреть БД\n"
         << "3. Добавить запись\n"
         << "4. Удалить запись\n"
         << "5. Сохранить БД в файл\n"
         << "6. Сортировать по коду\n"
         << "7. Сортировать по итоговой цене\n"
         << "8. Поиск по коду\n"
         << "9. Поиск по подстроке (имя/тип)\n"
         << "10. Применить акционную скидку\n"
         << "11. Удалить товары с ценой ниже порога\n"
         << "0. Выход\n"
         << "Выберите действие: ";
}

int main() {
    setlocale(LC_ALL, "ru"); 
    PriceDB db;
    int choice;
    string filename = "price.csv";

    do {
        showMenu();
        if (!safeInput(choice)) {
            cout << "Неверный ввод.\n";
            choice = -1;
        }
        switch (choice) {
        case 1:  db.loadFromFile(filename); break;
        case 2:  db.display(); break;
        case 3:  db.addRecord(); break;
        case 4:  db.deleteRecord(); break;
        case 5:  db.saveToFile(filename); break;
        case 6:  db.sortByCode(); break;
        case 7:  db.sortBySalePrice(); break;
        case 8:  db.searchByCode(); break;
        case 9:  db.searchBySubstring(); break;
        case 10: db.applyExtraDiscount(); break;
        case 11: db.removeBelowThreshold(); break;
        case 0:  cout << "Выход...\n"; break;
        default: cout << "Неизвестная команда.\n"; break;
        }
    } while (choice != 0);

    return 0;
}
