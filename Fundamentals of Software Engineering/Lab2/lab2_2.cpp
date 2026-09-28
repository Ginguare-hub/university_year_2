#include <iostream>
#include <string>
#include <iomanip>
#include <windows.h>

typedef struct car
{
    std::string name;
    double consumption;
    int mileage;
} car;

const int AMOUNT{5};
const car cars[AMOUNT]{"Toyota Camry", 8.5, 120000,
                       "Ford Focus", 6.9, 85000,
                       "BMW X5", 12.3, 45000,
                       "Lada Vesta", 7.1, 60000,
                       "Mercedes C200", 9.4, 30000};

void printTable(const car cars[], int amount);
void printAboveLimit(const car cars[], int amount);

int main(void)
{
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "Russian.UTF-8");
    printTable(cars, AMOUNT);
    printAboveLimit(cars, AMOUNT);

    return 0;
}

void printTable(const car cars[], int amount)
{
    std::cout << "┌──────┬────────────────────┬──────────┬───────────┐\n";
    std::cout << "│ " << std::right << std::setw(4) << "N/o"
              << " │ " << std::left << std::setw(18) << "Mark"
              << " │ " << std::right << std::setw(8) << "Intake"
              << " │ " << std::right << std::setw(9) << "Mileage"
              << " │\n";
    std::cout << "├──────┼────────────────────┼──────────┼───────────┤\n";

    for (int i{0}; i < amount; i++)
    {
        std::cout << "│ " << std::right << std::setw(4) << (i + 1)
                  << " │ " << std::left << std::setw(18) << cars[i].name
                  << " │ " << std::right << std::setw(8) << std::fixed << std::setprecision(1) << cars[i].consumption
                  << " │ " << std::right << std::setw(9) << cars[i].mileage
                  << " │\n";
    }

    std::cout << "└──────┴────────────────────┴──────────┴───────────┘\n";
}

void printAboveLimit(const car cars[], int amount)
{
    double limit{0};
    std::cout << "\nЗадайте предел расхода: ";
    std::cin >> limit;

    if (std::cin.fail() || limit <= 0)
    {
        std::cerr << "Ошибка: предел должен быть числом больше 0\n";
        return;
    }

    car found[AMOUNT];
    int foundCount = 0;

    for (int i{0}; i < amount; i++)
    {
        if (cars[i].consumption > limit)
        {
            found[foundCount] = cars[i];
            foundCount = foundCount + 1;
        }
    }

    if (foundCount == 0)
    {
        std::cout << "\nНет автомобилей, превышающих предел.\n";
        return;
    }

    std::cout << "\nАвтомобили с расходом выше "
              << std::fixed << std::setprecision(1) << limit << ":\n";
    printTable(found, foundCount);
}