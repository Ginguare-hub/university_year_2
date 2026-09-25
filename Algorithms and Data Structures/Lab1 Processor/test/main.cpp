#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>

const int CLIENT_COUNT = 6;

struct client
{
    int index;
    int priority;
    std::string story;
    std::vector<int> surges;
};

void getConstData(client *in)
{
    int c1[] = {4, 3, 1, 8, 6, 5, 2, 3};
    int c2[] = {3, 2, 4, 7, 2, 6, 8};
    int c3[] = {3, 1, 3, 1, 5, 4, 3, 2};
    int c4[] = {3, 2, 1, 3, 2, 4, 3};
    int c5[] = {1, 2, 1, 4, 2, 6};
    int c6[] = {2, 6, 3, 2, 1, 3, 2, 3};
    int *arrays[] = {c1, c2, c3, c4, c5, c6};
    int sizes[] = {8, 7, 8, 7, 6, 8};
    for (int i = 0; i < CLIENT_COUNT; i++)
    {
        in[i].index = i;
        if (i < 3)
        {
            in[i].priority = 0;
        }
        else if (i == 3)
        {
            in[i].priority = 1;
        }
        else
        {
            in[i].priority = 2;
        }
        in[i].story = "";
        in[i].surges.assign(arrays[i], arrays[i] + sizes[i]);
    }
}

int intInput(int min, int max, std::string mainMessage)
{
    int input;
    bool isIncorrect;
    do
    {
        isIncorrect = false;
        std::cout << mainMessage;
        std::cin >> input;
        if (std::cin.fail() || std::cin.get() != '\n')
        {
            std::cout << "Некорректный ввод! Попробуйте снова: " << std::endl;
            std::cin.clear();
            isIncorrect = true;
            while (std::cin.get() != '\n')
                ;
        }
        if (!isIncorrect && (input < min || input > max))
        {
            std::cout << "Число должно быть в диапазоне от " << min << " до " << max << " ! Попробуйте снова." << std::endl;
            std::cin.clear();
            isIncorrect = true;
        }
    } while (isIncorrect);
    return input;
}

void outputClients(client *in)
{
    for (int i = 0; i < CLIENT_COUNT; i++)
    {
        std::cout << "\nКлиент №" << in[i].index + 1
                  << "\nПриоритет: " << in[i].priority
                  << "\nВсплески: ";
        for (int s : in[i].surges)
            std::cout << s << " ";
        std::cout << "\n";
    }
}

