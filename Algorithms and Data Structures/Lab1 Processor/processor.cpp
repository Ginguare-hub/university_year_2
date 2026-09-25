#include <climits>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

struct Client
{
    int id;
    int priority;
    std::vector<int> bursts;  // длины всплесков (CPU-фазы)
    std::string timeline;     // визуализация каждого такта
};

struct ProcessState
{
    int burstIndex = 0;
    int burstDone = 0;
    int ioRemaining = 0;
    int readySeq = -1;
    bool ready = true;
    bool inIO = false;
    bool done = false;
};

struct SchedulerState
{
    std::vector<Client> clients;       // данные о клиентах
    std::vector<ProcessState> states;  // состояние каждого клиента

    int quantum;  // t такта
    int ioTime;   // t загрузки

    int current = -1;   // кто сейчас работает на ЦП (-1 — никто)
    int quantumLeft = 0;
    int nextSeq = 0;
    int startingIo = -1;

    int finished = 0;
    int totalTicks = 0;
    int idleTicks = 0;

    std::string cpuTimeline;  // когда ЦП простаивал (*)
};

// Создаёт одного клиента из массива всплесков
Client makeClient(int id, int priority, const int bursts[], int burstCount)
{
    Client client;
    client.id = id;
    client.priority = priority;
    client.timeline = "";

    for (int i = 0; i < burstCount; i++)
    {
        client.bursts.push_back(bursts[i]);
    }

    return client;
}

// Формирует список всех клиентов
std::vector<Client> makeClients()
{
    int b1[] = {6, 8, 7, 6, 3, 5, 4, 3, 2, 1};
    int b2[] = {5, 4, 6, 8, 3, 2, 4, 6, 1, 2, 4};
    int b3[] = {4, 6, 8, 3, 2, 6, 3, 4, 8, 9, 2, 1};
    int b4[] = {4, 3, 2, 1, 2, 3, 4, 6, 3, 4, 3};
    int b5[] = {3, 4, 3, 2, 1, 6, 3, 4, 2, 1};
    int b6[] = {2, 1, 2, 3, 3, 3, 2, 1, 1, 1};
    int b7[] = {4, 2, 2, 2, 3, 4, 2, 2, 1};
    int b8[] = {3, 2, 3, 2, 3, 2, 3, 2, 3, 2};

    std::vector<Client> clients;
    clients.push_back(makeClient(1, 0, b1, 10));
    clients.push_back(makeClient(2, 0, b2, 11));
    clients.push_back(makeClient(3, 0, b3, 12));
    clients.push_back(makeClient(4, 1, b4, 11));
    clients.push_back(makeClient(5, 1, b5, 10));
    clients.push_back(makeClient(6, 2, b6, 10));
    clients.push_back(makeClient(7, 2, b7, 9));
    clients.push_back(makeClient(8, 2, b8, 10));

    return clients;
}

// Ввод целого числа с проверкой корректности
int readNumber(int min, int max, const std::string &prompt)
{
    while (true)
    {
        std::cout << prompt;
        int value;
        std::cin >> value;

        if (std::cin.fail() || std::cin.get() != '\n')
        {
            std::cin.clear();
            while (std::cin.get() != '\n')
            {
            }
            std::cout << "Некорректный ввод! Попробуйте снова.\n";
            continue;
        }

        if (value < min || value > max)
        {
            std::cout << "Число должно быть в диапазоне от " << min
                      << " до " << max << "! Попробуйте снова.\n";
            continue;
        }

        return value;
    }
}

// Показывает исходные данные клиентов
void printClients(const std::vector<Client> &clients)
{
    for (int i = 0; i < (int)clients.size(); i++)
    {
        std::cout << "\nКлиент №" << clients[i].id + 1
                  << "\nПриоритет: " << clients[i].priority
                  << "\nВсплески: ";

        for (int j = 0; j < (int)clients[i].bursts.size(); j++)
        {
            std::cout << clients[i].bursts[j] << ' ';
        }
        std::cout << '\n';
    }
}

// Отправляет клиента на "загрузку" (ввод-вывод) или обратно в очередь
void shiftToIo(SchedulerState &s, int id)
{
    ProcessState &st = s.states[id];
    if (s.ioTime == 0)
    {
        st.ready = true;
        st.readySeq = s.nextSeq;
        s.nextSeq = s.nextSeq + 1;
    }
    else
    {
        st.inIO = true;
        st.ioRemaining = s.ioTime;
        s.startingIo = id;
    }
}

