#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class Fuel
{
public:
    Fuel() = default;

    explicit Fuel(long long ml) : ml_(ml)
    {
        if (ml < 0)
        {
            throw std::invalid_argument("объём топлива не может быть отрицательным");
        }
    }

    static Fuel fromLiters(double liters)
    {
        if (liters < 0)
        {
            throw std::invalid_argument("объём топлива не может быть отрицательным");
        }
        return Fuel(static_cast<long long>(liters * 1000.0 + 0.5));
    }

    long long milliliters() const { return ml_; }

    Fuel &operator+=(const Fuel &other)
    {
        ml_ += other.ml_;
        return *this;
    }

    Fuel &operator-=(const Fuel &other)
    {
        if (other.ml_ > ml_)
        {
            throw std::invalid_argument("результат вычитания отрицателен");
        }
        ml_ -= other.ml_;
        return *this;
    }

    friend Fuel operator+(Fuel a, const Fuel &b)
    {
        a += b;
        return a;
    }
    friend Fuel operator-(Fuel a, const Fuel &b)
    {
        a -= b;
        return a;
    }

    friend Fuel operator*(Fuel a, double k)
    {
        if (k < 0)
        {
            throw std::invalid_argument("множитель не может быть отрицательным");
        }
        a.ml_ = static_cast<long long>(a.ml_ * k + 0.5);
        return a;
    }
    friend Fuel operator*(double k, const Fuel &a) { return a * k; }

    bool operator==(const Fuel &other) const { return ml_ == other.ml_; }
    bool operator!=(const Fuel &other) const { return ml_ != other.ml_; }
    bool operator<(const Fuel &other) const { return ml_ < other.ml_; }
    bool operator>(const Fuel &other) const { return ml_ > other.ml_; }
    bool operator<=(const Fuel &other) const { return ml_ <= other.ml_; }
    bool operator>=(const Fuel &other) const { return ml_ >= other.ml_; }

    double cost(double pricePerLiter) const
    {
        if (pricePerLiter < 0)
        {
            throw std::invalid_argument("цена не может быть отрицательной");
        }
        return (ml_ / 1000.0) * pricePerLiter;
    }

    friend std::ostream &operator<<(std::ostream &os, const Fuel &f)
    {
        long long whole{f.ml_ / 1000};
        long long frac{f.ml_ % 1000};
        os << whole << '.' << std::setw(3) << std::setfill('0') << frac << std::setfill(' ') << " л";
        return os;
    }

    friend std::istream &operator>>(std::istream &is, Fuel &f)
    {
        std::string token;
        if (!(is >> token))
        {
            return is;
        }

        std::size_t end{token.size()};
        if (end > 0 && (token[end - 1] == 'l' || token[end - 1] == 'L'))
        {
            --end;
        }

        bool hasDot{false};
        bool ok{end > 0};
        for (std::size_t i{0}; i < end && ok; ++i)
        {
            char c = token[i];
            if (c == '.')
            {
                if (hasDot)
                {
                    ok = false;
                }
                hasDot = true;
            }
            else if (c < '0' || c > '9')
            {
                ok = false;
            }
        }
        if (!ok)
        {
            is.setstate(std::ios::failbit);
            return is;
        }

        long long whole{0};
        long long frac{0};
        long long fracDigits{0};
        std::size_t i{0};
        while (i < end && token[i] != '.')
        {
            whole = whole * 10 + (token[i] - '0');
            ++i;
        }
        if (i < end && token[i] == '.')
        {
            ++i;
            while (i < end)
            {
                frac = frac * 10 + (token[i] - '0');
                ++fracDigits;
                ++i;
            }
        }
        while (fracDigits < 3)
        {
            frac *= 10;
            ++fracDigits;
        }
        while (fracDigits > 3)
        {
            frac /= 10;
            --fracDigits;
        }

        f.ml_ = whole * 1000 + frac;
        return is;
    }

private:
    long long ml_ = 0;
};

struct Trip
{
    std::string car;
    double km;
    double consumption;
};

int main()
{
    setlocale(LC_ALL, "ru.UTF-8");
    const double pricePerLiter{56.40};

    const std::vector<Trip> trips = {
        {"Toyota Camry", 320.0, 8.5},
        {"Lada Vesta", 150.0, 7.1},
        {"BMW X5", 540.0, 12.3},
        {"Mercedes C200", 260.0, 9.4},
        {"Ford Focus", 210.0, 6.9},
    };

    std::cout << std::fixed << std::setprecision(2);

    Fuel total;
    Fuel maxFuel;
    std::string maxCar;
    bool first = true;

    std::cout << std::left << std::setw(16) << "Автомобиль"
              << std::right << std::setw(17) << "Км"
              << std::setw(29) << "Расход(л)"
              << std::setw(33) << "Стоимость(Руб)" << "\n";
    std::cout << std::string(70, '-') << "\n";

    for (const auto &t : trips)
    {
        double liters{t.km * t.consumption / 100.0};
        Fuel need{Fuel::fromLiters(liters)};

        total += need;
        if (first || need > maxFuel)
        {
            maxFuel = need;
            maxCar = t.car;
            first = false;
        }

        std::cout << std::left << std::setw(16) << t.car
                  << std::right << std::setw(10) << static_cast<int>(t.km)
                  << std::setw(14) << need
                  << std::setw(16) << need.cost(pricePerLiter) << "\n";
    }

    std::cout << std::string(70, '-') << "\n";
    std::cout << "Суммарный объём: " << total << "\n";
    std::cout << "Суммарная стоимость: " << total.cost(pricePerLiter) << " ₽\n";
    std::cout << "Самый затратный рейс: " << maxCar << " (" << maxFuel << ")\n";

    std::istringstream in("45.5l 12,5l");
    Fuel f1, f2;
    if (in >> f1)
    {
        std::cout << "Прочитано: " << f1 << "\n";
    }
    else
    {
        std::cerr << "Ошибка чтения первой записи\n";
    }
    if (in >> f2)
    {
        std::cout << "Прочитано: " << f2 << "\n";
    }
    else
    {
        std::cerr << "Ошибка чтения второй записи\n";
    }

    return 0;
}
