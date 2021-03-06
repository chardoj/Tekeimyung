/**
* @Author   Guillaume Labey
*/

#include <Engine/Debug/Logger.hpp>
#include <Engine/Utils/JsonValue.hpp>

JsonValue::JsonValue() {}

JsonValue::JsonValue(const Json::Value& json) : _json(json) {}

JsonValue::JsonValue(const JsonValue& other)
{
    _json = other.get();
}

JsonValue::~JsonValue() {}

JsonValue&  JsonValue::operator=(const JsonValue& other)
{
    _json = other.get();
    return (*this);
}

JsonValue   JsonValue::get(const std::string& index, const Json::Value& defaultValue) const
{
    return JsonValue(_json.get(index, defaultValue));
}

Json::Value     JsonValue::get() const
{
    return _json;
}

bool    JsonValue::getBool(const std::string& index, bool defaultValue) const
{
    return _json.get(index, defaultValue).asBool();
}

int JsonValue::getInt(const std::string& index, int defaultValue) const
{
    return _json.get(index, defaultValue).asInt();
}

unsigned int    JsonValue::getUInt(const std::string& index, unsigned int defaultValue) const
{
    return _json.get(index, defaultValue).asUInt();
}

float   JsonValue::getFloat(const std::string& index, float defaultValue) const
{
    return _json.get(index, defaultValue).asFloat();
}

std::string JsonValue::getString(const std::string& index, const std::string& defaultValue) const
{
    return _json.get(index, defaultValue).asString();
}

std::vector<std::string>    JsonValue::getStringVec(const std::string& index, const std::vector<std::string>& defaultValue) const
{
    auto& stringVecJson = _json[index];
    if (stringVecJson.size() > 0 && stringVecJson.type() != Json::ValueType::arrayValue)
    {
        LOG_WARN("JsonValue::getStringVec error: field %s is not an array", index.c_str());
        return (defaultValue);
    }

    std::vector<std::string> stringVec;
    for (const auto& string_ : stringVecJson)
    {
        stringVec.push_back(string_.asString());
    }

    return (stringVec);
}

glm::vec3   JsonValue::getColor3f(const std::string& index, const glm::vec3& defaultValue) const
{
    JsonValue color = get(index, {});

    return{ color.getFloat("r", defaultValue.x), color.getFloat("g", defaultValue.y),
        color.getFloat("b", defaultValue.z) };
}

glm::vec4   JsonValue::getColor4f(const std::string& index, const glm::vec4& defaultValue) const
{
    JsonValue color = get(index, {});

    return{ color.getFloat("r", defaultValue.x), color.getFloat("g", defaultValue.y),
        color.getFloat("b", defaultValue.z), color.getFloat("a", defaultValue.w) };
}


glm::vec2   JsonValue::getVec2f(const std::string& index, const glm::vec2& defaultValue) const
{
    JsonValue vec = get(index, {});

    return{ vec.getFloat("x", defaultValue.x), vec.getFloat("y", defaultValue.y) };
}

glm::vec3   JsonValue::getVec3f(const std::string& index, const glm::vec3& defaultValue) const
{
    JsonValue vec = get(index, {});

    return{ vec.getFloat("x", defaultValue.x), vec.getFloat("y", defaultValue.y),
    vec.getFloat("z", defaultValue.y) };
}

glm::vec4   JsonValue::getVec4f(const std::string& index, const glm::vec4& defaultValue) const
{
    JsonValue vec = get(index, {});

    return{ vec.getFloat("x", defaultValue.x), vec.getFloat("y", defaultValue.y),
    vec.getFloat("z", defaultValue.y), vec.getFloat("w", defaultValue.y) };
}

glm::uvec2  JsonValue::getUVec2f(const std::string& index, const glm::uvec2& defaultValue) const
{
    JsonValue vec = get(index, {});

    return{ vec.getUInt("x", defaultValue.x), vec.getUInt("y", defaultValue.y) };
}

glm::uvec3  JsonValue::getUVec3f(const std::string& index, const glm::uvec3& defaultValue) const
{
    JsonValue vec = get(index, {});

    return{ vec.getUInt("x", defaultValue.x), vec.getUInt("y", defaultValue.y),
    vec.getUInt("z", defaultValue.y) };
}

glm::uvec4  JsonValue::getUVec4f(const std::string& index, const glm::uvec4& defaultValue) const
{
    JsonValue vec = get(index, {});

    return{ vec.getUInt("x", defaultValue.x), vec.getUInt("y", defaultValue.y),
    vec.getUInt("z", defaultValue.y), vec.getUInt("w", defaultValue.y) };
}

glm::vec2   JsonValue::getVec2f(const std::string& index, const std::array<const char*, 2>& keys, const glm::vec2& defaultValue) const
{
    JsonValue vec = get(index, {});

    return{ vec.getFloat(keys[0], defaultValue.x), vec.getFloat(keys[1], defaultValue.y) };
}

