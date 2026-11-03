#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

class ServiceHistory
{
public:
    ServiceHistory()
        : data_(new int[2]), size_(0), capacity_(2)
    {
        std::cout << "создан объект (вместимость 2)\n";
    }

    // 1 Деструктор.
    ~ServiceHistory()
    {
        delete[] data_;
        std::cout << "деструктор\n";
    }

    // 2 глубокая копия
    ServiceHistory(const ServiceHistory &other)
        : data_(other.capacity_ > 0 ? new int[other.capacity_] : nullptr),
          size_(other.size_),
          capacity_(other.capacity_)
    {
        if (size_ > 0)
        {
            std::copy(other.data_, other.data_ + other.size_, data_);
        }
        std::cout << "копирующий конструктор\n";
    }

    // 3 Копирующее присваивание
    ServiceHistory &operator=(const ServiceHistory &other)
    {
        std::cout << "копирующее присваивание\n";
        if (this != &other)
        {
            int *fresh = other.capacity_ > 0 ? new int[other.capacity_] : nullptr;
            if (other.size_ > 0)
            {
                std::copy(other.data_, other.data_ + other.size_, fresh);
            }
            delete[] data_;
            data_ = fresh;
            size_ = other.size_;
            capacity_ = other.capacity_;
        }
        return *this;
    }

    //4 Перемещающий конструктор (исходный объект пуст)
    ServiceHistory(ServiceHistory &&other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        std::cout << "перемещающий конструктор\n";
    }

    // 5 Перемещающее присваивание
    ServiceHistory &operator=(ServiceHistory &&other) noexcept
    {
        std::cout << "перемещающее присваивание\n";
        if (this != &other)
        {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    void add(const int &value)
    {
        if (size_ == capacity_)
        {
            grow();
        }
        data_[size_] = value;
        ++size_;
    }

    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_; }

    int &operator[](std::size_t index) { return data_[index]; }
    const int &operator[](std::size_t index) const { return data_[index]; }

    int lastService() const
    {
        if (size_ == 0)
        {
            throw std::out_of_range("история обслуживания пуста");
        }
        return data_[size_ - 1];
    }

private:
    void grow()
    {
        const std::size_t newCapacity = capacity_ == 0 ? 2 : capacity_ * 2;
        int *fresh = new int[newCapacity];
        if (size_ > 0)
        {
            std::copy(data_, data_ + size_, fresh);
        }
        delete[] data_;
        data_ = fresh;
        capacity_ = newCapacity;
    }

    int *data_;
    std::size_t size_;
    std::size_t capacity_;
};

ServiceHistory makeServiceHistory()
{
    ServiceHistory history;
    history.add(15000);
    history.add(30000);
    history.add(45000);
    return history;
}

int main()
{
    setlocale(LC_ALL, "ru.utf-8");
    ServiceHistory history;
    history.add(15000);
    history.add(30000);
    history.add(45000);
    std::cout << "исходная история: " << history.size()
              << " записей, последнее ТО: " << history.lastService() << " км\n";

    std::cout << "\n1) изменение копии не меняет оригинал\n";
    ServiceHistory copy = history;
    copy[0] = 99999;
    std::cout << "оригинал[0] = " << history[0]
              << ", копия[0] = " << copy[0] << '\n';

    std::cout << "\n2) присваивание объекта самому себе\n";
    ServiceHistory &alias = history;
    history = alias;
    std::cout << "размер после a = a: " << history.size() << '\n';

    std::cout << "\n3) возврат объекта из функции\n";
    ServiceHistory fromFunction = makeServiceHistory();
    std::cout << "размер: " << fromFunction.size()
              << ", последнее ТО: " << fromFunction.lastService() << " км\n";

    std::cout << "\n4) передача через std::move\n";
    ServiceHistory moved = std::move(history);
    std::cout << "moved.size() = " << moved.size()
              << ", history.size() = " << history.size()
              << ", history.capacity() = " << history.capacity() << '\n';

    ServiceHistory target;
    target = std::move(moved);
    std::cout << "после перемещающего присваивания: target.size() = " << target.size()
              << ", moved.size() = " << moved.size() << '\n';

    std::cout << "\n5) два объекта в std::vector\n";
    std::vector<ServiceHistory> histories;
    histories.push_back(makeServiceHistory());
    histories.push_back(makeServiceHistory());
    std::cout << "в векторе: " << histories.size() << " объекта\n";

    std::cout << "\n6) lastService() для пустой истории\n";
    ServiceHistory empty;
    try
    {
        empty.lastService();
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "out_of_range: " << e.what() << '\n';
    }

    return 0;
}
