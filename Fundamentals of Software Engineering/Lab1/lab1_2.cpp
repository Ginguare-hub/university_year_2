#include <iostream>

int main()
{
    setlocale(LC_ALL, "Russian.UTF-8");
    const double fuelConsumption[6]{7.2, 9.8, 12.5, 6.9, 15.0, 8.3};
    const int SIZE{6};

    constexpr double LIMIT{10.0};

    double sum{0.0};
    double min = fuelConsumption[0];
    double max = fuelConsumption[0];
    int countAboveLimit{0};

    double average{0.0};
    double newCarConsumption{0.0};

    bool isInputIncorrect{false};

    for (const auto &value : fuelConsumption)
    {
        sum += value;

        if (value > LIMIT)
            countAboveLimit++;

        if (value < min)
            min = value;

        if (value > max)
            max = value;
    }

    average = static_cast<double>(sum) / SIZE;

    std::cout << "Результаты анализа:\n";
    std::cout << "Средний расход: " << average << " л/100км\n";
    std::cout << "Количество машин с расходом выше " << LIMIT << " л/100км: " << countAboveLimit << "\n";
    std::cout << "Минимальный расход: " << min << " л/100км\n";
    std::cout << "Максимальный расход: " << max << " л/100км\n";

    do
    {
        if (isInputIncorrect == false)
        {
            isInputIncorrect = false;
            std::cout << "\nВведите расход топлива нового автомобиля (л/100км): \n";
            std::cin >> newCarConsumption;
        }
        if (isInputIncorrect == true)
        {
            isInputIncorrect = false;
            std::cin >> newCarConsumption;
        }

        if (std::cin.fail() && isInputIncorrect == false)
        {
            isInputIncorrect = true;
            std::cout << "Ошибка: введите корректные данные типа float :\n";
            std::cin.clear();
            while (std::cin.get() != '\n')
                ;
        }

        if (std::cin.get() != '\n' && isInputIncorrect == false)
        {
            isInputIncorrect = true;
            std::cout << "Ошибка: введите корректные данные типа float :\n";
            while (std::cin.get() != '\n')
                ;
        }

        if (newCarConsumption < 0 && isInputIncorrect == false)
        {
            isInputIncorrect = true;
            std::cout << "Ошибка: расход не может быть отрицательным :\n";
            while (std::cin.get() != '\n')
                ;
        }
    } while (isInputIncorrect);

    std::cout << "\nРасход нового автомобиля: " << newCarConsumption << " л/100км\n";
    if (newCarConsumption > LIMIT)
    {
        std::cout << "Расход превышает лимит " << LIMIT << " л/100км\n";
    }
    else
    {
        std::cout << "Расход в пределах лимита " << LIMIT << " л/100км\n";
    }

    return 0;
}
