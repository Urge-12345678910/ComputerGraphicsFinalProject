#pragma once

#include <string>

class Shader
{
public:
    unsigned int ID = 0;

    Shader() = default;
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    bool load(const std::string& vertexPath, const std::string& fragmentPath);
    bool isValid() const { return ID != 0; }
    void use() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec3(const std::string& name, const float* v) const;
    void setMat4(const std::string& name, const float* m) const;

private:
    static bool readFile(const std::string& path, std::string& out);
    static unsigned int compile(unsigned int type, const std::string& src, const std::string& label);
    int location(const std::string& name) const;
};