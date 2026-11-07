#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class CarError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class InvalidRefuel : public CarError
{
public:
    explicit InvalidRefuel(double liters) : CarError("объём заправки должен быть больше 0"), liters_(liters) {}

    double liters() const { return liters_; }

private:
    double liters_;
};

class Car
{
public:
    Car(std::string model, double tankCapacity, double consumption)
        : model_(std::move(model)),
        tankCapacity_(tankCapacity),
        fuel_(tankCapacity / 2.0),
        consumption_(consumption),
        mileage_(0.0)
    {
        if (tankCapacity_ <= 0.0)
        {
            throw std::invalid_argument("объём бака должен быть больше 0");
        }
        if (consumption_ <= 0.0)
        {
            throw std::invalid_argument("расход должен быть больше 0");
        }
        ++count_;
    }

    ~Car() { --count_; }

    bool drive(double km)
    {
        if (km <= 0.0)
        {
            return false;
        }
        const double needed = km * consumption_ / 100.0;
        if (needed > fuel_)
        {
            return false;
        }
        fuel_ -= needed;
        mileage_ += km;
        return true;
    }

    double refuel(double liters)
    {
        if (liters <= 0.0)
        {
            throw InvalidRefuel(liters);
        }
        const double freeSpace = tankCapacity_ - fuel_;
        if (liters >= freeSpace)
        {
            fuel_ = tankCapacity_;
            return freeSpace;
        }
        fuel_ += liters;
        return liters;
    }

    double range() const { return fuel_ / consumption_ * 100.0; }

    const std::string &model() const { return model_; }
    double tankCapacity() const { return tankCapacity_; }
    double fuel() const { return fuel_; }
    double consumption() const { return consumption_; }
    double mileage() const { return mileage_; }

    void print() const
    {
        std::cout << std::left << std::setw(14) << model_
                  << std::right << std::fixed << std::setprecision(1)
                  << std::setw(12) << tankCapacity_
                  << std::setw(20) << fuel_
                  << std::setw(22) << consumption_
                  << std::setw(20) << mileage_
                  << std::setw(21) << range() << '\n';
    }

    static int count() { return count_; }

private:
    std::string model_;
    double tankCapacity_;
    double fuel_;
    double consumption_;
    double mileage_;

    static inline int count_ = 0;
};

void printHeader()
{
    std::cout << std::left << std::setw(14) << "Модель"
              << std::right << std::setw(22) << "Бак, л"
              << std::setw(28) << "Топливо, л"
              << std::setw(29) << "Расход, л/100"
              << std::setw(28) << "Пробег, км"
              << std::setw(30) << "Запас хода" << '\n';
}

int main()
{
    setlocale(LC_ALL, "ru.utf-8");
    std::vector<Car> cars;
    cars.reserve(5);

    cars.emplace_back("Toyota Camry", 60.0, 8.5);
    cars.emplace_back("Ford Focus", 55.0, 6.9);
    cars.emplace_back("BMW X5", 80.0, 12.3);
    cars.emplace_back("Lada Vesta", 50.0, 7.1);
    cars.emplace_back("Mercedes C200", 66.0, 9.4);

    std::cout << "Поездки:\n";
    std::cout << "Toyota Camry, 120 км: "
              << (cars[0].drive(120.0) ? "выполнена" : "не хватает топлива") << '\n';
    std::cout << "Ford Focus, 400 км: "
              << (cars[1].drive(400.0) ? "выполнена" : "не хватает топлива") << '\n';
    std::cout << "BMW X5, 200 км: "
              << (cars[2].drive(200.0) ? "выполнена" : "не хватает топлива") << '\n';
    std::cout << "Lada Vesta, 150 км: "
              << (cars[3].drive(150.0) ? "выполнена" : "не хватает топлива") << '\n';
    std::cout << "Mercedes C200, 300 км: "
              << (cars[4].drive(300.0) ? "выполнена" : "не хватает топлива") << '\n';

    std::cout << "\nЗаправки:\n";
    std::cout << "Toyota Camry, 25 л: залито " << cars[0].refuel(25.0) << " л\n";
    std::cout << "Ford Focus, 100 л: залито " << cars[1].refuel(100.0) << " л\n";
    std::cout << "BMW X5, 10 л: залито " << cars[2].refuel(10.0) << " л\n";
    std::cout << "Lada Vesta, 50 л: залито " << cars[3].refuel(50.0) << " л\n";
    std::cout << "Mercedes C200, 20 л: залито " << cars[4].refuel(20.0) << " л\n";

    std::cout << "\nАвтопарк:\n";
    printHeader();
    for (const Car &car : cars)
    {
        car.print();
    }
    std::cout << "Всего автомобилей: " << Car::count() << '\n';

    std::cout << "\nОбработка ошибок:\n";
    const double fuelBefore = cars[0].fuel();
    try
    {
        cars[0].refuel(-5.0);
    }
    catch (const InvalidRefuel &e)
    {
        std::cerr << "InvalidRefuel: " << e.what() << '\n';
        std::cerr << "переданный объём: " << e.liters() << " л\n";
    }
    catch (const CarError &e)
    {
        std::cerr << "CarError: " << e.what() << '\n';
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "invalid_argument: " << e.what() << '\n';
    }
    catch (const std::exception &e)
    {
        std::cerr << "exception: " << e.what() << '\n';
    }
    std::cout << "Топливо Toyota до/после ошибки: "
              << fuelBefore << " / " << cars[0].fuel() << " л\n";

    try
    {
        Car broken("Неисправный", -10.0, 8.0);
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "invalid_argument: " << e.what() << '\n';
    }
    catch (const std::exception &e)
    {
        std::cerr << "exception: " << e.what() << '\n';
    }

    return 0;
}