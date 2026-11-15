#include <glad/glad.h>
#include "Mesh.h"
#include <vector>
#include <cmath>

namespace
{
    const Mesh::Taper TAPERS[Mesh::ShapeCount] = {
        { 0.00f, 0.00f, 0.00f },   // Box
        { 0.00f, 0.00f, 0.00f },   // Cylinder 
        { 0.06f, 0.03f, 0.02f },   // Body
        { 0.12f, 0.32f, 0.20f },   // CabinSedan
        { 0.10f, 0.30f, 0.06f },   // CabinHatch
        { 0.08f, 0.20f, 0.07f },   // CabinSUV
    };

    unsigned int vaos[Mesh::ShapeCount] = {};
    unsigned int vbos[Mesh::ShapeCount] = {};
    int counts[Mesh::ShapeCount] = {};

    struct Builder
    {
        std::vector<float> d;
        void tri(Vec3 a, Vec3 b, Vec3 c)
        {
            Vec3 n = normalize(cross(b - a, c - a));
            Vec3 p[3] = { a, b, c };
            for (int i = 0; i < 3; i++)
            {
                d.push_back(p[i].x); d.push_back(p[i].y); d.push_back(p[i].z);
                d.push_back(n.x);    d.push_back(n.y);    d.push_back(n.z);
            }
        }
        void quad(Vec3 a, Vec3 b, Vec3 c, Vec3 e) { tri(a, b, c); tri(a, c, e); }
    };

    void frustum(Builder& b, float ix, float fz, float bz, bool bottom)
    {
        float tw = 0.5f - ix, zf = -0.5f + fz, zb = 0.5f - bz;
        Vec3 FL0(-0.5f, -0.5f, -0.5f), FR0(0.5f, -0.5f, -0.5f), BR0(0.5f, -0.5f, 0.5f), BL0(-0.5f, -0.5f, 0.5f);
        Vec3 FL1(-tw, 0.5f, zf), FR1(tw, 0.5f, zf), BR1(tw, 0.5f, zb), BL1(-tw, 0.5f, zb);
        b.quad(FL1, BL1, BR1, FR1);   
        b.quad(FR0, FL0, FL1, FR1);   
        b.quad(BL0, BR0, BR1, BL1);   
        b.quad(FL0, BL0, BL1, FL1);   
        b.quad(BR0, FR0, FR1, BR1);   
        if (bottom) b.quad(FL0, FR0, BR0, BL0);
    }

    void cylinder(Builder& b, int n)
    {
        const float R = 0.5f, PI2 = 6.2831853f;
        for (int i = 0; i < n; i++)
        {
            float a0 = PI2 * i / n, a1 = PI2 * (i + 1) / n;
            Vec3 m0(-0.5f, R * std::cos(a0), R * std::sin(a0)), m1(-0.5f, R * std::cos(a1), R * std::sin(a1));
            Vec3 p0(0.5f, R * std::cos(a0), R * std::sin(a0)), p1(0.5f, R * std::cos(a1), R * std::sin(a1));
            b.quad(m0, m1, p1, p0);                   
            b.tri(Vec3(0.5f, 0, 0), p0, p1);          
            b.tri(Vec3(-0.5f, 0, 0), m1, m0);         
        }
    }

    void upload(int i, const std::vector<float>& d)
    {
        glGenVertexArrays(1, &vaos[i]);
        glGenBuffers(1, &vbos[i]);
        glBindVertexArray(vaos[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
        glBufferData(GL_ARRAY_BUFFER, d.size() * sizeof(float), d.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        counts[i] = (int)(d.size() / 6);
    }
}

Mesh::Taper Mesh::taper(Shape s) { return TAPERS[s]; }

void Mesh::init()
{
    for (int i = 0; i < ShapeCount; i++)
    {
        Builder b;
        if (i == Cylinder) cylinder(b, 24);
        else frustum(b, TAPERS[i].ix, TAPERS[i].fz, TAPERS[i].bz, i == Box);
        upload(i, b.d);
    }
}

void Mesh::cleanup()
{
    for (int i = 0; i < ShapeCount; i++)
    {
        glDeleteVertexArrays(1, &vaos[i]);
        glDeleteBuffers(1, &vbos[i]);
    }
}

void Mesh::draw(const Shader& shader, Shape shape, const Mat4& model, Vec3 color, float emissive)
{
    shader.setMat4("model", model.ptr());
    shader.setVec3("objectColor", color.x, color.y, color.z);
    shader.setFloat("emissive", emissive);
    glBindVertexArray(vaos[shape]);
    glDrawArrays(GL_TRIANGLES, 0, counts[shape]);
}

void Mesh::drawBox(const Shader& shader, const Mat4& model, Vec3 color, float emissive)
{
    draw(shader, Box, model, color, emissive);
}
