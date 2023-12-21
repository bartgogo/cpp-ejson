#include "ejson.hpp"

int main() {
    system("chcp 65001");
    std::string jsonString = R"({
        "name": "John Doe",
        "age": 30,
        "isStudent": true,
        "scores": [[95, 87, 92],[111,222]]
        "address": {
            "street": "123 \t \" Main St",
            "city": "New York"
        }
    })";

    JSONParser parser;
    auto pre_ = std::chrono::high_resolution_clock().now();
    try {
        JSONValue jsonValue = parser.parsejson(jsonString);
        auto last = std::chrono::high_resolution_clock().now();
        std::cout<<"lasting time="<<std::chrono::duration_cast<std::chrono::microseconds>(last-pre_)<<"\n";
        jsonValue.printJSONMap();
    }
    catch(std::exception a)
    {
        std::cout<<a.what();
    }



    return 0;
}
