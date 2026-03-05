#pragma once
#include "Define.h"

struct Particle {
    Vector3f position;
    float    lifeTime;

    Vector3f velocity;
    float    size;    

    Vector4f color;   

    uint32_t type;    
    float    rotation;
    float    age;     
    float    custom;  
};