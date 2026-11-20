#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <vector>
#include <limits>

const int INF = (std::numeric_limits<int>::max)();

struct Graph
{
    int V = 0;
    std::vector<std::vector<int>> matrix;
};

void showGraphWindow(const Graph &g);