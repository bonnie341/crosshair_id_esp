#pragma once
#include "csgo.h"

#define RAD2DEG(x) ((float)(x) * (float)(180.f / 3.14159265358979323846f))
#define DEG2RAD(x) ((float)(x) * (float)(3.14159265358979323846f / 180.f))

namespace math {

    float dot(const vec3& v1, const vec3& v2);
    float length(const vec3& v);
    void clamp_angles(vec3& v);
    void vector_angles(vec3& forward, vec3& angles);
    void sincos(float radians, float* sine, float* cosine);
    void angle_vec(vec3 angles, vec3* forward);
    float vec_dot(vec3 v0, vec3 v1);
    float vec_length(vec3 v);
    vec3 vec_sub(vec3 p0, vec3 p1);
    float vec_length_sqrt(vec3 p0);
    vec3 vec_delta(vec3 p0, vec3 p1);
    float vec_distance(vec3 p0, vec3 p1);
    void vec_clamp(vec3* v);
    void vec_normalize(vec3* vec);
    vec3 vec_transform(vec3 p0, matrix3x4_t p1);
    vec3 vec_atd(vec3 vangle);
    bool vec_min_max(vec3 eye, vec3 dir, vec3 min, vec3 max, float radius);
    void vec_angles(vec3 forward, vec3* angles);
    float get_fov(vec3 vangle, vec3 angle);
}