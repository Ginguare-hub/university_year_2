#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include "value.hpp"
#include "errors.hpp"
#include "model.hpp"
#include "storage.hpp"
#include "repository.hpp"
#include "operations.hpp"
#include "analytics.hpp"
#include "utils.hpp"

namespace
{
    void clearLine()
    {
        std::cout << std::string(60, '-') << '\n';
    }

    void printHeader(const std::string& title)
    {
        std::cout << "\n=== " << title << " ===\n";
    }

    std::string ask(const std::string& prompt)
    {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        return s;
    }

    int askInt(const std::string& prompt, int def)
    {
        std::string s = ask(prompt);
        if (s.empty()) return def;
        try { return std::stoi(s); }
        catch (...) { return def; }
    }

    double askDouble(const std::string& prompt, double def)
    {
        std::string s = ask(prompt);
        if (s.empty()) return def;
        try { return std::stod(s); }
        catch (...) { return def; }
    }

    void pause()
    {
        std::cout << "\nНажмите Enter для продолжения...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }
}

int main()
{
    setlocale(LC_ALL, "ru.UTF-8");
    try
    {
        Repository repo;

        auto rows {readFleet("data/fleet.txt")};
        for (const auto& r : rows)
        {
            try
            {
                auto v {makeVehicle(r.type, r.plate, r.model, r.tank, r.fuel, r.mileage, r.mileage, r.param)};
                repo.add(std::move(v));
            }
            catch (const std::exception& e)
            {
                logError("data/fleet.txt", r.line, e.what());
            }
        }

        Operations ops(repo);
        auto opRows {readOperations("data/operations.txt")};
        for (const auto& o : opRows)
        {
            Operation op;
            op.cmd = o.cmd; op.plate = o.plate; op.a = o.a; op.b = o.b; op.line = o.line;
            ops.enqueue(op);
        }

        auto doList = [&]
        {
            printHeader("Парк транспортных средств");
            if (repo.index().empty())
            {
                std::cout << "Парк пуст.\n";
                return;
            }

            std::cout << std::left
                      << std::setw(24) << "Госномер"
                      << std::setw(11)  << "Тип"
                      << std::setw(30) << "Модель"
                      << std::setw(18) << "Топливо"
                      << std::setw(12) << "Пробег" << '\n';
            std::cout << std::string(72, '-') << '\n';

            for (const auto& p : repo.index())
            {
                std::cout << std::left
                          << std::setw(16) << p.first
                          << std::setw(8)  << p.second->kind()
                          << std::setw(20) << p.second->model()
                          << std::setw(10) << p.second->fuel()
                          << std::setw(14) << p.second->mileage() << '\n';
            }
            std::cout << std::string(72, '-') << '\n';
            std::cout << "Всего: " << repo.size() << " ТС\n";
        };

        auto doFind = [&]
        {
            printHeader("Поиск по госномеру");
            std::string plate = ask("Госномер: ");
            Vehicle* v = repo.find(plate);
            std::cout << "Найдено: " << v->plate() << " | " << v->kind()
                      << " | " << v->model()
                      << " | топливо=" << v->fuel()
                      << " | пробег=" << v->mileage() << '\n';
        };

        auto doRange = [&]
        {
            printHeader("Запас хода (по убыванию)");
            auto v {vehiclesByRangeDesc(repo)};
            if (v.empty()) { std::cout << "Парк пуст.\n"; }
            for (Vehicle* x : v)
                std::cout << std::left << std::setw(12) << x->plate()
                          << "запас хода = " << x->range() << " км\n";
        };

        auto doFuel = [&]
        {
            printHeader("Расход топлива");
            double price = askDouble("Цена за литр (по умолчанию 56.40): ", 56.40);
            auto s {totalConsumption(repo, price)};
            std::cout << "Расход = " << s.liters << " л\n"
                      << "Стоимость = " << s.cost << " руб\n";
        };

        auto doLoad = [&]
        {
            printHeader("Максимальная нагрузка");
            Vehicle* v {maxLoadVehicle(repo)};
            if (v) std::cout << "Макс. нагрузка: " << v->plate() << " — " << v->maxLoadKg() << " кг\n";
            else std::cout << "Парк пуст.\n";
        };

        auto doService = [&]
        {
            printHeader("Требуют ТО");
            auto v {needService(repo)};
            if (v.empty()) { std::cout << "Все в порядке, ТО не требуется.\n"; }
            for (Vehicle* x : v)
                std::cout << x->plate() << " — после ТО " << (x->mileage() - x->lastService()) << " км\n";
        };

        auto doTrip = [&]
        {
            printHeader("Проверка поездки");
            double dist = askDouble("Дистанция (км, по умолчанию 250): ", 250.0);
            bool ok {allCanTrip(repo, dist)};
            std::cout << (ok ? "Все доедут.\n" : "Не все доедут:\n");
            auto bad {whoCannotTrip(repo, dist)};
            for (Vehicle* x : bad)
                std::cout << "  не доедет: " << x->plate() << '\n';
        };

        auto doMonth = [&]
        {
            printHeader("Самый загруженный месяц");
            std::cout << "Месяц: " << busiestMonth(repo) << '\n';
        };

        auto doTop = [&]
        {
            printHeader("Топ по расходу");
            int n = askInt("Сколько показать (по умолчанию 3): ", 3);
            auto v {topByConsumption(repo, n)};
            if (v.empty()) { std::cout << "Парк пуст.\n"; }
            for (Vehicle* x : v)
                std::cout << x->plate() << " — расход " << x->consumption() << " л/100км\n";
        };

        auto doRun = [&]
        {
            printHeader("Выполнение операций");
            int n {ops.runAll()};
            std::cout << "Выполнено операций: " << n << '\n';
        };

        auto doWait = [&]
        {
            printHeader("Лист ожидания");
            auto& wl = ops.waitList();
            if (wl.empty()) { std::cout << "Пусто.\n"; }
            for (const auto& o : wl)
                std::cout << "  " << o.cmd << " " << o.plate << '\n';
        };

        auto doUndo = [&]
        {
            printHeader("Отмена последней операции");
            ops.undo();
            std::cout << "Готово.\n";
        };

        auto doHistory = [&]
        {
            printHeader("История операций");
            auto& h = ops.history();
            if (h.empty()) { std::cout << "Пусто.\n"; }
            for (const auto& s : h) std::cout << "  " << s << '\n';
        };

        auto doReport = [&]
        {
            printHeader("Формирование отчёта");
            std::ofstream out("output/report.txt");
            if (!out.is_open())
            {
                std::cerr << "Не удалось открыть output/report.txt\n";
            }
            else
            {
                out << "ОТЧЁТ\n\nПарк:\n";
                for (const auto& p : repo.index())
                    out << p.first << ";" << p.second->kind() << ";"
                        << p.second->model() << ";" << p.second->fuel() << ";"
                        << p.second->mileage() << '\n';
                out << "\nРасход по типам:\n";
                for (const auto& t : repo.byType())
                    out << t.first << ";" << t.second << '\n';
                out << "\nГрафик ТО:\n";
                for (const auto& h : repo.history())
                    for (int m : h.second)
                        out << h.first << ";" << m << '\n';
                out << "\nИстория операций:\n";
                for (const auto& h : ops.history())
                    out << h << '\n';
                auto s {totalConsumption(repo, 56.40)};
                out << "\nСуммарный расход=" << s.liters << " л, стоимость=" << s.cost << " руб\n";
                out << "Самый загруженный месяц: " << busiestMonth(repo) << '\n';
                std::cout << "Отчёт сохранён в output/report.txt\n";
            }
        };

        struct MenuItem
        {
            std::string key;
            std::string title;
            std::function<void()> action;
        };

        std::vector<MenuItem> items {
            {"1", "Список парка",                 doList},
            {"2", "Поиск по госномеру",           doFind},
            {"3", "Запас хода",                   doRange},
            {"4", "Расход топлива",               doFuel},
            {"5", "Максимальная нагрузка",        doLoad},
            {"6", "Требуют ТО",                   doService},
            {"7", "Проверка поездки",             doTrip},
            {"8", "Загруженный месяц",            doMonth},
            {"9", "Топ по расходу",               doTop},
            {"10", "Выполнить операции",          doRun},
            {"11", "Лист ожидания",               doWait},
            {"12", "Отменить операцию",           doUndo},
            {"13", "История операций",            doHistory},
            {"14", "Сформировать отчёт",          doReport},
            {"0", "Выход",                        {}}
        };

        auto showMenu = [&]
        {
            clearLine();
            std::cout << "  ГЛАВНОЕ МЕНЮ\n";
            clearLine();
            for (const auto& it : items)
                std::cout << std::right << std::setw(3) << it.key << ". " << it.title << '\n';
            clearLine();
        };

        std::string cmd;
        bool quit {false};
        while (!quit)
        {
            showMenu();
            std::cout << "Выберите пункт: ";
            bool got {static_cast<bool>(std::getline(std::cin, cmd))};
            if (!got)
            {
                quit = true;
            }
            else
            {
                bool found {false};
                for (const auto& it : items)
                {
                    if (it.key == cmd)
                    {
                        found = true;
                        if (it.key == "0")
                        {
                            quit = true;
                        }
                        else
                        {
                            try { it.action(); }
                            catch (const std::exception& e)
                            {
                                std::cerr << "Ошибка: " << e.what() << '\n';
                            }
                            pause();
                        }
                    }
                }
                if (!found)
                {
                    std::cout << "Неизвестный пункт. Введите номер из меню.\n";
                    pause();
                }
            }
        }

        std::cout << "До свидания!\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Критическая ошибка: " << e.what() << '\n';
    }
    return 0;
}