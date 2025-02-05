// Init

#include <iostream>
#include <cmath>
#include <string>

using namespace std;


// end init

// Функция для вычисления расстояния между двумя точками
float distance(float x1, float y1, float x2, float y2) {

    cout << "Вычисление расстояния между точками: " << endl;
    cout << "x1 = " + std::to_string(x1) << endl;
    cout << "y1 = " + std::to_string(y1) << endl;
    cout << "x2 = " + std::to_string(x2) << endl;
    cout << "y2 = " + std::to_string(y2) << endl;

    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int main() {
    system("chcp 1251"); // Кодировка Win терминала
    float xa, ya, xb, yb, xc, yc;

    // Опрос пользователя по точкам треугольника
    cout << "Введите координаты точки A (x y): ";
    cin >> xa >> ya;
    cout << "Введите координаты точки B (x y): ";
    cin >> xb >> yb;
    cout << "Введите координаты точки C (x y): ";
    cin >> xc >> yc;

    // Вычисление сторон треугольника
    cout << "Вычисление сторон треугольника: ";
    float AB = distance(xa, ya, xb, yb);
    cout << "Для AB: " + std::to_string(AB) << endl;
    float BC = distance(xb, yb, xc, yc);
    cout << "Для BC: " + std::to_string(BC) << endl;
    float CA = distance(xc, yc, xa, ya);
    cout << "Для CA: " + std::to_string(CA) << endl;

    // Вычисление периметра
    cout << "Вычисление периметра: " << endl;
    float perimeter = AB + BC + CA;
    cout << "Периметр: " + std::to_string(perimeter) << endl;

    // Вычисление площади треугольника по формуле Герона
    cout << "Вычисление площади: " << endl; 
    float perimeterHalved = perimeter / 2;      // Полупериметр
    cout << "Полупериметр равен: " + std::to_string(perimeterHalved) << endl;
    // Финальные вычисления \ формула
    float area = sqrt(perimeterHalved * (perimeterHalved - AB) * (perimeterHalved - BC) * (perimeterHalved - CA));

    // Вывод результатов
    cout << endl;
    cout << "##################################################" << endl;
    cout << endl << "Периметр треугольника: " << perimeter << endl << endl;
    cout << "Площадь треугольника: " << area << endl;

    return 0;
}
