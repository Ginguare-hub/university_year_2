#include <iostream>
#include <string>
#include <set>
#include <map>

int main()
{
    setlocale(LC_ALL, "ru.utf-8");

    const int N = 10;
    const std::string trips[N] = {
        "Lada Vesta", "Toyota Camry", "Lada Vesta", "Ford Focus", "BMW X5",
        "Lada Vesta", "Toyota Camry", "Mercedes C200", "Ford Focus", "Lada Vesta"};

    std::set<std::string> uniqueModels;
    for (const auto &trip : trips)
    {
        uniqueModels.insert(trip);
    }

    std::cout << "Уникальные модели:\n";
    for (const auto &model : uniqueModels)
    {
        std::cout << model << '\n';
    }

    std::set<std::string> uniqueManufacturers;
    for (const auto &model : trips)
    {
        std::size_t pos = model.find(' ');
        std::string manufacturer = (pos != std::string::npos) ? model.substr(0, pos) : model;
        uniqueManufacturers.insert(manufacturer);
    }

    std::cout << "\nУникальные производители:\n";
    for (const auto &manuf : uniqueManufacturers)
    {
        std::cout << manuf << '\n';
    }

    std::map<std::string, int> modelCounts;
    for (const auto &model : trips)
    {
        ++modelCounts[model];
    }

    std::string mostFrequentModel;
    int maxCount = 0;
    for (const auto &[model, count] : modelCounts)
    {
        if (count > maxCount)
        {
            maxCount = count;
            mostFrequentModel = model;
        }
    }

    std::cout << "\nСамая загруженная модель: " << mostFrequentModel << " (выездов: " << maxCount << ")\n";

    std::cout << "\nВведите модель для поиска: ";
    std::string query;
    if (!std::getline(std::cin, query) || query.empty())
    {
        std::cerr << "Ошибка: пустой ввод\n";
        return 1;
    }

    auto it = modelCounts.find(query);
    if (it != modelCounts.end())
    {
        std::cout << "Модель " << query << " встречается " << it->second << " раз(а)\n";
    }
    else
    {
        std::cout << "Модель " << query << " не найдена\n";
    }

    return 0;
}
