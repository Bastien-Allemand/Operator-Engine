#include "pch.h"
#include "Geometry.h"
#include <DirectXColors.h>

void Geometry::BuildBox(uint32 _width, uint32 _height, uint32 _length, Vector4f _color)
{
    vertices = nullptr;
    indices = nullptr;

    vertices = new Vertex[24];
    indices = new uint32[36];

    float w = (float)_width;
    float h = (float)_height;
    float l = (float)_length;

    // On définit les 24 sommets (4 par face)
    // Position, Normale, UV, Couleur
    Vertex v[24] = {
        // Face Avant (Z+) : Normale (0,0,1)
        { {-w, -h,  -l}, {0,0,1}, {0,1},_color }, // 0: Bas-Gauche
        { {-w,  h,  -l}, {0,0,1}, {0,0},_color }, // 1: Haut-Gauche
        { { w,  h,  -l}, {0,0,1}, {1,0},_color }, // 2: Haut-Droite
        { { w, -h,  -l}, {0,0,1}, {1,1},_color }, // 3: Bas-Droite

        // Face Arrière (Z-) : Normale (0,0,-1)
        { { w, -h, l}, {0,0,-1}, {0,1},_color }, // 4
        { { w,  h, l}, {0,0,-1}, {0,0},_color }, // 5
        { {-w,  h, l}, {0,0,-1}, {1,0},_color }, // 6
        { {-w, -h, l}, {0,0,-1}, {1,1},_color }, // 7

        // Face Haut (Y+) : Normale (0,1,0)
        { {-w,  -h,  l}, {0,1,0}, {0,1},_color }, // 8
        { {-w,  -h, -l}, {0,1,0}, {0,0},_color }, // 9
        { { w,  -h, -l}, {0,1,0}, {1,0},_color }, // 10
        { { w,  -h,  l}, {0,1,0}, {1,1},_color }, // 11

        // Face Bas (Y-) : Normale (0,-1,0)
        { {-w, h, -l}, {0,-1,0}, {0,1},_color }, // 12
        { {-w, h,  l}, {0,-1,0}, {0,0},_color }, // 13
        { { w, h,  l}, {0,-1,0}, {1,0},_color }, // 14
        { { w, h, -l}, {0,-1,0}, {1,1},_color }, // 15

        // Face Gauche (X-) : Normale (-1,0,0)
        { {w, -h, -l}, {-1,0,0}, {0,1},_color }, // 16
        { {w,  h, -l}, {-1,0,0}, {0,0},_color }, // 17
        { {w,  h,  l}, {-1,0,0}, {1,0},_color }, // 18
        { {w, -h,  l}, {-1,0,0}, {1,1},_color }, // 19

        // Face Droite (X+) : Normale (1,0,0)
        { { -w, -h,  l}, {1,0,0}, {0,1},_color }, // 20
        { { -w,  h,  l}, {1,0,0}, {0,0},_color }, // 21
        { { -w,  h, -l}, {1,0,0}, {1,0},_color }, // 22
        { { -w, -h, -l}, {1,0,0}, {1,1},_color }  // 23
    };

    for (int i = 0; i < 24; ++i) vertices[i] = (v[i]);

    uint32 idx = 0; // compteur global pour les indices

    for (uint32 face = 0; face < 6; ++face)
    {
        uint32 b = face * 4; // premier vertex de la face

        indices[idx++] = b;
        indices[idx++] = b + 1;
        indices[idx++] = b + 2;

        indices[idx++] = b;
        indices[idx++] = b + 2;
        indices[idx++] = b + 3;
    }

	geoBounds = DirectX::BoundingSphere({ 0,0,0 }, sqrtf(w * w + h * h + l * l));

	vertexCount = 24;
	indexCount = idx;
}