// Выбирает следующего клиента: меньший приоритет, затем порядковый номер
void pickNext(SchedulerState &s)
{
    int best = -1;
    int bestPriority = INT_MAX;
    int bestSeq = INT_MAX;

    for (int i = 0; i < (int)s.clients.size(); i++)
    {
        ProcessState &st = s.states[i];
        if (!st.ready || st.done)
        {
            continue;
        }

        int priority = s.clients[i].priority;
        if (priority < bestPriority ||
            (priority == bestPriority && st.readySeq < bestSeq))
        {
            bestPriority = priority;
            bestSeq = st.readySeq;
            best = i;
        }
    }

    if (best != -1)
    {
        s.current = best;
        s.states[best].ready = false;
        s.quantumLeft = s.quantum;
    }
}

// Выполняет один такт работы текущего клиента
void stepCurrent(SchedulerState &s)
{
    ProcessState &st = s.states[s.current];
    st.burstDone = st.burstDone + 1;
    s.quantumLeft = s.quantumLeft - 1;

    int burstLength = s.clients[s.current].bursts[st.burstIndex];

    if (st.burstDone >= burstLength)
    {
        st.burstIndex = st.burstIndex + 1;
        st.burstDone = 0;

        if (st.burstIndex >= (int)s.clients[s.current].bursts.size())
        {
            st.done = true;  // все всплески выполнены
            s.finished = s.finished + 1;
        }
        else
        {
            shiftToIo(s, s.current);
        }

        s.current = -1;
        s.quantumLeft = 0;
    }
    else if (s.quantumLeft == 0)
    {
        shiftToIo(s, s.current);
        s.current = -1;
    }
}

// Добавляет символ такта в историю каждого клиента и историю ЦП
void appendTimelines(SchedulerState &s, bool isBoundary, int ran)
{
    int n = (int)s.clients.size();

    for (int i = 0; i < n; i++)
    {
        ProcessState &st = s.states[i];

        if (isBoundary)
        {
            s.clients[i].timeline = s.clients[i].timeline + '|';
        }

        char ch;
        if (i == ran)
        {
            ch = 'L';  // работа ЦП
        }
        else if (st.done)
        {
            ch = ' ';  // завершение работы
        }
        else if (st.inIO)
        {
            ch = 'b';  // загрузка
        }
        else
        {
            ch = '-';  // ожидание
        }

        s.clients[i].timeline = s.clients[i].timeline + ch;
    }

    if (isBoundary)
    {
        s.cpuTimeline = s.cpuTimeline + '|';
    }

    if (ran != -1)
    {
        s.cpuTimeline = s.cpuTimeline + ' ';
    }
    else
    {
        s.cpuTimeline = s.cpuTimeline + '*';  // простой ЦП
    }
}

// Уменьшает оставшееся время "загрузки" у всех клиентов
void tickIo(SchedulerState &s)
{
    for (int i = 0; i < (int)s.states.size(); i++)
    {
        ProcessState &st = s.states[i];

        bool justStarted = (s.startingIo == i);
        if (st.done || justStarted)
        {
            continue;
        }

        if (st.ioRemaining > 0)
        {
            st.ioRemaining = st.ioRemaining - 1;
            if (st.ioRemaining == 0)
            {
                st.inIO = false;
                st.ready = true;
                st.readySeq = s.nextSeq;
                s.nextSeq = s.nextSeq + 1;
            }
        }
    }
}

// Сумма всех всплесков — сколько тактов ЦП нужен полезно
int requiredTicks(const SchedulerState &s)
{
    int sum = 0;

    for (int i = 0; i < (int)s.clients.size(); i++)
    {
        for (int j = 0; j < (int)s.clients[i].bursts.size(); j++)
        {
            sum = sum + s.clients[i].bursts[j];
        }
    }

    return sum;
}

// Создаёт состояние симуляции
SchedulerState initScheduler(std::vector<Client> clients, int quantum, int ioTime)
{
    SchedulerState s;
    s.clients = clients;
    s.states.resize(s.clients.size());
    s.quantum = quantum;
    s.ioTime = ioTime;

    for (int i = 0; i < (int)s.states.size(); i++)
    {
        s.states[i].readySeq = i;
    }

    return s;
}

