/**
  ******************************************************************************
  * @file           : ejson.hpp
  * @author         : JackZhao
  * @brief          : None
  * @attention      : None
  * @date           : 2023/12/13
  ******************************************************************************
  */


#ifndef EJSON_EJSON_HPP
#define EJSON_EJSON_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <chrono>
enum class JSONValueType {
    Null,
    Boolean,
    Integer,   // int
    Float,     // double
    String,
    Array,
    Object
};

class JSONValue {
public:
    JSONValue() : type(JSONValueType::Null) {}


    JSONValueType getType() const {
        return type;
    }

    void setType(JSONValueType valueType) {
        type = valueType;
    }

    bool getBoolean() const {
        return booleanValue;
    }

    void setBoolean(bool value) {
        booleanValue = value;
    }

    void setNumber(int value) {
        type = JSONValueType::Integer;
        numberValue.intValue = value;
    }

    void setNumber(double value) {
        type = JSONValueType::Float;
        numberValue.floatValue = value;
    }

    int getInteger() const {
        return numberValue.intValue;
    }

    double getFloat() const {
        return numberValue.floatValue;
    }

    const std::string& getString() const {
        return stringValue;
    }

    void setString(const std::string& value) {
        stringValue = value;
    }

    const std::vector<JSONValue>& getArray() const {
        return arrayValue;
    }

    void setArray(const std::vector<JSONValue>& value) {
        arrayValue = value;
    }

    const std::map<std::string, JSONValue>& getObject() const {
        return objectValue;
    }

    void setObject(const std::map<std::string, JSONValue>& value) {
        objectValue = value;
    }

    std::string getvalue()  const {

        if (this->type == JSONValueType::Null)
            return "";
        if (this->type == JSONValueType::Boolean)
            return booleanValue ? "true" : "false";
        if (this->type == JSONValueType::Integer)
            return std::to_string(numberValue.intValue);
        if (this->type == JSONValueType::Float)
            return std::to_string(numberValue.floatValue);
        if (this->type == JSONValueType::String)
            return stringValue;
        if (this->type == JSONValueType::Array) {
            std::string a;
            for (const auto &i: arrayValue)
            {
                if(a=="")
                    a="["+i.getvalue();
                a = a + "," + i.getvalue();
            }
            a+="]";
            return a;
        }
        if (this->type == JSONValueType::Object) {
            std::string obj;
            for (const auto &pair: objectValue) {
                const std::string &key = pair.first;
                const JSONValue &value = pair.second;
                obj += key + ": " + value.getvalue() + ", ";
            }
            // Remove the trailing comma and space
            if (!obj.empty()) {
                obj = obj.substr(0, obj.length() - 2);
            }
            return "{" + obj + "}";
        }
    }
    std::string serialize() const {
        std::ostringstream oss;

        switch (type) {
            case JSONValueType::Null:
                oss << "null";
                break;
            case JSONValueType::Boolean:
                oss << (booleanValue ? "true" : "false");
                break;
            case JSONValueType::Integer:
                oss << numberValue.intValue;
                break;
            case JSONValueType::Float:
                oss << numberValue.floatValue;
            case JSONValueType::String:
                oss << '"' << stringValue << '"';
                break;
            case JSONValueType::Array:
                oss << '[';
                for (size_t i = 0; i < arrayValue.size(); ++i) {
                    oss << arrayValue[i].serialize();
                    if (i != arrayValue.size() - 1) {
                        oss << ',';
                    }
                }
                oss << ']';
                break;
            case JSONValueType::Object:
                oss << '{';
                for (auto it = objectValue.begin(); it != objectValue.end(); ++it) {
                    oss << '"' << it->first << '"' << ':' << it->second.serialize();
                    if (std::next(it) != objectValue.end()) {
                        oss << ',';
                    }
                }
                oss << '}';
                break;
        }

        return oss.str();
    }
    void printJSONMap() {
        for (const auto& pair : this->getObject()) {
            const std::string& key = pair.first;
            const JSONValue& value = pair.second;

            std::cout << "Key: " << key << ", Value: "<<value.getvalue();

            std::cout << std::endl;
        }
    }



private:
    JSONValueType type;
    bool booleanValue;
    union {
        int intValue;
        double floatValue;
    } numberValue;
    std::string stringValue;
    std::vector<JSONValue> arrayValue;
    std::map<std::string, JSONValue> objectValue;
};

