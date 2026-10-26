#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <iomanip>
#include <limits>

struct Car
{
    std::string model;
    double consumption; // л/100км
    int mileage;        // км
};

class ConsumptionAbove
{
public:
    explicit ConsumptionAbove(double limit) : limit_(limit) {}

    bool operator()(const Car &car) const
    {
        return car.consumption > limit_;
    }

private:
    double limit_;
};

struct ByMileageDesc
{
    bool operator()(const Car &a, const Car &b) const
    {
        if (a.mileage != b.mileage)
        {
            return a.mileage > b.mileage;
        }
        return a.model < b.model;
    }
};

bool readPositiveDouble(const std::string &prompt, double &value)
{
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value)
        {
            if (value > 0.0)
            {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return true;
            }
            std::cerr << "Ошибка: значение должно быть > 0\n";
        }
        else
        {
            std::cerr << "Ошибка: некорректный ввод\n";
            std::cin.clear();
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void printHeader()
{
    std::cout << std::left << std::setw(26) << "Модель"
              << std::right << std::setw(10) << "Расход"
              << std::setw(18) << "Пробег" << '\n';
    std::cout << std::string(38, '-') << '\n';
}

void printCar(const Car &car)
{
    std::cout << std::left << std::setw(16) << car.model
              << std::right << std::fixed << std::setprecision(1)
              << std::setw(10) << car.consumption
              << std::setw(12) << car.mileage << '\n';
}

int main()
{
    setlocale(LC_ALL, "ru.utf-8");

    std::vector<Car> cars = {
        {"Toyota Camry", 8.5, 120000},
        {"Ford Focus", 6.9, 85000},
        {"BMW X5", 12.3, 45000},
        {"Lada Vesta", 7.1, 60000},
        {"Mercedes C200", 9.4, 30000}};

    // сортировка
    std::sort(cars.begin(), cars.end(),
              [](const Car &a, const Car &b)
              {
                  return a.consumption < b.consumption;
              });

    std::cout << "Автомобили по возрастанию расхода:\n";
    printHeader();
    for (const auto &car : cars)
    {
        printCar(car);
    }

    //функтор
    double limit = 0.0;
    readPositiveDouble("\nВведите предел расхода (л/100 км): ", limit);

    ConsumptionAbove aboveLimit(limit);
    auto countAbove = std::count_if(cars.begin(), cars.end(), aboveLimit);

    std::cout << "Автомобилей с расходом выше " << limit
              << ": " << countAbove << '\n';

    //лямбда
    double fuelPrice = 0.0;
    readPositiveDouble("Введите цену литра топлива (руб.): ", fuelPrice);

    std::cout << "\nСтоимость 100 км:\n";
    std::for_each(cars.begin(), cars.end(),
                  [fuelPrice](const Car &car)
                  {
                      double cost = car.consumption * fuelPrice;
                      std::cout << std::left << std::setw(16) << car.model
                                << std::right << std::fixed << std::setprecision(2)
                                << std::setw(12) << cost << " руб.\n";
                  });

    //set
    std::set<Car, ByMileageDesc> byMileage(cars.begin(), cars.end());

    std::cout << "\nАвтомобили по убыванию пробега:\n";
    printHeader();
    for (const auto &car : byMileage)
    {
        printCar(car);
    }

    return 0;
}