glm::vec3   JsonValue::getVec3f(const std::string& index, const std::array<const char*, 3>& keys, const glm::vec3& defaultValue) const
{
    JsonValue vec = get(index, {});

    return{ vec.getFloat(keys[0], defaultValue.x), vec.getFloat(keys[1], defaultValue.y),
    vec.getFloat(keys[2], defaultValue.z) };
}

glm::vec4   JsonValue::getVec4f(const std::string& index, const std::array<const char*, 4>& keys, const glm::vec4& defaultValue) const
{
    JsonValue vec = get(index, {});

    return{ vec.getFloat(keys[0], defaultValue.x), vec.getFloat(keys[1], defaultValue.y),
    vec.getFloat(keys[2], defaultValue.z), vec.getFloat(keys[3], defaultValue.w) };
}

void    JsonValue::setValue(const std::string& index, const JsonValue& value)
{
    _json[index] = value.get();
}

void    JsonValue::setValueVec(const std::string& index, const std::vector<JsonValue>& value)
{
    if (value.size() == 0)
        _json[index] = Json::Value(Json::arrayValue);

    for (uint32_t i = 0; i < value.size(); i++)
    {
        _json[index][i] = value[i].get();
    }
}

void    JsonValue::setBool(const std::string& index, bool value)
{
    _json[index] = value;
}

void JsonValue::setInt(const std::string& index, int value)
{
    _json[index] = value;
}

void    JsonValue::setUInt(const std::string& index, unsigned int value)
{
    _json[index] = value;
}

void   JsonValue::setFloat(const std::string& index, float value)
{
    _json[index] = value;
}

void JsonValue::setString(const std::string& index, const std::string& value)
{
    _json[index] = value;
}

void    JsonValue::setStringVec(const std::string& index, const std::vector<std::string>& value)
{
    if (value.size() == 0)
        _json[index] = Json::Value(Json::arrayValue);

    for (uint32_t i = 0; i < value.size(); i++)
    {
        _json[index][i] = value[i];
    }
}

void   JsonValue::setColor3f(const std::string& index, const glm::vec3& value)
{
    Json::Value& color = _json[index];

    color["r"] = value.x;
    color["g"] = value.y;
    color["b"] = value.z;
}

void   JsonValue::setColor4f(const std::string& index, const glm::vec4& value)
{
    Json::Value& color = _json[index];

    color["r"] = value.x;
    color["g"] = value.y;
    color["b"] = value.z;
    color["a"] = value.w;
}


void   JsonValue::setVec2f(const std::string& index, const glm::vec2& value)
{
    Json::Value& vec = _json[index];

    vec["x"] = value.x;
    vec["y"] = value.y;
}

void   JsonValue::setVec3f(const std::string& index, const glm::vec3& value)
{
    Json::Value& vec = _json[index];

    vec["x"] = value.x;
    vec["y"] = value.y;
    vec["z"] = value.z;
}

void   JsonValue::setVec4f(const std::string& index, const glm::vec4& value)
{
    Json::Value& vec = _json[index];

    vec["x"] = value.x;
    vec["y"] = value.y;
    vec["z"] = value.z;
    vec["w"] = value.w;
}

void    JsonValue::setUVec2f(const std::string& index, const glm::uvec2& value)
{
    Json::Value& vec = _json[index];

    vec["x"] = value.x;
    vec["y"] = value.y;
}

void    JsonValue::setUVec3f(const std::string& index, const glm::uvec3& value)
{
    Json::Value& vec = _json[index];

    vec["x"] = value.x;
    vec["y"] = value.y;
    vec["z"] = value.z;
}

void    JsonValue::setUVec4f(const std::string& index, const glm::uvec4& value)
{
    Json::Value& vec = _json[index];

    vec["x"] = value.x;
    vec["y"] = value.y;
    vec["z"] = value.z;
    vec["w"] = value.w;
}

void   JsonValue::setVec2f(const std::string& index, const std::array<const char*, 2>& keys, const glm::vec2& value)
{
    Json::Value& vec = _json[index];

    vec[keys[0]] = value.x;
    vec[keys[1]] = value.y;
}

void   JsonValue::setVec3f(const std::string& index, const std::array<const char*, 3>& keys, const glm::vec3& value)
{
    Json::Value& vec = _json[index];

    vec[keys[0]] = value.x;
    vec[keys[1]] = value.y;
    vec[keys[2]] = value.z;
}

void   JsonValue::setVec4f(const std::string& index, const std::array<const char*, 4>& keys, const glm::vec4& value)
{
    Json::Value& vec = _json[index];

    vec[keys[0]] = value.x;
    vec[keys[1]] = value.y;
    vec[keys[2]] = value.z;
    vec[keys[3]] = value.w;
}

uint32_t    JsonValue::size() const
{
    return _json.size();
}