void Geometry::BuildSphere(uint32 _sliceCount, uint32 _stackCount,float _radius, Vector4f _color)
{
    vertices = new Vertex[(_stackCount + 1) * (_sliceCount + 1)];
    indices = new uint32[_stackCount * _sliceCount * 6];

    for (uint32 stack = 0; stack <= _stackCount; ++stack)
    {
        float phi = DirectX::XM_PI * stack / _stackCount;
        float y = cosf(phi);
        float r = sinf(phi);

        for (uint32 slice = 0; slice <= _sliceCount; ++slice)
        {
            float theta = 2.0f * DirectX::XM_PI * slice / _sliceCount;
            float x = r * cosf(theta);
            float z = r * sinf(theta);

            Vector3f normal = { x, y, z };
            DirectX::XMStoreFloat3(&normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&normal)));

            uint32 idx = stack * (_sliceCount + 1) + slice;
            vertices[idx] = { {-x * _radius, -y * _radius, -z * _radius}, normal, {theta / (2 * DirectX::XM_PI), phi / DirectX::XM_PI}, _color };
        }
    }
	vertexCount = (_stackCount + 1) * (_sliceCount + 1);
    uint32 idx = 0;
    for (uint32 stack = 0; stack < _stackCount; ++stack)
    {
        for (uint32 slice = 0; slice < _sliceCount; ++slice)
        {
            uint32 first = stack * (_sliceCount + 1) + slice;
            uint32 second = first + _sliceCount + 1;

            indices[idx++] = first;
            indices[idx++] = second;
            indices[idx++] = first + 1;

            indices[idx++] = second;
            indices[idx++] = second + 1;
            indices[idx++] = first + 1;
        }
    }

	geoBounds = DirectX::BoundingSphere({ 0,0,0 }, _radius);

	indexCount = idx;
}

void Geometry::BuildPlane(uint32 _width, uint32 _length, Vector4f _color)
{
    // Allocation directe pour 4 vertices et 6 indices
    vertices = new Vertex[4];
    indices = new uint32[6];

    float w = static_cast<float>(_width);
    float l = static_cast<float>(_length);

    // Définition des 4 sommets
    Vertex v[4] =
    {
        { {-w, 0.0f, -l}, {0,-1,0}, {0,1}, _color }, // Bas-Gauche
        { {-w, 0.0f,  l}, {0,-1,0}, {0,0}, _color }, // Haut-Gauche
        { { w, 0.0f,  l}, {0,-1,0}, {1,0}, _color }, // Haut-Droite
        { { w, 0.0f, -l}, {0,-1,0}, {1,1}, _color }  // Bas-Droite
    };

    // Copier les vertices dans le tableau alloué
    for (size_t i = 0; i < 4; i++)
        vertices[i] = v[i];

    // Définir les indices pour deux triangles (sens horaire)
    uint32 idx = 0;
    uint32 quadStart = 0; // début du quad
    indices[idx++] = quadStart;
    indices[idx++] = quadStart + 1;
    indices[idx++] = quadStart + 2;

    indices[idx++] = quadStart;
    indices[idx++] = quadStart + 2;
    indices[idx++] = quadStart + 3;

	geoBounds = DirectX::BoundingSphere({ 0,0,0 }, sqrtf(w * w + l * l));

	vertexCount = 4;
	indexCount = idx;
}

void Geometry::BuildCylinder(uint32 _sliceCount, uint32 _height, Vector4f _color)
{
    // 2 cercles (bas et haut) × _sliceCount vertices
    vertexCount = _sliceCount * 2;
    indexCount = _sliceCount * 6; // 2 triangles par quad latéral

    vertices = new Vertex[vertexCount];
    indices = new uint32[indexCount];

    float halfHeight = _height * 0.5f;

    // Remplissage des vertices (bas et haut)
    for (uint32 slice = 0; slice < _sliceCount; ++slice)
    {
        float theta = 2.0f * DirectX::XM_PI * slice / _sliceCount;
        float x = cosf(theta);
        float z = sinf(theta);

        // Vertex bas
        vertices[slice] = { {x, -halfHeight, z}, {-x,0,-z}, {slice / static_cast<float>(_sliceCount),0}, _color };
        // Vertex haut
        vertices[slice + _sliceCount] = { {x, halfHeight, z}, {-x,0,-z}, {slice / static_cast<float>(_sliceCount),1}, _color };
    }

    // Remplissage des indices pour les quads latéraux
    uint32 idx = 0;
    for (uint32 slice = 0; slice < _sliceCount; ++slice)
    {
        uint32 next = (slice + 1) % _sliceCount;

        uint32 bottomCurrent = slice;
        uint32 bottomNext = next;
        uint32 topCurrent = slice + _sliceCount;
        uint32 topNext = next + _sliceCount;

        // Premier triangle du quad
        indices[idx++] = bottomCurrent;
        indices[idx++] = topCurrent;
        indices[idx++] = bottomNext;

        // Deuxième triangle du quad
        indices[idx++] = topCurrent;
        indices[idx++] = topNext;
        indices[idx++] = bottomNext;
    }

	geoBounds = DirectX::BoundingSphere({ 0,0,0 }, sqrtf(1.0f + halfHeight * halfHeight)); // Rayon du cylindre englobant
}