void runProcessor(client *in, int tact, int waiting)
{
    struct State
    {
        int burstIdx = 0;
        int burstDone = 0;
        int ioRemaining = 0;
        bool ready = true;
        bool done = false;
        int readySeq = 0;
        bool inIO = false;
    } st[CLIENT_COUNT];

    for (int i = 0; i < CLIENT_COUNT; i++)
        st[i].readySeq = i;
    int nextSeq = CLIENT_COUNT;

    int current = -1;
    int quantumLeft = 0;
    int skipped = 0;
    int idle = 0;
    int doneCount = 0;

    int required = 0;
    for (int i = 0; i < CLIENT_COUNT; i++)
        for (int s : in[i].surges)
            required += s;

    std::string cpuStory = "";

    while (doneCount < CLIENT_COUNT)
    {
        bool boundary = (skipped % tact == 0);
        bool ranThisTick[CLIENT_COUNT] = {false};
        bool idleThisTick = false;
        bool ioJustSent[CLIENT_COUNT] = {false};

        if (boundary && current == -1)
        {
            int chosen = -1, bestPrio = INT_MAX, bestSeq = INT_MAX;
            for (int i = 0; i < CLIENT_COUNT; i++)
            {
                if (st[i].ready && !st[i].done)
                {
                    if (in[i].priority < bestPrio ||
                        (in[i].priority == bestPrio && st[i].readySeq < bestSeq))
                    {
                        bestPrio = in[i].priority;
                        bestSeq = st[i].readySeq;
                        chosen = i;
                    }
                }
            }
            if (chosen != -1)
            {
                current = chosen;
                st[chosen].ready = false;
                quantumLeft = tact;
            }
        }

        if (current != -1)
        {
            ranThisTick[current] = true;
            st[current].burstDone++;
            quantumLeft--;
            skipped++;

            if (st[current].burstDone >= in[current].surges[st[current].burstIdx])
            {
                st[current].burstIdx++;
                st[current].burstDone = 0;

                if (st[current].burstIdx >= (int)in[current].surges.size())
                {
                    st[current].done = true;
                    doneCount++;
                    current = -1;
                    quantumLeft = 0;
                }
                else
                {
                    if (waiting == 0)
                    {
                        st[current].ready = true;
                        st[current].readySeq = nextSeq++;
                    }
                    else
                    {
                        st[current].ioRemaining = waiting;
                        st[current].inIO = true;
                        ioJustSent[current] = true;
                    }
                    current = -1;
                    quantumLeft = 0;
                }
            }
            else if (quantumLeft == 0)
            {
                if (waiting == 0)
                {
                    st[current].ready = true;
                    st[current].readySeq = nextSeq++;
                }
                else
                {
                    st[current].ioRemaining = waiting;
                    st[current].inIO = true;
                    ioJustSent[current] = true;
                }
                current = -1;
            }
        }
        else
        {
            idle++;
            skipped++;
            idleThisTick = true;
        }

        for (int i = 0; i < CLIENT_COUNT; i++)
        {
            if (boundary)
                in[i].story += '|';
            if (ranThisTick[i])
                in[i].story += 'L';
            else if (st[i].done)
                in[i].story += ' ';
            else if (st[i].inIO)
                in[i].story += 'd';
            else
                in[i].story += '-';
        }
        if (boundary)
            cpuStory += '|';
        cpuStory += idleThisTick ? '*' : ' ';

        for (int i = 0; i < CLIENT_COUNT; i++)
        {
            if (!st[i].done && st[i].ioRemaining > 0 && !ioJustSent[i])
            {
                st[i].ioRemaining--;
                if (st[i].ioRemaining == 0)
                {
                    st[i].inIO = false;
                    st[i].ready = true;
                    st[i].readySeq = nextSeq++;
                }
            }
        }
    }

    std::ofstream fout("simulation_result.txt");
    if (!fout.is_open())
    {
        std::cerr << "Не удалось открыть файл simulation_result.txt\n";
        return;
    }

    fout << "РЕЗУЛЬТАТЫ СИМУЛЯЦИИ\n";
    fout << "t такта: " << tact << "\nt простоя: " << waiting << "\n\n";

    fout << "  'L' — работа ЦП\n";
    fout << "  'd' — загрузка\n";
    fout << "  '-' — ожидание\n";
    fout << "  ' ' — завершение работы на потоке\n";
    fout << "  '*' — простой\n";

    for (int i = 0; i < CLIENT_COUNT; i++)
    {
        fout << "Клиент " << (i + 1)
             << " (приоритет " << in[i].priority << "): "
             << in[i].story << "\n";
    }
    fout << "Простой ЦП:             "
         << cpuStory << "\n";

    fout << "\n\nt ВСЕГО:   " << skipped << " тактов\n";
    fout << "t работы:  " << required << " тактов\n";
    fout << "t простоя: " << idle << " тактов\n";

    double kpd = (double)required / skipped * 100;
    fout << "КПД: " << std::fixed
         << std::setprecision(4) << kpd << "%\n";

    fout.close();
    std::cout << "\nРезультат сохранён в файл: simulation_result.txt\n";
}

int main()
{
    setlocale(LC_ALL, "ru.UTF-8");
    int tact, waiting;
    client in[CLIENT_COUNT];
    getConstData(in);
    tact = intInput(1, 10, "Введите t такта: ");
    waiting = intInput(0, 10, "Введите t загрузки: ");
    outputClients(in);
    runProcessor(in, tact, waiting);
    return 0;
}