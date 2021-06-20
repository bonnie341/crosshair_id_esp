#pragma once
#include <iostream>
#include <windows.h>

#include "xorstr.hpp"
#define xorstr XorStr

bool cs_initialize(void);

typedef unsigned int csptr_t;

typedef struct {
    char value[120];
} SHORT_STRING;

typedef struct {
    char value[260];
} LONG_STRING;

typedef struct {
    float x, y, z;
} vec3;

typedef struct {
    int x, y;
} vec2i;

typedef float matrix3x4_t[3][4];

class cs_virtual_table {
    csptr_t self;
public:
    csptr_t address(void) const;
    csptr_t function(int index);
};

class cs_convar {
    csptr_t self;
public:
    SHORT_STRING get_name(void);
    void         set_string(const char* v);
    void         set_int(int v);
    void         set_float(float v);
    SHORT_STRING get_string(void);
    int          get_int(void);
    float        get_float(void);
};

class cs_interface_reg {
    csptr_t self;
public:
    cs_virtual_table find(const char* name);
};

class cs_netvar_table {
    csptr_t self;
public:
    uint32_t offset(const char* name);
private:
    uint32_t offset_ex(csptr_t, const char*);
};

class cs_player {
    csptr_t self;
public:
    int     get_team(void);
    int     get_health(void);
    int     get_life_state(void);
    int     get_tick_count(void);
    int     get_shots_fired(void);
    bool    is_scoped(void);
    bool    is_dormant(void);
    csptr_t get_weapon(void);
    vec3    get_origin(void);
    vec3    get_vec_view(void);
    vec3    get_eye_pos(void);
    vec3    get_vec_velocity(void);
    vec3    get_vec_punch(void);
    int     get_fov(void);

    void    bone_matrix(int index, matrix3x4_t* out);

    bool    is_valid(void);
};

namespace inputsystem {
    typedef enum _CS_BUTTONCODE : int {
        MOUSE_1 = 107,
        MOUSE_2 = 108,
        MOUSE_3 = 109,
        MOUSE_4 = 110,
        MOUSE_5 = 111,

        KEY_C = 13,
        KEY_X = 34
    } CS_BUTTONCODE;
    bool  is_button_down(CS_BUTTONCODE button);
    vec2i get_mouse_analog(void);
    vec2i get_mouse_analog_delta(void);
}

namespace engine {
	int       is_running(void);
	int       get_local_player(void);
	vec3      get_view_angles(void);
	void      set_view_angles(vec3 v);
	int       get_max_clients(void);
	bool      is_in_game(void);
    bool      is_in_insecure(void);
}

namespace entity {
	cs_player get_client_ent(int index);
}

namespace interfaces {
    cs_interface_reg find(const wchar_t* module_name);
}

namespace esp {
    void _crosshair_set_color(int cl_crosshaircolor_r, int cl_crosshaircolor_g,
        int cl_crosshaircolor_b);
}

namespace cvar {
    cs_convar find(const char* name);
}

namespace netvars {
    cs_netvar_table find(const char* name);
}