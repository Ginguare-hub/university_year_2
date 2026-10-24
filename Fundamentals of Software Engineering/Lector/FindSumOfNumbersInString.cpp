#include <iostream>
#include <cctype>
#include <string>
#include <deque>

int main(void)
{
    std::string str;
    int sum{0}, temp{0};

    std::getline(std::cin, str);

    for (char c : str)
    {
        if (isdigit(c))
        {
            temp *= 10;
            temp += c - '0';
        }

        if (!isdigit(c) && temp != 0)
        {
            sum += temp;
            temp = 0;
        }
    }

    if (temp != 0)
    {
        sum += temp;
        temp = 0;
    }

    std::cout << sum;

    return 0;
}


/*
#include <iostream>
#include <cctype>
#include <string>
int main(){std::string str;int sum=0,temp=0;std::getline(std::cin,str);for(char c:str){if(isdigit(c)){temp=temp*10+c-'0';}if(!isdigit(c)&&temp!=0){sum+=temp;temp=0;}}if(temp!=0){sum+=temp;temp=0;}std::cout<<sum;return 0;}
*/