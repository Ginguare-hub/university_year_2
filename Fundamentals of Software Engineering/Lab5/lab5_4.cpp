#include <algorithm>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Vehicle
{
public:
    Vehicle(std::string plate, double mileage) : plate_(std::move(plate)), mileage_(mileage)
    {
        if (plate_.empty())
        {
            throw std::invalid_argument("госномер не может быть пустым");
        }
        if (mileage < 0)
        {
            throw std::invalid_argument("пробег не может быть отрицательным");
        }
    }

    virtual ~Vehicle() = default;

    virtual double consumption() const = 0;
    virtual double maxLoadKg() const = 0;
    virtual std::string kind() const = 0;

    double tripFuel(double km) const
    {
        if (km < 0)
        {
            throw std::invalid_argument("расстояние не может быть отрицательным");
        }
        return km * consumption() / 100.0;
    }

    const std::string &plate() const { return plate_; }
    double mileage() const { return mileage_; }

protected:
    std::string plate_;
    double mileage_;
};

class PassengerCar : public Vehicle
{
public:
    PassengerCar(std::string plate, double mileage, int seats) : Vehicle(std::move(plate), mileage), seats_(seats)
    {
        if (seats <= 0)
        {
            throw std::invalid_argument("число мест должно быть > 0");
        }
    }

    double consumption() const override { return 8.0; }
    double maxLoadKg() const override { return seats_ * 80.0; }
    std::string kind() const override { return "Легковой"; }

    int seats() const { return seats_; }

private:
    int seats_;
};

class Truck : public Vehicle
{
public:
    Truck(std::string plate, double mileage, double capacityTons) : Vehicle(std::move(plate), mileage), capacityTons_(capacityTons)
    {
        if (capacityTons <= 0)
        {
            throw std::invalid_argument("грузоподъёмность должна быть > 0");
        }
    }

    double consumption() const override { return 20.0 + 2.0 * capacityTons_; }
    double maxLoadKg() const override { return capacityTons_ * 1000.0; }
    std::string kind() const override { return "Грузовик"; }

    double capacityTons() const { return capacityTons_; }

private:
    double capacityTons_;
};

class Bus : public Vehicle
{
public:
    Bus(std::string plate, double mileage, int seats) : Vehicle(std::move(plate), mileage), seats_(seats)
    {
        if (seats <= 0)
        {
            throw std::invalid_argument("число мест должно быть > 0");
        }
    }

    double consumption() const override { return 25.0; }
    double maxLoadKg() const override { return seats_ * 75.0; }
    std::string kind() const override { return "Автобус"; }

    int seats() const { return seats_; }

private:
    int seats_;
};

int main()
{
    setlocale(LC_ALL, "ru.UTF-8");
    std::vector<std::unique_ptr<Vehicle>> fleet;

    try
    {
        fleet.push_back(std::make_unique<PassengerCar>("А001АА", 12000.0, 5));
        fleet.push_back(std::make_unique<PassengerCar>("В002ВВ", 34000.0, 5));
        fleet.push_back(std::make_unique<Truck>("Е003ЕЕ", 88000.0, 10.0));
        fleet.push_back(std::make_unique<Bus>("К004КК", 50000.0, 25));
        fleet.push_back(std::make_unique<Bus>("М005ММ", 21000.0, 16));
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Ошибка создания: " << e.what() << "\n";
        return 1;
    }

    const double tripKm{250.0};
    double totalFuel{0.0};

    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::left << std::setw(14) << "Номер"
              << std::setw(18) << "Тип"
              << std::right << std::setw(24) << "Расход(л/100км)"
              << std::setw(24) << "Нагрузка(кг)"
              << std::setw(20) << "Топливо(л)" << "\n";
    std::cout << std::string(65, '-') << "\n";

    for (const auto &v : fleet)
    {
        double fuel{v->tripFuel(tripKm)};
        totalFuel += fuel;
        std::cout << std::left << std::setw(12) << v->plate()
                  << std::setw(12) << v->kind()
                  << std::right << std::setw(14) << v->consumption()
                  << std::setw(19) << v->maxLoadKg()
                  << std::setw(12) << fuel << "\n";
    }

    std::cout << std::string(65, '-') << "\n";
    std::cout << "Суммарный расход на " << static_cast<int>(tripKm)
              << " км: " << totalFuel << " л\n";

    auto heaviest = std::max_element(
        fleet.begin(), fleet.end(),
        [](const std::unique_ptr<Vehicle> &a, const std::unique_ptr<Vehicle> &b)
        {
            return a->maxLoadKg() < b->maxLoadKg();
        });
    std::cout << "Наибольшая нагрузка: " << (*heaviest)->plate()
              << " (" << (*heaviest)->kind() << ", "
              << (*heaviest)->maxLoadKg() << " кг)\n";

    int trucks{0};
    for (const auto &v : fleet)
    {
        if (dynamic_cast<const Truck *>(v.get()) != nullptr)
        {
            ++trucks;
        }
    }
    std::cout << "Грузовиков в парке: " << trucks << "\n";

    return 0;
}
