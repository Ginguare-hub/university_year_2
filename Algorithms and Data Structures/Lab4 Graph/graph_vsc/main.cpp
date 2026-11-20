#include "graph.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <windows.h>

using namespace std;

struct PathInfo
{
    int distance;
    vector<int> vertices;
};

Graph loadMatrix(const string &filename)
{
    Graph graph;
    ifstream inputFile(filename);

    if (!inputFile.is_open())
    {
        cerr << "Ошибка открытия файла!" << endl;
        exit(1);
    }

    inputFile >> graph.V;
    graph.matrix.assign(graph.V, vector<int>(graph.V, INF));

    for (int row = 0; row < graph.V; row++)
    {
        for (int col = 0; col < graph.V; col++)
        {
            int weight;
            inputFile >> weight;

            if ((weight == 0 && row != col) || weight == -1)
            {
                continue;
            }

            graph.matrix[row][col] = weight;
        }
    }

    return graph;
}

Graph loadList(const string &filename)
{
    Graph graph;
    ifstream inputFile(filename);

    if (!inputFile.is_open())
    {
        cerr << "Ошибка открытия файла!" << endl;
        exit(1);
    }

    int edgeCount;
    inputFile >> graph.V >> edgeCount;
    graph.matrix.assign(graph.V, vector<int>(graph.V, INF));

    for (int i = 0; i < graph.V; i++)
    {
        graph.matrix[i][i] = 0;
    }

    for (int edgeIndex = 0; edgeIndex < edgeCount; edgeIndex++)
    {
        int fromVertex, toVertex, weight;
        inputFile >> fromVertex >> toVertex >> weight;
        graph.matrix[fromVertex - 1][toVertex - 1] = weight;
    }

    return graph;
}

void dijkstra(const Graph &graph, int startVertex)
{
    int vertexCount = graph.V;
    vector<int> distances(vertexCount, INF);
    vector<int> previousVertex(vertexCount, -1);

    distances[startVertex] = 0;

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> priorityQueue;
    priorityQueue.push({0, startVertex});

    while (!priorityQueue.empty())
    {
        int currentDistance = priorityQueue.top().first;
        int currentVertex = priorityQueue.top().second;
        priorityQueue.pop();

        if (currentDistance > distances[currentVertex])
        {
            continue;
        }

        for (int neighborVertex = 0; neighborVertex < vertexCount; neighborVertex++)
        {
            if (graph.matrix[currentVertex][neighborVertex] != INF &&
                distances[currentVertex] + graph.matrix[currentVertex][neighborVertex] < distances[neighborVertex])
            {
                distances[neighborVertex] = distances[currentVertex] + graph.matrix[currentVertex][neighborVertex];
                previousVertex[neighborVertex] = currentVertex;
                priorityQueue.push({distances[neighborVertex], neighborVertex});
            }
        }
    }

    cout << "\n==== Алгоритм Дейкстры (" << startVertex + 1 << " - начальная вершина) ====" << endl;

    for (int targetVertex = 0; targetVertex < vertexCount; targetVertex++)
    {
        if (targetVertex == startVertex)
        {
            continue;
        }

        cout << "Путь до вершины " << targetVertex + 1 << ": ";

        if (distances[targetVertex] == INF)
        {
            cout << "НЕ существует" << endl;
        }
        else
        {
            cout << "длина = " << distances[targetVertex] << ", путь: ";
            vector<int> path;
            for (int at = targetVertex; at != -1; at = previousVertex[at])
            {
                path.push_back(at + 1);
            }
            reverse(path.begin(), path.end());

            for (size_t pathIndex = 0; pathIndex < path.size(); pathIndex++)
            {
                cout << path[pathIndex];
                if (pathIndex < path.size() - 1)
                    cout << " -> ";
            }
            cout << endl;
        }
    }
}

vector<vector<int>> floydWarshall(const Graph &graph)
{
    int vertexCount = graph.V;
    vector<vector<int>> distances = graph.matrix;

    for (int intermediate = 0; intermediate < vertexCount; intermediate++)
    {
        for (int fromVertex = 0; fromVertex < vertexCount; fromVertex++)
        {
            for (int toVertex = 0; toVertex < vertexCount; toVertex++)
            {
                if (distances[fromVertex][intermediate] != INF &&
                    distances[intermediate][toVertex] != INF &&
                    distances[fromVertex][intermediate] + distances[intermediate][toVertex] < distances[fromVertex][toVertex])
                {
                    distances[fromVertex][toVertex] = distances[fromVertex][intermediate] + distances[intermediate][toVertex];
                }
            }
        }
    }

    return distances;
}

