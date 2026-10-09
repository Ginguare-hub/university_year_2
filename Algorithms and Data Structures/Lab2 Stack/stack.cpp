#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

const int MAX_STACK = 256;

struct stack
{
    char data[MAX_STACK];
    size_t index = 0;    // Пустой, указывает на след. пустую "ячейку"
} my_stack;

// Stack operations
bool isEmpty(const struct stack &s);
void push(char element, struct stack &s);
bool pop(struct stack &s, char &out);
bool top(struct stack &s, char &out);
//

bool translate(std::string_view infix, std::string &postfix, int &rang);
bool validate(char c);
int get_stack_priority(char c);
int get_relative_priority(char c);
int get_rang(char c);
void append_to_postfix(std::string &postfix, int &rang);
bool pop_all_to_postfix(std::string &postfix, int &rang);

int main()
{
    setlocale(LC_ALL, "ru.utf-8");
    std::string infix, postfix;
    int rang;
    
    std::cout << "Введите инфиксную строку-выражение: ";
    std::getline(std::cin, infix);
    postfix.reserve(infix.size());
    infix.erase(std::remove(infix.begin(), infix.end(), ' '), infix.end());

    if (translate(infix, postfix, rang))
    {
        if (rang == 1)
        {
            std::cout << "Постфиксная запись: \n" << postfix << "\n";
        }
        else
        {
            std::cout << "Некорректный ввод инфиксного выражения\n";
            std::cout << "Постфиксная запись: \n" << postfix << "\n";
        }
        std::cout << "Ранг: " << rang << "\n";
    }
    else
    {
        std::cout << "Некорректный ввод инфиксного выражения\n";
    }

    return 0;
}

void init(struct stack &s)
{
    s.index = 0;
}

bool isEmpty(const struct stack &s)
{
    if (s.index == 0)
    {
        return true;
    }

    return false;
}

void push(char element, struct stack &s)
{
    if (s.index >= MAX_STACK)
    {
        std::cerr << "ERROR: Stack overflow\n";
        return;
    }

    s.data[s.index] = element;
    s.index++;
}

bool pop(struct stack &s, char &out)
{
    if (isEmpty(s))
    {
        return false;
    }

    s.index--;
    out = s.data[s.index];
    return true;
}

bool top(struct stack &s, char &out)
{
    if (isEmpty(s))
    {
        out = ' ';
        return false;
    }

    out = s.data[s.index-1];
    return true;
}

bool translate(std::string_view infix, std::string &postfix, int &rang)
{
    char last{' '}, temp;
    rang = 0;

    init(my_stack);

    for (char c : infix)
    {
        if (!validate(c)) 
        {
            return false;
        }

        if (!isEmpty(my_stack))
        {
            top(my_stack, last);
        }

        if (c == ')')
        {
            while (last != '(')
            {
                if (isEmpty(my_stack))
                {
                    return false;
                }
                append_to_postfix(postfix, rang);
                top(my_stack, last);
            }
            pop(my_stack, temp);
        }
        else
        {
            if (get_relative_priority(c) > get_stack_priority(last))
            {
                push(c, my_stack);
            }
            else
            {
                while (get_relative_priority(c) <= get_stack_priority(last))
                {
                    append_to_postfix(postfix, rang);
                    top(my_stack, last);
                }
                push(c, my_stack);
            }
        }
    }

    return pop_all_to_postfix(postfix, rang);
}

void append_to_postfix(std::string &postfix, int &rang)
{
    char temp;
    if (pop(my_stack, temp))
    {
        postfix += temp;
        rang += get_rang(temp);  
    }   
}

bool pop_all_to_postfix(std::string &postfix, int &rang)
{
    char op;

    while (!isEmpty(my_stack))
    {
        pop(my_stack, op);

        if (op == '(' || op == ')')
        {
            return false;
        }

        postfix += op;
        rang += get_rang(op);
    }

    return true;
}

bool validate(char c)
{
    bool isValid{false};

    if (std::isalpha(c) || c == '(' || c == ')' || c == '^' || c == '+' || c == '-' || c == '*' || c == '/')
    {
        isValid = true;
    }

    return isValid;
}

int get_stack_priority(char c)
{
    int priority{0};

    if (c == '+' || c == '-')
        priority = 2;
    else if (c == '*' || c == '/')
        priority = 4;
    else if (c == '^')
        priority = 5;
    else if (isalpha(c))
        priority = 8;
    else if (c == '(')
        priority = 0;
    else if (c == ' ')
        priority = -1;

    return priority;
}

int get_relative_priority(char c)
{
    int priority{0};

    if (c == '+' || c == '-')
        priority = 1;
    else if (c == '*' || c == '/')
        priority = 3;
    else if (c == '^')
        priority = 6;
    else if (isalpha(c))
        priority = 7;
    else if (c == '(')
        priority = 9;
    else if (c == ')')
        priority = 0;

    return priority;
}

int get_rang(char c)
{
    int priority{0};

    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^')
        priority = -1;
    else if (isalpha(c))
        priority = 1;

    return priority;
}
