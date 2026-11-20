#define _USE_MATH_DEFINES

#include "graph.h"
#include <windows.h>
#include <cmath>
#include <string>

static const Graph *graphToRender = nullptr;
static std::vector<POINT> nodeScreenCoordinates;
const int NODE_RADIUS = 25;

LRESULT CALLBACK WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        if (!graphToRender)
        {
            break;
        }

        int vertexCount = graphToRender->V;
        nodeScreenCoordinates.resize(vertexCount);

        // Центр окна и радиус для расположения вершин по кругу
        int centerX = 400;
        int centerY = 400;
        int layoutRadius = 250;

        for (int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
        {
            double nodeAngle = 2.0 * M_PI * vertexIndex / vertexCount - M_PI / 2.0;
            nodeScreenCoordinates[vertexIndex].x = centerX + (LONG)(layoutRadius * cos(nodeAngle));
            nodeScreenCoordinates[vertexIndex].y = centerY + (LONG)(layoutRadius * sin(nodeAngle));
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT paintStruct;
        HDC deviceContext = BeginPaint(windowHandle, &paintStruct);

        if (!graphToRender)
        {
            EndPaint(windowHandle, &paintStruct);
            break;
        }

        // Создаем инструменты для рисования (перо для линий, кисть для заливки)
        HPEN edgePen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
        HBRUSH nodeFillBrush = CreateSolidBrush(RGB(200, 10, 230));

        // Сохраняем старые инструменты, чтобы потом вернуть их обратно
        HGDIOBJ previousPen = SelectObject(deviceContext, edgePen);
        HGDIOBJ previousBrush = SelectObject(deviceContext, nodeFillBrush);

        SetBkMode(deviceContext, TRANSPARENT);
        SetTextColor(deviceContext, RGB(0, 0, 0));

        // 1. Рисуем ребра графа (линии и стрелки)
        for (int fromVertex = 0; fromVertex < graphToRender->V; fromVertex++)
        {
            for (int toVertex = 0; toVertex < graphToRender->V; toVertex++)
            {
                if (graphToRender->matrix[fromVertex][toVertex] != INF && fromVertex != toVertex)
                {
                    POINT startPoint = nodeScreenCoordinates[fromVertex];
                    POINT endPoint = nodeScreenCoordinates[toVertex];

                    double deltaX = endPoint.x - startPoint.x;
                    double deltaY = endPoint.y - startPoint.y;
                    double edgeLength = sqrt(deltaX * deltaX + deltaY * deltaY);
                    double edgeAngle = atan2(deltaY, deltaX);

                    // Вычисляем точки начала и конца линии, чтобы они не залезали внутрь кружков
                    int lineStartX = startPoint.x + (int)(NODE_RADIUS * cos(edgeAngle));
                    int lineStartY = startPoint.y + (int)(NODE_RADIUS * sin(edgeAngle));
                    int lineEndX = endPoint.x - (int)(NODE_RADIUS * cos(edgeAngle));
                    int lineEndY = endPoint.y - (int)(NODE_RADIUS * sin(edgeAngle));

                    MoveToEx(deviceContext, lineStartX, lineStartY, NULL);
                    LineTo(deviceContext, lineEndX, lineEndY);

                    // Рисуем стрелку на конце ребра
                    double arrowHeadAngle = M_PI / 6;
                    int arrowHeadLength = 12;
                    int arrowLeftX = lineEndX - (int)(arrowHeadLength * cos(edgeAngle - arrowHeadAngle));
                    int arrowLeftY = lineEndY - (int)(arrowHeadLength * sin(edgeAngle - arrowHeadAngle));
                    int arrowRightX = lineEndX - (int)(arrowHeadLength * cos(edgeAngle + arrowHeadAngle));
                    int arrowRightY = lineEndY - (int)(arrowHeadLength * sin(edgeAngle + arrowHeadAngle));

                    POINT arrowHeadPoints[3] = {{lineEndX, lineEndY}, {arrowLeftX, arrowLeftY}, {arrowRightX, arrowRightY}};
                    Polygon(deviceContext, arrowHeadPoints, 3);

                    // Выводим вес ребра
                    std::wstring edgeWeightText = std::to_wstring(graphToRender->matrix[fromVertex][toVertex]);

                    double middleX = (lineStartX + lineEndX) / 2.0;
                    double middleY = (lineStartY + lineEndY) / 2.0;

                    // Вычисляем нормаль (перпендикуляр) для смещения текста от линии
                    double normalX = -(lineEndY - lineStartY);
                    double normalY = (lineEndX - lineStartX);

                    double normalLength = sqrt(normalX * normalX + normalY * normalY);
                    if (normalLength > 0)
                    {
                        normalX /= normalLength;
                        normalY /= normalLength;
                    }

                    double textOffsetDistance = 15.0;
                    int weightTextX = (int)(middleX + normalX * textOffsetDistance);
                    int weightTextY = (int)(middleY + normalY * textOffsetDistance);

                    SetTextAlign(deviceContext, TA_CENTER | TA_BASELINE);
                    SetTextColor(deviceContext, RGB(200, 100, 0));
                    TextOutW(deviceContext, weightTextX, weightTextY, edgeWeightText.c_str(), edgeWeightText.length());
                }
            }
        }

        // 2. Рисуем сами узлы (кружки) и их номера
        SetTextColor(deviceContext, RGB(0, 0, 0));
        for (int vertexIndex = 0; vertexIndex < graphToRender->V; vertexIndex++)
        {
            int nodeX = nodeScreenCoordinates[vertexIndex].x;
            int nodeY = nodeScreenCoordinates[vertexIndex].y;

            Ellipse(deviceContext, nodeX - NODE_RADIUS, nodeY - NODE_RADIUS, nodeX + NODE_RADIUS, nodeY + NODE_RADIUS);

            std::wstring vertexNumberText = std::to_wstring(vertexIndex + 1);

            SetTextAlign(deviceContext, TA_CENTER | TA_BASELINE);
            TextOutW(deviceContext, nodeX, nodeY + 5, vertexNumberText.c_str(), vertexNumberText.length());
        }
        SetTextAlign(deviceContext, TA_LEFT | TA_BASELINE);

        // Возвращаем старые инструменты и удаляем созданные, чтобы не было утечек памяти
        SelectObject(deviceContext, previousPen);
        SelectObject(deviceContext, previousBrush);
        DeleteObject(edgePen);
        DeleteObject(nodeFillBrush);

        EndPaint(windowHandle, &paintStruct);
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    default:
    {
        return DefWindowProc(windowHandle, message, wParam, lParam);
    }
    }
    return 0;
}

void showGraphWindow(const Graph &graph)
{
    graphToRender = &graph;

    // Меняем W на A
    WNDCLASSA windowClass = {0};
    windowClass.lpfnWndProc = WndProc;
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_GRADIENTACTIVECAPTION + 1);
    windowClass.lpszClassName = "GraphWindowClass"; // Без L

    RegisterClassA(&windowClass); // Меняем W на A

    HWND windowHandle = CreateWindowA(
        "GraphWindowClass", // Без L
        "551004",           // Без L
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        800,
        NULL,
        NULL,
        windowClass.hInstance,
        NULL);

    if (!windowHandle)
    {
        return;
    }

    MSG message;
    while (GetMessage(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}