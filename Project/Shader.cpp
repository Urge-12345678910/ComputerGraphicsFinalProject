#include <glad/glad.h>
#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    load(vertexPath, fragmentPath);
}

Shader::~Shader()
{
    if (ID != 0) glDeleteProgram(ID);
}

bool Shader::readFile(const std::string& path, std::string& out)
{
    std::ifstream file;
    std::string prefix;
    for (int up = 0; up < 5; up++, prefix += "../")
    {
        file.clear();
        file.open(prefix + path);
        if (file.is_open()) break;
    }
    if (!file.is_open())
    {
        std::cerr << "[SHADER ERROR] Cannot open file: " << path << std::endl;
        return false;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    out = ss.str();
    return true;
}

unsigned int Shader::compile(unsigned int type, const std::string& src, const std::string& label)
{
    unsigned int shader = glCreateShader(type);
    const char* code = src.c_str();
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "[SHADER ERROR] Compile failed (" << label << "):\n" << log << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Shader::load(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vertSrc, fragSrc;
    if (!readFile(vertexPath, vertSrc) || !readFile(fragmentPath, fragSrc))
        return false;

    unsigned int vs = compile(GL_VERTEX_SHADER, vertSrc, vertexPath);
    unsigned int fs = compile(GL_FRAGMENT_SHADER, fragSrc, fragmentPath);
    if (vs == 0 || fs == 0)
    {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return false;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    int success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[1024];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "[SHADER ERROR] Link failed:\n" << log << std::endl;
        glDeleteProgram(program);
        return false;
    }

    if (ID != 0) glDeleteProgram(ID);
    ID = program;
    return true;
}

void Shader::use() const { glUseProgram(ID); }

int Shader::location(const std::string& name) const
{
    return glGetUniformLocation(ID, name.c_str());
}

void Shader::setBool(const std::string& n, bool v) const { glUniform1i(location(n), (int)v); }
void Shader::setInt(const std::string& n, int v) const { glUniform1i(location(n), v); }
void Shader::setFloat(const std::string& n, float v) const { glUniform1f(location(n), v); }
void Shader::setVec3(const std::string& n, float x, float y, float z) const { glUniform3f(location(n), x, y, z); }
void Shader::setVec3(const std::string& n, const float* v) const { glUniform3fv(location(n), 1, v); }
void Shader::setMat4(const std::string& n, const float* m) const { glUniformMatrix4fv(location(n), 1, GL_FALSE, m); }