class JSONParser {
public:
    JSONValue parsejson(const std::string& json) {
        position = 0;
        return parseValue(json);
    }
    JSONValue parse(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }

        std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        position = 0;
        //输出字符串std::cout<<json<<std::endl;
        return parseValue(json);
    }


private:
    JSONValue parseValue(const std::string& json) {
        skipWhitespace(json);
        char currentChar = json[position];

        if (currentChar == 'n') {
            position += 4;  // Skip "null"
            return JSONValue();
        }
        else if (currentChar == 't') {
            position += 4;  // Skip "true"
            JSONValue value;
            value.setType(JSONValueType::Boolean);
            value.setBoolean(true);
            return value;
        }
        else if (currentChar == 'f') {
            position += 5;  // Skip "false"
            JSONValue value;
            value.setType(JSONValueType::Boolean);
            value.setBoolean(false);
            return value;
        }
        else if (currentChar == '"') {
            return parseString(json);
        }
        else if (currentChar == '[') {
            return parseArray(json);
        }
        else if (currentChar == '{') {
            return parseObject(json);
        }
        else if(isdigit(json[position]))
        {
            return parseNumber(json);
        } else {
            throw std::logic_error("unexpected character in parse json");
        }
    }

    JSONValue parseString(const std::string& json) {
        std::string value;
        position++;  // Skip opening quote

        while (json[position] != '"') {
            value += json[position++];
        }

        position++;  // Skip closing quote

        JSONValue jsonValue;
        jsonValue.setType(JSONValueType::String);
        jsonValue.setString(value);
        return jsonValue;
		
    }

    JSONValue parseNumber(const std::string& json) {
        std::string value;

        while (isdigit(json[position]) || json[position] == '.') {
            value += json[position++];
        }

            if (value.find('.') != std::string::npos) {
                double number = std::stod(value);

                JSONValue jsonValue;
                //jsonValue.setType(JSONValueType::Float);
                jsonValue.setNumber(number);
                return jsonValue;
            } else {
                int number = std::stoi(value);

                JSONValue jsonValue;
                //jsonValue.setType(JSONValueType::Integer);
                jsonValue.setNumber(number);
                return jsonValue;
            }

    }

    JSONValue parseArray(const std::string& json) {
        position++;  // Skip opening bracket

        std::vector<JSONValue> array;

        while (json[position] != ']') {
            array.push_back(parseValue(json));

            skipWhitespace(json);

            if (json[position] == ',') {
                position++;  // Skip comma
            }
        }

        position++;  // Skip closing bracket

        JSONValue jsonValue;
        jsonValue.setType(JSONValueType::Array);
        jsonValue.setArray(array);
        return jsonValue;
    }

    JSONValue parseObject(const std::string& json) {
        position++;  // Skip opening brace

        std::map<std::string, JSONValue> object;

        while (json[position] != '}') {
            skipWhitespace(json);

            std::string key = parseString(json).getString();

            skipWhitespace(json);

            position++;  // Skip colon

            JSONValue value = parseValue(json);

            object.insert(std::make_pair(key, value));

            skipWhitespace(json);

            if (json[position] == ',') {
                position++;  // Skip comma
            }
        }

        position++;  // Skip closing brace

        JSONValue jsonValue;
        jsonValue.setType(JSONValueType::Object);
        jsonValue.setObject(object);
        return jsonValue;
    }

    void skipWhitespace(const std::string& json) {
        while (isspace(json[position])) {
            position++;
        }
    }

    size_t position;
};



#endif //EJSON_EJSON_HPP
