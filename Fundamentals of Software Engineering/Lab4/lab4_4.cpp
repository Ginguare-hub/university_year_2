#include <algorithm>
#include <clocale>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <locale>
#include <map>
#include <set>
#include <string>
#include <vector>

struct Car
{
    std::string model;
    double consumption;
    int mileage;
};

template <typename T, typename Pred>
std::vector<T> filterBy(const std::vector<T> &src, Pred pred)
{
    std::vector<T> out;
    for (const auto &item : src)
    {
        if (pred(item))
        {
            out.push_back(item);
        }
    }
    return out;
}

template <typename T, typename Compare>
const T *findBest(const std::vector<T> &src, Compare cmp)
{
    if (src.empty())
    {
        return nullptr;
    }
    const T *best = &src.front();
    for (const auto &item : src)
    {
        if (cmp(item, *best))
        {
            best = &item;
        }
    }
    return best;
}

template <typename T, typename Proj>
double sumBy(const std::vector<T> &src, Proj proj)
{
    double total = 0.0;
    for (const auto &item : src)
    {
        total += static_cast<double>(proj(item));
    }
    return total;
}

struct ConsumptionAbove
{
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

int main()
{
    setlocale(LC_ALL, "ru.UTF-8");

    std::vector<Car> cars =
        {
            {"Toyota Camry", 8.5, 120000},
            {"Ford Focus", 6.9, 85000},
            {"BMW X5", 12.3, 45000},
            {"Lada Vesta", 7.1, 60000},
            {"Mercedes C200", 9.4, 30000}};

    std::sort(cars.begin(), cars.end(),
              [](const Car &a, const Car &b)
              {
                  return a.consumption < b.consumption;
              });

    std::cout << "Парк по возрастанию расхода:\n";
    std::cout << std::left << std::setw(20) << "Модель"
              << std::right << std::setw(20) << "Расход"
              << std::setw(15) << "Пробег" << '\n';
    std::cout << std::string(38, '-') << '\n';
    for (const auto &car : cars)
    {
        std::cout << std::left << std::setw(16) << car.model
                  << std::right << std::setw(10) << std::fixed << std::setprecision(1)
                  << car.consumption
                  << std::setw(12) << car.mileage << '\n';
    }

    double limit = 0.0;
    std::cout << "\nВведите предел расхода (> 0): ";
    if (!(std::cin >> limit) || limit <= 0.0)
    {
        std::cerr << "Некорректный предел расхода\n";
        return 1;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    const std::size_t aboveCount =
        static_cast<std::size_t>(std::count_if(cars.begin(), cars.end(), ConsumptionAbove(limit)));
    std::cout << "Автомобилей с расходом выше " << std::fixed << std::setprecision(1)
              << limit << ": " << aboveCount << '\n';

    double fuelPrice = 0.0;
    std::cout << "\nВведите цену литра топлива (> 0): ";
    if (!(std::cin >> fuelPrice) || fuelPrice <= 0.0)
    {
        std::cerr << "Некорректная цена топлива\n";
        return 1;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "\nСтоимость 100 км:\n";
    std::cout << std::left << std::setw(20) << "Модель"
              << std::right << std::setw(25) << "Стоимость" << '\n';
    std::cout << std::string(28, '-') << '\n';
    std::for_each(cars.begin(), cars.end(), [fuelPrice](const Car &car)
                  {
                      const double cost = car.consumption * fuelPrice;
                      std::cout << std::left << std::setw(16) << car.model
                                << std::right << std::setw(12) << std::fixed
                                << std::setprecision(2) << cost << '\n'; });

    std::set<Car, ByMileageDesc> byMileage(cars.begin(), cars.end());
    std::cout << "\nАвтомобили по убыванию пробега:\n";
    std::cout << std::left << std::setw(16) << "Модель" << std::right << std::setw(24) << "Пробег" << '\n';
    std::cout << std::string(28, '-') << '\n';
    for (const auto &car : byMileage)
    {
        std::cout << std::left << std::setw(16) << car.model
                  << std::right << std::setw(12) << car.mileage << '\n';
    }

    std::map<std::string, std::function<bool(const Car &)>> commands;

    commands["economy"] = [](const Car &car)
    {
        return car.consumption < 8.0;
    };
    commands["worn"] = [](const Car &car)
    {
        return car.mileage > 50000;
    };
    commands["premium"] = [](const Car &car)
    {
        return car.model.find("BMW") != std::string::npos || car.model.find("Mercedes") != std::string::npos;
    };

    bool running = true;
    while (running)
    {
        std::cout << "\nКоманда (economy/worn/premium, пусто — выход): ";
        std::string cmd;
        if (!std::getline(std::cin, cmd))
        {
            std::cerr << "Ошибка чтения\n";
            running = false;
        }
        else if (cmd.empty())
        {
            running = false;
        }
        else
        {
            auto cmdIt = commands.find(cmd);
            if (cmdIt == commands.end())
            {
                std::cerr << "Неизвестная команда: " << cmd << '\n';
            }
            else
            {
                const auto selected = filterBy(cars, cmdIt->second);
                std::cout << "\nОтобрано (" << selected.size() << "):\n";
                if (selected.empty())
                {
                    std::cout << "  (пусто)\n";
                }
                else
                {
                    for (const auto &car : selected)
                    {
                        std::cout << "  " << std::left << std::setw(16) << car.model
                                  << " расход " << std::fixed << std::setprecision(1)
                                  << car.consumption
                                  << ", пробег " << car.mileage << '\n';
                    }
                }

                const Car *best = findBest(selected,
                                           [](const Car &a, const Car &b)
                                           {
                                               return a.consumption < b.consumption;
                                           });
                if (best != nullptr)
                {
                    std::cout << "Лучшая по минимальному расходу: " << best->model
                              << " (" << std::fixed << std::setprecision(1)
                              << best->consumption << ")\n";
                }
                else
                {
                    std::cout << "Лучшая: нет данных\n";
                }

                const double totalMileage = sumBy(selected,
                                                  [](const Car &car)
                                                  {
                                                      return car.mileage;
                                                  });
                std::cout << "Суммарный пробег отобранных: " << totalMileage << '\n';
            }
        }
    }

    const std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const auto evens = filterBy(numbers, [](int x)
                                { return x % 2 == 0; });
    std::cout << "\nfilterBy для std::vector<int> (чётные): ";
    for (int v : evens)
    {
        std::cout << v << ' ';
    }
    std::cout << '\n';

    return 0;
}