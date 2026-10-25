#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <iterator>

constexpr int MAX_MILEAGE = 10000000;

struct Car
{
    std::string brand;
    int mileage;
};

int readInt(const int MIN_NUMBER, const int MAX_NUMBER, std::string myString)
{
    bool isIncorrect{false};
    int number{0};

    do
    {
        std::cout << myString;
        isIncorrect = false;

        std::cin >> number;

        if (std::cin.fail() || std::cin.get() != '\n')
        {
            isIncorrect = true;
            std::cerr << "Ошибка: некорректный ввод\n";
            std::cin.clear();
            while (std::cin.get() != '\n')
            {
            };
        }

        if (!isIncorrect && ((number < MIN_NUMBER) || (number > MAX_NUMBER)))
        {
            isIncorrect = true;
            std::cerr << "Ошибка: число должно входить в диапазон ["
                      << MIN_NUMBER << ".." << MAX_NUMBER << "]\n";
        }

    } while (isIncorrect);

    return number;
}

void printCar(const Car &c)
{
    std::cout << c.brand << " — " << c.mileage << " км\n";
}

void printFleet(const std::list<Car> &fleet)
{
    if (fleet.empty())
    {
        std::cout << "(список пуст)\n";
        return;
    }
    int i = 1;
    for (const Car &c : fleet)
    {
        std::cout << i++ << ") " << c.brand << " — " << c.mileage << " км\n";
    }
}

void insertBeforeBrand(std::list<Car> &fleet)
{
    if (fleet.empty())
    {
        std::cerr << "Список пуст\n";
        return;
    }
    std::string target;
    std::cout << "Перед какой маркой вставить?\n";
    int car = readInt(1, fleet.size(), "> ");
    car--;
    auto it = fleet.begin();
    std::advance(it, car);

    Car newCar;
    std::cout << "Марка новой машины: ";
    std::getline(std::cin, newCar.brand);
    newCar.mileage = readInt(0, MAX_MILEAGE, "Пробег новой машины: ");

    fleet.insert(it, newCar);
    std::cout << "Вставлено перед " << target << '\n';
}

void removeByIndex(std::list<Car> &fleet)
{
    if (fleet.empty())
    {
        std::cerr << "Список пуст\n";
        return;
    }

    std::cout << "Введите номер машины для удаления: \n";
    int index = readInt(1, fleet.size(), "> ");

    auto it = std::next(fleet.begin(), index - 1);

    std::cout << "Удалена машина: " << it->brand << " — " << it->mileage << " км\n";

    fleet.erase(it);
    std::cout << "Удалено\n";
}

void showHighMileage(const std::list<Car> &fleet)
{
    std::vector<Car> big;
    std::copy_if(fleet.begin(), fleet.end(), std::back_inserter(big),
                 [](const Car &c)
                 { return c.mileage > 50000; });

    if (big.empty())
    {
        std::cout << "Нет автомобилей с пробегом > 50000\n";
        return;
    }
    std::cout << "Автомобили с пробегом > 50000:\n";
    for (const Car &c : big)
        printCar(c);
    std::cout << "Их количество: " << big.size() << '\n';
}

int main()
{
    setlocale(LC_ALL, "ru.utf-8");

    std::list<Car> fleet;

    std::cout << "Введите кол-во автомобилей\n";
    int n = readInt(0, 100, "> ");
    for (int i = 0; i < n; ++i)
    {
        Car c;
        std::cout << "Марка #" << (i + 1) << ": ";
        std::getline(std::cin, c.brand);
        c.mileage = readInt(0, MAX_MILEAGE, "Пробег: ");
        fleet.push_back(c);
    }

    int choice;
    do
    {
        std::cout << "\n=== Меню ===\n";
        std::cout << "1. Показать автопарк\n";
        std::cout << "2. Вставить перед заданной маркой\n";
        std::cout << "3. Удалить по значению\n";
        std::cout << "4. Автомобили с пробегом > 50000\n";
        std::cout << "0. Выход\n";

        choice = readInt(0, 4, "> ");

        switch (choice)
        {
        case 1:
            printFleet(fleet);
            break;
        case 2:
            insertBeforeBrand(fleet);
            break;
        case 3:
            removeByIndex(fleet);
            break;
        case 4:
            showHighMileage(fleet);
            break;
        case 0:
            std::cout << "Выход\n";
            break;
        }
    } while (choice != 0);

    return 0;
}