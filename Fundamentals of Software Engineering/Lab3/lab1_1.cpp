#include <iostream>
#include <array>
#include <vector>
#include <string>

int main()
{
    std::array<std::string, 5> arr = {"Toyota Camry", 
                                      "Ford Focus", 
                                      "BMW X5",
                                      "Lada Vesta", 
                                      "Mercedes C200"};

    std::vector<std::string> cars(arr.begin(), arr.end());

    

    return 0;
}