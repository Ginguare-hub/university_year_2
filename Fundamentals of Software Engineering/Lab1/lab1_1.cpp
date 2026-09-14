#include <iostream>
#include <clocale>

double fuelCost(double liters, double pricePerLiter = 58.5)
{
    return liters * pricePerLiter;
}

int fuelCost(int liters, int pricePerLiter)
{
    return liters * pricePerLiter;
}

void readDouble(const char *prompt, double &value)
{
    bool isInputIncorrect{true};

    do
    {
        isInputIncorrect = false;
        std::cout << prompt << '\n';
        std::cin >> value;

        if (std::cin.fail())
        {
            isInputIncorrect = true;
            std::cerr << "Ошибка: введите корректные данные типа double\n";
            std::cin.clear();
            while (std::cin.get() != '\n')
                ;
        }
        else if (std::cin.get() != '\n')
        {
            isInputIncorrect = true;
            std::cerr << "Ошибка: в строке лишние символы\n";
            while (std::cin.get() != '\n')
                ;
        }
        else if (value < 0)
        {
            isInputIncorrect = true;
            std::cerr << "Ошибка: значение не может быть отрицательным\n";
            while (std::cin.get() != '\n')
                ;
        }
    } while (isInputIncorrect);
}

void readInt(const char *prompt, int &value)
{
    bool isInputIncorrect{true};

    do
    {
        isInputIncorrect = false;
        std::cout << prompt << '\n';
        std::cin >> value;

        if (std::cin.fail())
        {
            isInputIncorrect = true;
            std::cerr << "Ошибка: введите корректные данные типа int\n";
            std::cin.clear();
            while (std::cin.get() != '\n')
                ;
        }
        else if (std::cin.get() != '\n')
        {
            isInputIncorrect = true;
            std::cerr << "Ошибка: в строке лишние символы\n";
            while (std::cin.get() != '\n')
                ;
        }
        else if (value < 0)
        {
            isInputIncorrect = true;
            std::cerr << "Ошибка: значение не может быть отрицательным\n";
            while (std::cin.get() != '\n')
                ;
        }
    } while (isInputIncorrect);
}

int main()
{
    setlocale(LC_ALL, "Russian.UTF-8");

    double liters{0.0};
    int litersInt{0};
    int priceInt{0};

    readDouble("Введите объём топлива (л):", liters);
    std::cout << "Расчёт fuelCost(double) с ценой по умолчанию 58.5: " << fuelCost(liters) << " руб.\n";

    readInt("Введите объём топлива (целое, л):", litersInt);
    readInt("Введите цену за литр (целое):", priceInt);
    std::cout << "Расчёт fuelCost(int, int): " << fuelCost(litersInt, priceInt) << " руб.\n";

    return 0;
}
