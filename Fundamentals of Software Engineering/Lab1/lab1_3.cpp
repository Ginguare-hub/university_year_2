#include <iostream>
#include <format>

enum Car
{
    eco,
    average,
    inefficient
};

constexpr double NORMAL_CONS_BOUND{8.0};

constexpr double highConsumptionThreshold()
{
    constexpr double HIGH_CONS_BOUND{10.0};
    return HIGH_CONS_BOUND;
}

Car getCarConsumptionType(double cons)
{
    Car car_type{eco};

    if (cons > highConsumptionThreshold())
    {
        car_type = inefficient;
    }
    else
    {
        if (cons >= NORMAL_CONS_BOUND)
        {
            car_type = average;
        }
    }

    return car_type;
}

void applyMaintenancePenalty(double &consumption, double penalty = 1.2)
{
    consumption *= penalty;
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

double readPenalty(const double MIN_NUMBER, const double MAX_NUMBER, const std::string myString)
{
    bool isIncorrect{false};
    double number{1.0};

    do
    {
        std::cout << myString;
        isIncorrect = false;

        if (std::cin.peek() == '\n')
        {
            std::cin.get();
            return 0.0;
        }

        std::cin >> number;

        if (std::cin.fail())
        {
            isIncorrect = true;
            std::cin.clear();
            while (std::cin.get() != '\n' && !std::cin.eof())
            {
            }
            std::cerr << "Ошибка: некорректный ввод\n";
        }

        if (!isIncorrect && std::cin.get() != '\n')
        {
            isIncorrect = true;
            while (std::cin.get() != '\n' && !std::cin.eof())
            {
            }
            std::cerr << "Ошибка: некорректный ввод\n";
        }

        if (!isIncorrect && (number < MIN_NUMBER || number > MAX_NUMBER))
        {
            isIncorrect = true;
            std::cerr << "Ошибка: число должно входить в диапазон [" << MIN_NUMBER << ".." << MAX_NUMBER << "]\n";
        }

    } while (isIncorrect);

    return number;
}

void countCarTypes(const double cars_consumption[], const int length,
                   int &countEco, int &countAverage, int &countInefficient)
{
    countEco = 0;
    countAverage = 0;
    countInefficient = 0;

    for (int i = 0; i < length; ++i)
    {
        Car car_type{eco};

        car_type = getCarConsumptionType(cars_consumption[i]);
        switch (car_type)
        {
        case eco:
            countEco++;
            break;

        case average:
            countAverage++;
            break;

        case inefficient:
            countInefficient++;
            break;
        }
    }
}

void writeCarSpecifications(const double cons)
{
    Car car_type{eco};
    std::string type{""};

    car_type = getCarConsumptionType(cons);

    switch (car_type)
    {
    case eco:
        type = "Экономичная";
        break;

    case average:
        type = "Средняя";
        break;

    case inefficient:
        type = "Прожорливая";
        break;
    }

    std::cout << "    Потребление: " << cons << " (л/100км)\n";
    std::cout << "    Тип потребления: " << type << "\n";
}

void writeCarTypeCount(const double cars[], const int length)
{
    int countEco{0}, countAverage{0}, countInefficient{0};

    countCarTypes(cars, length, countEco, countAverage, countInefficient);
    std::cout << "Количество машин каждой категории:\n";
    std::cout << "    Экономичных: " << countEco << "\n";
    std::cout << "    Средних: " << countAverage << "\n";
    std::cout << "    Прожорливых: " << countInefficient << "\n";
}

int main()
{
    constexpr int CARS_COUNT{6};
    constexpr double MAX_PENALTY{10.0};
    double cars_consumption[CARS_COUNT]{7.2, 9.8, 12.5, 6.9, 15.0, 8.3};
    bool shouldRepeat{true};
    int index{-1};
    Car car_type{eco};

    setlocale(LC_ALL, "Russian.UTF-8");

    writeCarTypeCount(cars_consumption, CARS_COUNT);

    do
    {
        std::cout << "\nВведите номер машины [1..6] для вывода характеристик и наложения штрафа для прожорливых машин (0 для выхода):\n";
        index = readInt(0, CARS_COUNT, "> ");
        index--;

        if (index == -1)
        {
            std::cout << "====== Выход ======\n";
            shouldRepeat = false;
        }
        else
        {
            car_type = getCarConsumptionType(cars_consumption[index]);
            switch (car_type)
            {
            case inefficient:
            {
                double new_cons{cars_consumption[index]};
                std::cout << "Введите штраф-множитель для увеличения потребления машины (Enter для стандартного 1.2):\n";
                double penalty{readPenalty(1.0, MAX_PENALTY, "> ")};

                if (penalty == 0.0)
                {
                    applyMaintenancePenalty(new_cons);
                }
                else
                {
                    applyMaintenancePenalty(new_cons, penalty);
                }
                std::cout << "\n=== До применения штрафа ===\n";
                writeCarSpecifications(cars_consumption[index]);
                std::cout << "=== После применения штрафа ===\n";
                writeCarSpecifications(new_cons);
                break;
            }

            default:
                std::cout << "Характеристики машины №" << index + 1 << "\n";
                writeCarSpecifications(cars_consumption[index]);
                break;
            }
        }

    } while (shouldRepeat);

    return 0;
}