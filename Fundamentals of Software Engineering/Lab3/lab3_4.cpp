#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <string>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <locale>

struct Car
{
    std::string brand;
    double consumption;
    int mileage;
};

int main()
{
    setlocale(LC_ALL, "ru.UTF-8");
    std::vector<Car> cars = {
        {"Toyota Camry", 8.5, 120000},
        {"Ford Focus", 6.9, 85000},
        {"BMW X5", 12.3, 45000},
        {"Lada Vesta", 7.1, 60000},
        {"Mercedes C200", 9.4, 30000}};

    std::stack<std::string> undoStack;
    undoStack.push("Toyota Camry");
    undoStack.push("Ford Focus");
    undoStack.push("BMW X5");

    std::cout << "Последнее назначение: " << undoStack.top() << "\n";
    std::cout << "Undo: снято \"" << undoStack.top() << "\"\n";
    undoStack.pop();
    std::cout << "Теперь верх стека: " << undoStack.top() << "\n\n";

    std::queue<std::string> serviceQueue;
    serviceQueue.push("Lada Vesta");
    serviceQueue.push("Mercedes C200");
    serviceQueue.push("Toyota Camry");

    std::cout << "Первый в очереди на ТО: " << serviceQueue.front() << "\n";
    std::cout << "Отправлен на ТО: " << serviceQueue.front() << "\n";
    serviceQueue.pop();
    std::cout << "Следующий на ТО: " << serviceQueue.front() << "\n\n";

    double pricePerLiter;
    std::cout << "Введите цену за литр топлива: ";
    std::cin >> pricePerLiter;

    std::vector<double> fuelCost(cars.size());
    std::transform(cars.begin(), cars.end(), fuelCost.begin(),
                   [pricePerLiter](const Car &c)
                   {
                       return c.consumption * pricePerLiter;
                   });

    std::cout << std::setprecision(2) << std::fixed;
    std::cout << "\nСтоимость топлива на 100 км:\n";
    for (size_t i = 0; i < cars.size(); ++i)
    {
        std::cout << "  " << std::setw(16) << cars[i].brand
                  << " -> " << fuelCost[i] << " руб.\n";
    }

    int totalMileage = std::accumulate(
        cars.begin(), cars.end(), 0,
        [](int sum, const Car &c)
        { return sum + c.mileage; });

    std::cout << "\nСуммарный пробег парка: " << totalMileage << " км\n";

    std::vector<Car>::iterator mostThirsty = std::max_element(
        cars.begin(), cars.end(),
        [](const Car &a, const Car &b)
        {
            return a.consumption < b.consumption;
        });

    std::vector<Car>::iterator mostEconomical = std::min_element(
        cars.begin(), cars.end(),
        [](const Car &a, const Car &b)
        {
            return a.consumption < b.consumption;
        });

    std::cout << std::setprecision(1) << std::fixed;
    std::cout << "Самый прожорливый: " << mostThirsty->brand
              << " (" << mostThirsty->consumption << " л/100км)\n";
    std::cout << "Самый экономичный: " << mostEconomical->brand
              << " (" << mostEconomical->consumption << " л/100км)\n";

    return 0;
}