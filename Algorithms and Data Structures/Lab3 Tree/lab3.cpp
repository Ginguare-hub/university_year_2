#include <iostream>
#include <vector>
#include <locale.h>

struct Node {
    int key;
    Node* left;
    Node* right;
    bool leftThread;
    bool rightThread;
};

Node* root = nullptr;

Node* createNode(int value) {
    Node* node = new Node;
    node->key = value;
    node->left = nullptr;
    node->right = nullptr;
    node->leftThread = false;
    node->rightThread = false;
    return node;
}

bool contains(Node* node, int value) {
    if (node == nullptr) {
        return false;
    }
    if (value == node->key) {
        return true;
    }
    if (value < node->key) {
        return contains(node->left, value);
    }
    return contains(node->right, value);
}

Node* insertNode(Node* node, int value) {
    if (node == nullptr) {
        return createNode(value);
    }
    if (value < node->key) {
        node->left = insertNode(node->left, value);
    } else if (value > node->key) {
        node->right = insertNode(node->right, value);
    }
    return node;
}

Node* findMin(Node* node) {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

Node* removeNode(Node* node, int value) {
    if (node == nullptr) {
        return nullptr;
    }

    if (value < node->key) {
        node->left = removeNode(node->left, value);
        return node;
    }
    if (value > node->key) {
        node->right = removeNode(node->right, value);
        return node;
    }

    if (node->left == nullptr && node->right == nullptr) {
        delete node;
        return nullptr;
    }
    if (node->left == nullptr) {
        Node* child = node->right;
        delete node;
        return child;
    }
    if (node->right == nullptr) {
        Node* child = node->left;
        delete node;
        return child;
    }

    Node* successor = findMin(node->right);
    node->key = successor->key;
    node->right = removeNode(node->right, successor->key);
    return node;
}

void resetThreads(Node* node) {
    if (node == nullptr) {
        return;
    }

    if (node->leftThread) {
        node->left = nullptr;
        node->leftThread = false;
    } else {
        resetThreads(node->left);
    }

    if (node->rightThread) {
        node->right = nullptr;
        node->rightThread = false;
    } else {
        resetThreads(node->right);
    }
}

void destroyTree(Node* node) {
    if (node == nullptr) {
        return;
    }
    if (!node->leftThread) {
        destroyTree(node->left);
    }
    if (!node->rightThread) {
        destroyTree(node->right);
    }
    delete node;
}

void collect(Node* node, int order, std::vector<Node*>& nodes) {
    if (node == nullptr) {
        return;
    }

    if (order == 1) {
        nodes.push_back(node);
    }
    if (!node->leftThread) {
        collect(node->left, order, nodes);
    }
    if (order == 2) {
        nodes.push_back(node);
    }
    if (!node->rightThread) {
        collect(node->right, order, nodes);
    }
    if (order == 3) {
        nodes.push_back(node);
    }
}

const char* traversalName(int order) {
    if (order == 1) {
        return "прямой обход (RAB)";
    }
    if (order == 2) {
        return "симметричный обход (ARB)";
    }
    return "концевой обход (ABR)";
}

void printNode(Node* node, int depth) {
    if (node == nullptr) {
        return;
    }

    if (!node->rightThread) {
        printNode(node->right, depth + 1);
    }

    for (int i = 0; i < depth; i++) {
        std::cout << "    ";
    }
    std::cout << node->key;

    if (node->rightThread && node->right != nullptr) {
        std::cout << " (-> " << node->right->key << ")";
    }
    if (node->leftThread && node->left != nullptr) {
        std::cout << " (<- " << node->left->key << ")";
    }
    std::cout << '\n';

    if (!node->leftThread) {
        printNode(node->left, depth + 1);
    }
}

void printTree(Node* node) {
    if (node == nullptr) {
        std::cout << "Дерево пусто.\n";
        return;
    }
    std::cout << "Дерево:\n";
    printNode(node, 0);
}

void printTraversal(Node* node, int order) {
    std::vector<Node*> nodes;
    collect(node, order, nodes);

    if (nodes.empty()) {
        std::cout << "Дерево пусто.\n";
        return;
    }

    std::cout << traversalName(order) << ": ";
    for (int i = 0; i < (int)nodes.size(); i++) {
        if (i > 0) {
            std::cout << ' ';
        }
        std::cout << nodes[i]->key;
    }
    std::cout << '\n';
}

void threadTree(int order, bool toRight) {
    resetThreads(root);

    std::vector<Node*> nodes;
    collect(root, order, nodes);
    int count = (int)nodes.size();

    if (count == 0) {
        std::cout << "Дерево пусто, прошивать нечего.\n";
        return;
    }

    if (toRight) {
        for (int i = 0; i + 1 < count; i++) {
            if (nodes[i]->right == nullptr) {
                nodes[i]->right = nodes[i + 1];
                nodes[i]->rightThread = true;
            }
        }
        std::cout << "Дерево прошито вправо по порядку: "
                  << traversalName(order) << ".\n";
    } else {
        for (int i = 1; i < count; i++) {
            if (nodes[i]->left == nullptr) {
                nodes[i]->left = nodes[i - 1];
                nodes[i]->leftThread = true;
            }
        }
        std::cout << "Дерево прошито влево по порядку: "
                  << traversalName(order) << ".\n";
    }

    std::cout << "\n(-> X) - нить к следующему\n"
                 "(<- X) - нить к предыдущему\n";
    printTree(root);
}

int readInt() {
    int value;
    while (!(std::cin >> value)) {
        std::cout << "Ошибка: нужно целое число. Повторите ввод: ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
    return value;
}

int main() {
    setlocale(LC_ALL, "ru.utf-8");

    while (true) {
        std::cout << "\n====== Бинарное дерево поиска ======\n"
                  << " 1) Добавить один элемент\n"
                  << " 2) Добавить несколько элементов\n"
                  << " 3) Удалить элемент\n"
                  << " 4) Показать дерево\n"
                  << " 5) Прямой обход (RAB)\n"
                  << " 6) Симметричный обход (ARB)\n"
                  << " 7) Концевой обход (ABR)\n"
                  << " 8) Прошить дерево\n"
                  << " 0) Выход\n";

        std::cout << "> ";
        int choice = readInt();

        if (choice == 1) {
            std::cout << "Значение: ";
            int value = readInt();

            resetThreads(root);
            if (contains(root, value)) {
                std::cout << "Такой элемент уже есть.\n";
            } else {
                root = insertNode(root, value);
                std::cout << "Элемент добавлен.\n";
            }
            printTree(root);
        } else if (choice == 2) {
            std::cout << "Сколько элементов: ";
            int count = readInt();

            if (count <= 0) {
                std::cout << "Количество должно быть положительным.\n";
                continue;
            }

            resetThreads(root);
            int added = 0;
            for (int i = 0; i < count; i++) {
                std::cout << "Элемент " << i + 1 << ": ";
                int value = readInt();

                if (contains(root, value)) {
                    continue;
                }
                root = insertNode(root, value);
                added++;
            }

            std::cout << "Добавлено " << added << " из " << count << ".\n";
            printTree(root);
        } else if (choice == 3) {
            std::cout << "Значение для удаления: ";
            int value = readInt();

            resetThreads(root);
            if (!contains(root, value)) {
                std::cout << "Элемент не найден.\n";
            } else {
                root = removeNode(root, value);
                std::cout << "Элемент удалён.\n";
            }
            printTree(root);
        } else if (choice == 4) {
            printTree(root);
        } else if (choice == 5) {
            printTraversal(root, 1);
        } else if (choice == 6) {
            printTraversal(root, 2);
        } else if (choice == 7) {
            printTraversal(root, 3);
        } else if (choice == 8) {
            if (root == nullptr) {
                std::cout << "Дерево пусто, прошивать нечего.\n";
                continue;
            }

            std::cout << "Метод прохода (1 - RAB, 2 - ARB, 3 - ABR): ";
            int order = readInt();
            while (order < 1 || order > 3) {
                std::cout << "Введите 1, 2 или 3: ";
                order = readInt();
            }

            std::cout << "Направление (1 - вправо, 2 - влево): ";
            int direction = readInt();
            while (direction != 1 && direction != 2) {
                std::cout << "Введите 1 или 2: ";
                direction = readInt();
            }

            threadTree(order, direction == 1);
        } else if (choice == 0) {
            destroyTree(root);
            std::cout << "Выход.\n";
            return 0;
        } else {
            std::cout << "Неверный пункт меню.\n";
        }
    }
}
