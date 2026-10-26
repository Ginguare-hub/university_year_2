#include <algorithm>
#include <clocale>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

struct Run
{
    std::string driver;
    std::string model;
    int km;
};

int main()
{
    setlocale(LC_ALL, "ru.UTF-8");


    const std::vector<Run> runs =
        {
            {"Петров", "Toyota Camry", 320},
            {"Соколов", "Lada Vesta", 150},
            {"Петров", "Ford Focus", 210},
            {"Волков", "BMW X5", 540},
            {"Соколов", "Lada Vesta", 180},
            {"Волков", "Mercedes C200", 260},
            {"Петров", "Toyota Camry", 400},
            {"Соколов", "Ford Focus", 90}};


    std::unordered_map<std::string, std::vector<std::size_t>> index;
    for (std::size_t i = 0; i < runs.size(); ++i)
    {
        index[runs[i].model].push_back(i);
    }

    std::map<std::string, int> driverKm;
    for (const auto &run : runs)
    {
        driverKm[run.driver] += run.km;
    }

    std::string model;
    std::cout << "Введите модель: ";
    if (!std::getline(std::cin, model))
    {
        std::cerr << "Ошибка чтения ввода\n";
        return 1;
    }


    if (model.empty())
    {
        std::cerr << "Пустой ввод\n";
        return 1;
    }

    auto it = index.find(model);

    if (it == index.end())
    {
        std::cout << "Модель \"" << model << "\" не найдена\n";
    }
    else
    {
        std::cout << "\nРейсы модели " << model << ":\n";
        std::cout << std::left << std::setw(12) << "Водитель"
                  << std::right << std::setw(8) << "Км" << '\n';
        std::cout << std::string(20, '-') << '\n';

        int modelTotal = 0;
        for (std::size_t pos : it->second)
        {
            const Run &r = runs[pos];

            std::cout << std::left << std::setw(12) << r.driver << std::right << std::setw(8) << r.km << '\n';

            modelTotal += r.km;
        }

        std::cout << std::string(20, '-') << '\n';
        std::cout << std::left << std::setw(12) << "Итого" << std::right << std::setw(8) << modelTotal << '\n';
    }

    std::cout << "\nПробег водителей:\n";
    std::cout << std::left << std::setw(12) << "Водитель" << std::right << std::setw(8) << "Км" << '\n';
    std::cout << std::string(20, '-') << '\n';

    for (const auto &[driver, km] : driverKm)
    {
        std::cout << std::left << std::setw(12) << driver << std::right << std::setw(8) << km << '\n';
    }

    return 0;
}