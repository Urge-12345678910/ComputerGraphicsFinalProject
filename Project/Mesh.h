#pragma once
#include "Shader.h"
#include "Math.h"

namespace Mesh
{
    enum Shape { Box = 0, Cylinder, Body, CabinSedan, CabinHatch, CabinSUV, ShapeCount };

    struct Taper { float ix, fz, bz; };
    Taper taper(Shape s);

    void init();
    void cleanup();
    void draw(const Shader& shader, Shape shape, const Mat4& model, Vec3 color, float emissive = 0.0f);
    void drawBox(const Shader& shader, const Mat4& model, Vec3 color, float emissive = 0.0f);
}