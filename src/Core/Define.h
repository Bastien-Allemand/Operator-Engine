#pragma once

#include <DirectXMath.h>
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <unordered_map>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

inline constexpr uint32 MAX_VALUE = 4294967295u;

using float64 = double;

using Vector2i = DirectX::XMINT2;
using Vector3i = DirectX::XMINT3;   

using Vector2ui = DirectX::XMUINT2;
using Vector3ui = DirectX::XMUINT3;

using Vector2f = DirectX::XMFLOAT2;

using Vector3f = DirectX::XMFLOAT3;

//Operator for Vector3f
inline Vector3f operator-(const Vector3f& _v)
{
    return Vector3f(-_v.x, -_v.y, -_v.z);
}
inline Vector3f operator-(const Vector3f& _a, const Vector3f& _b)
{
    return Vector3f(_a.x - _b.x, _a.y - _b.y, _a.z - _b.z);
}
inline Vector3f operator+(const Vector3f& _a, const Vector3f& _b)
{
    return Vector3f(_a.x + _b.x, _a.y + _b.y, _a.z + _b.z);
}
inline Vector3f operator*(const Vector3f& _a, const Vector3f& _b)
{
    return Vector3f(_a.x * _b.x, _a.y * _b.y, _a.z * _b.z);
}
inline Vector3f operator/(const Vector3f& _a, const Vector3f& _b)
{
    return Vector3f(_a.x / _b.x, _a.y / _b.y, _a.z / _b.z);
}

inline Vector3f operator-(const Vector3f& _a, float _scalar)
{
    return Vector3f(_a.x - _scalar, _a.y - _scalar, _a.z - _scalar);
}
inline Vector3f operator+(const Vector3f& _a, float _scalar)
{
    return Vector3f(_a.x + _scalar, _a.y + _scalar, _a.z + _scalar);
}
inline Vector3f operator*(const Vector3f& _a, float _scalar)
{
    return Vector3f(_a.x * _scalar, _a.y * _scalar, _a.z * _scalar);
}
inline Vector3f operator/(const Vector3f& _a, float _scalar)
{
    return Vector3f(_a.x / _scalar, _a.y / _scalar, _a.z / _scalar);
}

inline Vector3f operator+=(Vector3f& _a, const Vector3f& _b)
{
    _a.x += _b.x;
    _a.y += _b.y;
    _a.z += _b.z;
    return _a;
}
inline Vector3f operator-=(Vector3f& _a, const Vector3f& _b)
{
    _a.x -= _b.x;
    _a.y -= _b.y;
    _a.z -= _b.z;
    return _a;
}

inline bool operator==(const Vector3f& _a, const Vector3f& _b)
{
    return (_a.x == _b.x) && (_a.y == _b.y) && (_a.z == _b.z);
}
inline bool operator!=(const Vector3f& _a, const Vector3f& _b)
{
    return !(_a == _b);
}
inline bool operator<(const Vector3f& _a, const Vector3f& _b)
{
    if (_a.x != _b.x) return _a.x < _b.x;
    if (_a.y != _b.y) return _a.y < _b.y;
    return _a.z < _b.z;
}
inline bool operator>(const Vector3f& _a, const Vector3f& _b)
{
    if (_a.x != _b.x) return _a.x > _b.x;
    if (_a.y != _b.y) return _a.y > _b.y;
    return _a.z > _b.z;
}

inline bool operator<(const Vector3f& _a, const float& _b)
{
    if (_a.x != _b) return _a.x < _b;
    if (_a.y != _b) return _a.y < _b;
    return _a.z < _b;
}
inline bool operator>(const Vector3f& _a, const float& _b)
{
    if (_a.x != _b) return _a.x > _b;
    if (_a.y != _b) return _a.y > _b;
    return _a.z > _b;
}

// Normalize a Vector3f
inline Vector3f Normalize(const Vector3f& _v)
{
    DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&_v);        // load XMFLOAT3 into XMVECTOR
    vec = DirectX::XMVector3Normalize(vec);                  // normalize
    Vector3f result;
    DirectX::XMStoreFloat3(&result, vec);                    // store back
    return result;
}

