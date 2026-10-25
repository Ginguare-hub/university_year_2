#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <algorithm>

int readInt(const int MIN_NUMBER, const int MAX_NUMBER, std::string myString);

int main()
{
    setlocale(LC_ALL, "ru.utf-8");

    std::array<std::string, 5> arr = {"Toyota Camry", 
                                      "Ford Focus", 
                                      "BMW X5",
                                      "Lada Vesta", 
                                      "Mercedes C200"};

    std::vector<std::string> cars(arr.begin(), arr.end());

    std::string newCar;
    std::cout << "Введите марку машины: ";
    std::getline(std::cin, newCar);
    cars.push_back(newCar);
    
    int it{1};
    std::cout << "\nВведите номер машины для удаление из контейнера: \n";
    std::for_each(cars.begin(), cars.end(), [&it](const std::string car)
                  { std::cout << it++ << ". " << car << "\n"; });

    cars.erase(cars.begin() + readInt(1, cars.size(), "> ") - 1);
    std::cout << "\nТекущее наполнение контейнера: \n";
    it = 1;
    std::for_each(cars.begin(), cars.end(), [&it](const std::string car) { std::cout << it++ << ". " << car << "\n"; });

    auto first_with_number = std::find_if(cars.begin(), cars.end(), [](std::string &s) { return std::any_of(s.begin(), s.end(), [](unsigned char c) { return std::isdigit(c); }); });
    std::cout << "\nПервая марка с числом в названии: " << (*first_with_number) << "\n"; 

    constexpr int MAX_LEN = 10;
    int cars_with_long_names = std::count_if(cars.begin(), cars.end(), [MAX_LEN](const std::string &s) { return s.length() > MAX_LEN; });
    std::cout << "Кол-во марок, с названием, которое содержит более " << MAX_LEN << " символов: " << cars_with_long_names;

    return 0;
}

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
            std::cerr << "Ошибка: число должно входить в диапазон [" << MIN_NUMBER << ".." << MAX_NUMBER << "]\n";
        }

    } while (isIncorrect);

    return number;
}