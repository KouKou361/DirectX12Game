#pragma once
#include <DirectXMath.h>


class VECTOR2 : public DirectX::XMFLOAT2
{
public:
    VECTOR2() : DirectX::XMFLOAT2(0, 0) {}
    VECTOR2(float x, float y) : DirectX::XMFLOAT2(x, y) {}
    VECTOR2(const VECTOR2& v) { x = v.x; y = v.y; }
    ~VECTOR2() {}

    VECTOR2& operator=(const VECTOR2&);
    VECTOR2& operator+=(const VECTOR2&);
    VECTOR2& operator-=(const VECTOR2&);
    VECTOR2& operator*=(float);
    VECTOR2& operator/=(float);

    VECTOR2 operator+() const;
    VECTOR2 operator-() const;

    VECTOR2 operator+(const VECTOR2&) const;
    VECTOR2 operator-(const VECTOR2&) const;
    VECTOR2 operator*(float) const;
    friend VECTOR2 operator*(float, const VECTOR2&);
    VECTOR2 operator/(float) const;

    bool operator == (const VECTOR2&) const;
    bool operator != (const VECTOR2&) const;
};

class VECTOR3 : public DirectX::XMFLOAT3
{
public:
    VECTOR3() : DirectX::XMFLOAT3(0, 0, 0) {}
    VECTOR3(float x, float y, float z) : DirectX::XMFLOAT3(x, y, z) {}
    VECTOR3(const VECTOR3& v) { x = v.x; y = v.y; z = v.z; }
    ~VECTOR3() {}

    VECTOR3& operator=(const VECTOR3&);
    VECTOR3& operator+=(const VECTOR3&);
    VECTOR3& operator-=(const VECTOR3&);
    VECTOR3& operator*=(float);
    VECTOR3& operator/=(float);

    VECTOR3 operator+() const;
    VECTOR3 operator-() const;

    VECTOR3 operator+(const VECTOR3&) const;
    VECTOR3 operator-(const VECTOR3&) const;
    VECTOR3 operator*(float) const;
    friend VECTOR3 operator*(float, const VECTOR3&);
    VECTOR3 operator/(float) const;

    bool operator == (const VECTOR3&) const;
    bool operator != (const VECTOR3&) const;

    VECTOR3(const VECTOR2& v);
    // VECTOR3‚ÉVECTOR2‚ð‘ã“ü
    VECTOR3& operator=(const VECTOR2&);
};

class VECTOR4 : public DirectX::XMFLOAT4
{
public:
    VECTOR4() : DirectX::XMFLOAT4(0, 0, 0, 0) {}
    VECTOR4(float x, float y, float z, float w) : DirectX::XMFLOAT4(x, y, z, w) {}
    ~VECTOR4() {}
};