void findCenter(const vector<vector<int>> &distances)
{
    // (Код функции остался прежним)
    int vertexCount = distances.size();
    int minEccentricity = INF;
    int centerVertex = -1;
    cout << "\n==== Эксцентриситеты и центр графа ====" << endl;
    for (int vertex = 0; vertex < vertexCount; vertex++)
    {
        int maxDistance = 0;
        bool isIsolated = false;
        for (int neighbor = 0; neighbor < vertexCount; neighbor++)
        {
            if (vertex != neighbor)
            {
                if (distances[vertex][neighbor] == INF)
                {
                    isIsolated = true;
                    break;
                }
                maxDistance = max(maxDistance, distances[vertex][neighbor]);
            }
        }
        if (!isIsolated)
        {
            cout << "Вершина " << vertex + 1 << ": эксцентриситет = " << maxDistance << endl;
            if (maxDistance < minEccentricity)
            {
                minEccentricity = maxDistance;
                centerVertex = vertex;
            }
        }
    }
    if (centerVertex != -1)
        cout << "Центр графа: вершина " << centerVertex + 1 << " (эксцентриситет " << minEccentricity << ")" << endl;
}

void findAllPathsDFS(const Graph &graph, int currentVertex, int targetVertex,
                     vector<bool> &visited, vector<int> &currentPath, int currentDistance,
                     vector<PathInfo> &allPaths)
{
    currentPath.push_back(currentVertex);

    if (currentVertex == targetVertex)
    {
        allPaths.push_back({currentDistance, currentPath});
    }
    else
    {
        visited[currentVertex] = true;
        for (int nextVertex = 0; nextVertex < graph.V; nextVertex++)
        {
            if (graph.matrix[currentVertex][nextVertex] != INF && !visited[nextVertex])
            {
                findAllPathsDFS(graph, nextVertex, targetVertex, visited, currentPath,
                                currentDistance + graph.matrix[currentVertex][nextVertex], allPaths);
            }
        }
        visited[currentVertex] = false; // Возврат (backtracking)
    }

    currentPath.pop_back();
}

void printAllPathsAndLongest(const Graph &graph, int startVertex, int endVertex)
{
    vector<PathInfo> allPaths;
    vector<bool> visited(graph.V, false);
    vector<int> currentPath;

    findAllPathsDFS(graph, startVertex, endVertex, visited, currentPath, 0, allPaths);

    cout << "\n==== Все пути от вершины " << startVertex + 1 << " до " << endVertex + 1 << " ====" << endl;

    if (allPaths.empty())
    {
        cout << "Пути между заданными вершинами не существуют." << endl;
        return;
    }

    sort(allPaths.begin(), allPaths.end(), [](const PathInfo &a, const PathInfo &b)
         { return a.distance < b.distance; });

    for (size_t i = 0; i < allPaths.size(); i++)
    {
        cout << i + 1 << ") Длина = " << setw(3) << allPaths[i].distance << " | Путь: ";
        for (size_t j = 0; j < allPaths[i].vertices.size(); j++)
        {
            cout << allPaths[i].vertices[j] + 1;
            if (j < allPaths[i].vertices.size() - 1)
                cout << " -> ";
        }
        cout << endl;
    }

    cout << "\n>>> САМЫЙ ДЛИННЫЙ ПУТЬ <<<" << endl;
    cout << "Длина = " << allPaths.back().distance << " | Путь: ";
    for (size_t j = 0; j < allPaths.back().vertices.size(); j++)
    {
        cout << allPaths.back().vertices[j] + 1;
        if (j < allPaths.back().vertices.size() - 1)
            cout << " -> ";
    }
    cout << endl;
}

int main()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int inputTypeChoice;
    cout << "Выберите тип входных данных:\n";
    cout << " 1) Матрица смежности\n";
    cout << " 2) Списки смежности\n";
    cout << "> ";
    cin >> inputTypeChoice;

    string filename;
    Graph graph;

    if (inputTypeChoice == 1)
    {
        filename = "matrix.txt";
        graph = loadMatrix(filename);
    }
    else if (inputTypeChoice == 2)
    {
        filename = "list.txt";
        graph = loadList(filename);
    }
    else
    {
        cerr << "Ошибка: Неверный выбор" << endl;
        return 1;
    }

    int startVertex;
    cout << "Введите начальную вершину для алгоритма Дейкстры (1.." << graph.V << "): ";
    cin >> startVertex;

    if (startVertex < 1 || startVertex > graph.V)
    {
        cout << "Некорректная вершина. По умолчанию выбрана 1." << endl;
        startVertex = 1;
    }

    dijkstra(graph, startVertex - 1);

    vector<vector<int>> shortestPaths = floydWarshall(graph);

    findCenter(shortestPaths);

    int pathStart, pathEnd;
    cout << "\nВведите две вершины для поиска ВСЕХ путей и самого длинного (через пробел): ";
    cin >> pathStart >> pathEnd;

    if (pathStart >= 1 && pathStart <= graph.V && pathEnd >= 1 && pathEnd <= graph.V)
    {
        printAllPathsAndLongest(graph, pathStart - 1, pathEnd - 1);
    }
    else
    {
        cout << "Некорректные номера вершин для поиска путей." << endl;
    }

    cout << "\nНажмите ENTER для визуализации графа..." << endl;
    cin.ignore();
    cin.get();

    showGraphWindow(graph);

    return 0;
}