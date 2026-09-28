#include <iostream>
#include <string>

void writeCarsBySubstring(std::string sub);
void findLexicographically(std::string &lowest, std::string &highest);
std::string findWithMostSize();

void writeSizeTask();
void writeLexicographicallyTask();
void writeSubststringTask();

const int AMOUNT{5};
const std::string cars[AMOUNT]{"Toyota Camry", "Ford Focus", "BMW X5", "Lada Vesta", "Mercedes C200"};

int main(void)
{
    setlocale(LC_ALL, "ru.UTF-8");
    writeSubststringTask();
    writeLexicographicallyTask();
    writeSizeTask();

    return 0;
}

void writeSubststringTask()
{
    std::string sub;
    std::cout << "Введите подстроку: ";
    std::getline(std::cin, sub);

    std::cout << "Все марки машин с подстрокой \"" << sub << "\":\n";
    writeCarsBySubstring(sub);
}

void writeLexicographicallyTask()
{
    std::string lowest, highest;

    std::cout << "\nНайденные лексикографически наименьшая и наибольшая марки:\n";
    findLexicographically(lowest, highest);
    std::cout << "Наименьшая - " << lowest << "\n"
              << "Наибольшая - " << highest << "\n";

}

void writeSizeTask()
{
    std::string mostSize;

    std::cout << "\nМарка с наибольшим числом символов:\n";

    mostSize = findWithMostSize();
    std::cout << mostSize << "\n";
}

void writeCarsBySubstring(std::string sub)
{
    bool isExist{false};

    for (int i{0}; i < AMOUNT; ++i)
    {
        if (cars[i].find(sub) != std::string::npos)
        {
            std::cout << "  " << cars[i] << "\n";
            isExist = true;
        }
    }
    
    if (!isExist)
    {
        std::cout << "С данной подстрокой не найдено марок машин\n";
    }
}

void findLexicographically(std::string &lowest, std::string &highest)
{
    lowest = cars[0];
    highest = cars[0];

    for (int i{1}; i < AMOUNT; ++i)
    {
        if (cars[i] > highest)
        {
            highest = cars[i];
        }
        if (cars[i] < lowest)
        {
            lowest = cars[i];
        }
    }
}

std::string findWithMostSize()
{
    std::string most{cars[0]};
    
    for (int i{1}; i < AMOUNT; ++i)
    {
        if (size(cars[i]) > size(most))
        {
            most = cars[i];
        }
    }

    return most;
}