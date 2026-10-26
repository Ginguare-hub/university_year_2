#include <iostream>
#include <iomanip>
#include <string>
#include <deque>
#include <algorithm>
#include <clocale>

struct Car
{
    std::string brand;
    double fuel;
    int mileage;
};

void printTable(const std::deque<Car> &cars, const std::string &title)
{
    std::cout << '\n'
              << title << '\n';
    std::cout << std::left
              << std::setw(6) << "№"
              << std::setw(26) << "Марка"
              << std::setw(13) << "Расход"
              << std::setw(10) << "Пробег" << '\n';
    std::cout << std::string(37, '-') << '\n';

    int idx = 1;
    for (const auto &c : cars)
    {
        std::cout << std::left << std::setw(4) << idx++
                  << std::left << std::setw(16) << c.brand
                  << std::fixed << std::setprecision(1)
                  << std::right << std::setw(10) << c.fuel
                  << std::right << std::setw(9) << c.mileage << '\n';
    }
}

int main()
{
    setlocale(LC_ALL, "ru.UTF-8");

    std::deque<Car> cars =
        {
            {"Toyota Camry", 8.5, 120000},
            {"Ford Focus", 6.9, 85000},
            {"BMW X5", 12.3, 45000},
            {"Lada Vesta", 7.1, 60000},
            {"Mercedes C200", 9.4, 30000}};

    printTable(cars, "Исходный автопарк:");

    std::deque<Car> priority;
    auto it = cars.begin();
    while (it != cars.end())
    {
        if (it->mileage > 80000)
        {
            priority.push_front(*it);
            it = cars.erase(it);
        }
        else
        {
            ++it;
        }
    }
    for (auto rit = priority.rbegin(); rit != priority.rend(); ++rit)
        cars.push_front(*rit);

    printTable(cars, "После push_front (пробег > 80000 — в начало, приоритет на ТО):");

    std::sort(cars.begin(), cars.end(),
              [](const Car &a, const Car &b)
              { return a.mileage > b.mileage; });

    printTable(cars, "После std::sort по пробегу (по убыванию):");

    return 0;
}