#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <windows.h>

struct Car
{
    std::string name;
    double consumption;
    int mileage;
};

bool saveFleet(const std::string& filename, const Car cars[], int amount)
{
    std::ofstream out(filename);
    if (!out.is_open())
    {
        std::cerr << "Ошибка: не удалось открыть файл \"" << filename << "\" для записи\n";
        return false;
    }

    for (int i = 0; i < amount; ++i)
    {
        out << cars[i].name << ';'
            << std::fixed << std::setprecision(1) << cars[i].consumption << ';'
            << cars[i].mileage << '\n';
    }
    return true;
}

bool loadFleet(const std::string& filename)
{
    int count;
    double sumConsumption;
    Car maxMileageCar;
    bool hasMax;
    double avg;
    count = 0;
    sumConsumption = 0.0;
    hasMax = false;

    std::ifstream in(filename);
    if (!in.is_open())
    {
        std::cerr << "Ошибка: не удалось открыть файл \"" << filename << "\" для чтения\n";
        return false;
    }

    std::string line;
    
    while (std::getline(in, line))
    {
        if (!line.empty())
        {
            std::istringstream iss(line);
            std::string fieldName;
            std::string fieldConsumption;
            std::string fieldMileage;

            if (std::getline(iss, fieldName, ';') &&
                std::getline(iss, fieldConsumption, ';') &&
                std::getline(iss, fieldMileage, ';'))
            {
                Car c;
                bool parsed = true;

                try
                {
                    c.name = fieldName;
                    c.consumption = std::stod(fieldConsumption);
                    c.mileage = std::stoi(fieldMileage);
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Предупреждение: ошибка разбора строки \""
                              << line << "\": " << e.what() << '\n';
                    parsed = false;
                }

                if (parsed)
                {
                    sumConsumption += c.consumption;
                    ++count;

                    if (!hasMax || c.mileage > maxMileageCar.mileage)
                    {
                        maxMileageCar = c;
                        hasMax = true;
                    }
                }
            }
            else
            {
                std::cerr << "Предупреждение: пропущена некорректная строка: " << 
                              line << '\n';
            }
        }
    }
    if (count == 0)
    {
        std::cerr << "Файл не содержит корректных записей\n";
        return false;
    }

    std::cout << "\n=== Результаты загрузки парка ===\n";
    std::cout << "Всего записей: " << count << '\n';

    std::cout << "Автомобиль с максимальным пробегом: "
              << maxMileageCar.name
              << " (" << maxMileageCar.mileage << " км)\n";

    avg = static_cast<double>(sumConsumption) / count;
    std::cout << "Средний расход: "
              << std::fixed << std::setprecision(1) << avg
              << " л/100км\n";

    return true;
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "Russian.UTF-8");

    const int AMOUNT = 5;
    const Car cars[AMOUNT] = {
        {"Toyota Camry",  8.5, 120000},
        {"Ford Focus",    6.9,  85000},
        {"BMW X5",       12.3,  45000},
        {"Lada Vesta",    7.1,  60000},
        {"Mercedes C200", 9.4,  30000}
    };

    const std::string filename = "fleet.csv";

    if (saveFleet(filename, cars, AMOUNT))
    {
        std::cout << "Парк успешно сохранён в файл \"" << filename << "\"\n";
    }

    loadFleet(filename);

    return 0;
}
 