// Запускает симуляцию до конца
void runSimulation(SchedulerState &s)
{
    while (s.finished < (int)s.clients.size())
    {
        bool isBoundary = (s.totalTicks % s.quantum == 0);

        s.startingIo = -1;
        if (isBoundary && s.current == -1)
        {
            pickNext(s);
        }

        int ran = s.current;
        if (ran != -1)
        {
            stepCurrent(s);
        }
        else
        {
            s.idleTicks = s.idleTicks + 1;
        }

        appendTimelines(s, isBoundary, ran);
        tickIo(s);

        s.totalTicks = s.totalTicks + 1;
    }
}

struct ReportRow
{
    int id;
    int priority;
    std::string timeline;
};

struct SimulationReport
{
    int quantum = 0;
    int ioTime = 0;
    std::string cpuTimeline;
    int totalTicks = 0;
    int idleTicks = 0;
    int requiredTicks = 0;
    std::vector<ReportRow> rows;
};

// КПД = процент тактов, когда ЦП реально работал
double utilization(const SimulationReport &report)
{
    if (report.totalTicks == 0)
    {
        return 0.0;
    }
    else
    {
        return 100.0 * report.requiredTicks / report.totalTicks;
    }
}

// Собирает данные для отчёта
SimulationReport buildReport(const SchedulerState &s)
{
    SimulationReport report;
    report.quantum = s.quantum;
    report.ioTime = s.ioTime;
    report.cpuTimeline = s.cpuTimeline;
    report.totalTicks = s.totalTicks;
    report.idleTicks = s.idleTicks;
    report.requiredTicks = requiredTicks(s);

    for (int i = 0; i < (int)s.clients.size(); i++)
    {
        ReportRow row;
        row.id = s.clients[i].id;
        row.priority = s.clients[i].priority;
        row.timeline = s.clients[i].timeline;
        report.rows.push_back(row);
    }

    return report;
}

// Записывает отчёт в файл
void writeReportToFile(const SimulationReport &report, const std::string &path)
{
    std::ofstream out(path);
    if (!out)
    {
        std::cerr << "Не удалось открыть файл " << path << '\n';
        return;
    }

    out << "РЕЗУЛЬТАТЫ СИМУЛЯЦИИ\n";
    out << "t такта: " << report.quantum
        << "\nt загрузки: " << report.ioTime << "\n\n";

    out << "  'L' — работа ЦП\n";
    out << "  'b' — загрузка\n";
    out << "  '-' — ожидание\n";
    out << "  ' ' — завершение работы на потоке\n";
    out << "  '*' — простой\n\n";

    for (int i = 0; i < (int)report.rows.size(); i++)
    {
        out << "Клиент " << report.rows[i].id + 1
            << " (приоритет " << report.rows[i].priority << "): "
            << report.rows[i].timeline << '\n';
    }

    out << "Простой ЦП:             " << report.cpuTimeline << "\n\n";

    out << "t ВСЕГО:   " << report.totalTicks << " тактов\n";
    out << "t работы:  " << report.requiredTicks << " тактов\n";
    out << "t простоя: " << report.idleTicks << " тактов\n";
    out << "КПД: " << std::fixed << std::setprecision(4)
        << utilization(report) << "%\n";

    std::cout << "\nРезультат сохранён в файл: " << path << '\n';
}

// Выводит сводку в терминал
void printSummaryToConsole(const SimulationReport &report)
{
    std::cout << "\n\n=== СВОДКА ===\n";
    std::cout << "t такта:   " << report.quantum << '\n';
    std::cout << "t загрузки: " << report.ioTime << '\n';
    std::cout << "t ВСЕГО:   " << report.totalTicks << " тактов\n";
    std::cout << "t работы:  " << report.requiredTicks << " тактов\n";
    std::cout << "t простоя: " << report.idleTicks << " тактов\n";
    std::cout << "КПД:       " << std::fixed << std::setprecision(4)
              << utilization(report) << "%\n";
}

int main()
{
    setlocale(LC_ALL, "ru.UTF-8");

    std::vector<Client> clients = makeClients();
    printClients(clients);

    int quantum = readNumber(1, 10, "Введите t такта: ");
    int ioTime = readNumber(0, 10, "Введите t загрузки: ");

    SchedulerState scheduler = initScheduler(clients, quantum, ioTime);
    runSimulation(scheduler);

    SimulationReport report = buildReport(scheduler);
    writeReportToFile(report, "simulation_result.txt");
    printSummaryToConsole(report);

    return 0;
}