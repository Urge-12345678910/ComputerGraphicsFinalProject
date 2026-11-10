#pragma once
#include <cmath>

struct Vec3
{
    float x = 0, y = 0, z = 0;
    Vec3() = default;
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};
inline Vec3  operator+(Vec3 a, Vec3 b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
inline Vec3  operator-(Vec3 a, Vec3 b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
inline Vec3  operator*(Vec3 a, float s) { return { a.x * s, a.y * s, a.z * s }; }
inline float dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3  cross(Vec3 a, Vec3 b)
{
    return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}
inline Vec3 normalize(Vec3 v)
{
    float l = std::sqrt(dot(v, v));
    return l > 0 ? v * (1.0f / l) : v;
}
inline float radians(float deg) { return deg * 3.14159265358979f / 180.0f; }

struct Mat4
{
    float m[16];
    Mat4() { for (int i = 0; i < 16; i++) m[i] = (i % 5 == 0) ? 1.0f : 0.0f; }
    const float* ptr() const { return m; }
};

inline Mat4 operator*(const Mat4& a, const Mat4& b)
{
    Mat4 r;
    for (int c = 0; c < 4; c++)
        for (int row = 0; row < 4; row++)
        {
            float s = 0;
            for (int k = 0; k < 4; k++) s += a.m[k * 4 + row] * b.m[c * 4 + k];
            r.m[c * 4 + row] = s;
        }
    return r;
}

inline Mat4 translate(Vec3 t)
{
    Mat4 r; r.m[12] = t.x; r.m[13] = t.y; r.m[14] = t.z; return r;
}
inline Mat4 scale(Vec3 s)
{
    Mat4 r; r.m[0] = s.x; r.m[5] = s.y; r.m[10] = s.z; return r;
}
inline Mat4 rotateX(float a)
{
    Mat4 r; float c = std::cos(a), s = std::sin(a);
    r.m[5] = c; r.m[6] = s; r.m[9] = -s; r.m[10] = c; return r;
}
inline Mat4 rotateY(float a)
{
    Mat4 r; float c = std::cos(a), s = std::sin(a);
    r.m[0] = c; r.m[2] = -s; r.m[8] = s; r.m[10] = c; return r;
}
inline Mat4 rotateZ(float a)
{
    Mat4 r; float c = std::cos(a), s = std::sin(a);
    r.m[0] = c; r.m[1] = s; r.m[4] = -s; r.m[5] = c; return r;
}

inline Mat4 perspective(float fovRad, float aspect, float nearP, float farP)
{
    Mat4 r;
    float f = 1.0f / std::tan(fovRad / 2.0f);
    r.m[0] = f / aspect;
    r.m[5] = f;
    r.m[10] = (farP + nearP) / (nearP - farP);
    r.m[11] = -1.0f;
    r.m[14] = 2.0f * farP * nearP / (nearP - farP);
    r.m[15] = 0.0f;
    return r;
}

inline Mat4 lookAt(Vec3 eye, Vec3 center, Vec3 up)
{
    Vec3 f = normalize(center - eye);
    Vec3 s = normalize(cross(f, up));
    Vec3 u = cross(s, f);
    Mat4 r;
    r.m[0] = s.x; r.m[4] = s.y; r.m[8] = s.z;
    r.m[1] = u.x; r.m[5] = u.y; r.m[9] = u.z;
    r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z;
    r.m[12] = -dot(s, eye);
    r.m[13] = -dot(u, eye);
    r.m[14] = dot(f, eye);
    return r;
}
inline float clampf(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }
inline float mixf(float a, float b, float t) { return a + (b - a) * t; }
inline Vec3  mixv(Vec3 a, Vec3 b, float t) { return a + (b - a) * t; }