// Dot product of two Vector3f
inline float Dot(const Vector3f& _a, const Vector3f& _b)
{
    DirectX::XMVECTOR va = DirectX::XMLoadFloat3(&_a);
    DirectX::XMVECTOR vb = DirectX::XMLoadFloat3(&_b);
    return DirectX::XMVectorGetX(DirectX::XMVector3Dot(va, vb));  // extract float
}
// Cross product of two Vector3f
inline Vector3f Cross(const Vector3f& _a, const Vector3f& _b)
{
    DirectX::XMVECTOR va = DirectX::XMLoadFloat3(&_a);
    DirectX::XMVECTOR vb = DirectX::XMLoadFloat3(&_b);
    DirectX::XMVECTOR cross = DirectX::XMVector3Cross(va, vb);
    Vector3f result;
    DirectX::XMStoreFloat3(&result, cross);
    return result;
}

// Length / magnitude of a Vector3f
inline float Length(const Vector3f& _v)
{
    DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&_v);
    return DirectX::XMVectorGetX(DirectX::XMVector3Length(vec));
}

using Vector4f = DirectX::XMFLOAT4;

inline Vector4f operator-(const Vector4f& _v)
{
    return Vector4f(-_v.x, -_v.y, -_v.z, -_v.w);
}
inline Vector4f operator-(const Vector4f& _a, const Vector4f& _b)
{
    return Vector4f(_a.x - _b.x, _a.y - _b.y, _a.z - _b.z, _a.w - _b.w);
}
inline Vector4f operator*(const Vector4f& _a, const Vector4f& _b)
{
    return Vector4f(_a.x * _b.x, _a.y * _b.y, _a.z * _b.z, _a.w * _b.w);
}

using Quaternion = DirectX::XMFLOAT4;
using Matrix4x4f = DirectX::XMFLOAT4X4;

using Matrix = DirectX::XMMATRIX;
using XVector = DirectX::XMVECTOR;

using String = std::string;
using WString = std::wstring;

template<typename T>
using Vector = std::vector<T>;

template<typename Key, typename Value>
using UnOrderedMap = std::unordered_map<Key, Value>;

inline Vector3f VEC3_RIGHT = { 1.0f, 0.0f, 0.0f };
inline Vector3f VEC3_UP = { 0.0f, 1.0f, 0.0f };
inline Vector3f VEC3_FOWARD = { 0.0f, 0.0f, 1.0f };
inline Vector3f VEC3_ZERO = { 0.0f, 0.0f, 0.0f };
inline Vector3f VEC3_ONE = { 1.0f, 1.0f, 1.0f };
inline Quaternion QUAT_ZERO = { 0.0f, 0.0f, 0.0f, 0.0f };
inline Quaternion QUAT_IDENTITY = { 0.0f, 0.0f, 0.0f, 1.0f };

inline Matrix4x4f MATRIX_IDENTITY = 
{ 1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f };

enum class DebugFlag {
    LOG,  
    WARNING, 
    ERROR_   
};

#define DebugMsg(_msg, _flag) \
    do { \
        if (_flag == DebugFlag::ERROR_) { \
            std::wcerr << L"[CRITICAL ERROR] " << _msg << std::endl; \
            std::abort(); \
        } else { \
            std::wcout << L"[LOG] " << _msg << std::endl; \
        } \
    } while(0)

#define Log(_condition, _message)                                                                          \
    do {                                                                                                 \
        if (_condition) {                                                                                 \
            std::cerr << RED << BOLD << "[ERROR] " << RESET << RED << _message << RESET << std::endl;    \
        } else {                                                                                         \
            std::cout << GREEN << BOLD << "[Valid] " << RESET << GREEN << _message << RESET << std::endl; \
        }                                                                                                \
    } while (0);

#define _Log(_message) \
    do { \
        std::cout << CYAN << BOLD << "[Log] " << RESET << CYAN << _message << RESET << std::endl; \
    } while (0);
