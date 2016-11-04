#pragma once

#include "Utils/JsonValue.hpp"

class JsonWriter
{
 public:
    JsonWriter();
    ~JsonWriter();

    void    write(const std::string& fileName, JsonValue& json);
};