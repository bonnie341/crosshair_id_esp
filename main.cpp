#include "csgo.h"
#include "maths.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define STATUS_SUCCESS 0
static cs_player   _target;
static cs_convar   _sensitivity;
static cs_convar   _mp_teammates_are_enemies;
static float       _flsensitivity;
static int         _current_tick, _previous_tick;

float              _aimbotic_fov = 3.0f;
float              _aimbotic_smooth = 1.0f;

typedef struct {
    int   bone;
    float radius;
    vec3  min;
    vec3  max;
} hitbox;

static hitbox hitbox_list[2][6] = {
    {
        {5, 6.200000f, {4.800000f,  0.150000f,  -4.100000f}, {4.800000f,  0.150000f, 4.100000f}},
        {4, 6.500000f, {3.800000f,  0.800000f,  -2.400000f}, {3.800000f,  0.400000f, 2.400000f}},
        {3, 6.000000f, {1.400000f,  0.800000f,  3.100000f},  {1.400000f,  0.800000f, -3.100000f}},
        {0, 6.000000f, {-2.700000f, 1.100000f,  -3.200000f}, {-2.700000f, 1.100000f, 3.200000f}},
        {7, 3.500000f, {-1.300000f, -0.200000f, 0.000000f},  {1.400000f,  0.600000f, 0.000000f}},
        {8, 4.300000f, {-1.100000f, 1.400000f,  0.100000f},  {3.000000f,  0.800000f, 0.100000f}}
    },

    {
        {5, 6.200000f, {4.800000f,  0.150000f,  -4.100000f}, {4.800000f,  0.150000f, 4.100000f}},
        {4, 6.500000f, {3.800000f,  0.800000f,  -2.400000f}, {3.800000f,  0.400000f, 2.400000f}},
        {3, 6.000000f, {1.400000f,  0.800000f,  3.100000f},  {1.400000f,  0.800000f, -3.100000f}},
        {0, 6.000000f, {-2.700000f, 1.100000f,  -3.200000f}, {-2.700000f, 1.100000f, 3.200000f}},
        {7, 3.500000f, {-1.300000f, 0.900000f,  0.000000f},  {1.400000f,  1.300000f, 0.000000f}},
        {8, 3.200000f, {-0.200000f, 1.100000f,  0.000000f},  {3.600000f,  0.100000f, 0.000000f}}
    }
};

static int crosshair_id(vec3 vangle, cs_player self)
{
    vec3        dir, eye;
    int         i;
    cs_player   entity;
    int         id, j;
    matrix3x4_t matrix;
    bool        status;


    dir = math::vec_atd(vangle);
    eye = self.get_eye_pos();
    for (i = 1; i < engine::get_max_clients(); i++) {
        entity = entity::get_client_ent(i);
        if (!entity.is_valid())
            continue;

        id = entity.get_team();
        if (!_mp_teammates_are_enemies.get_int() && self.get_team() == id)
            continue;
        id -= 2;
        for (j = 6; j--;) {
            entity.bone_matrix(hitbox_list[id][j].bone, &matrix);
            status = math::vec_min_max(
                eye,
                dir,
                math::vec_transform(hitbox_list[id][j].min, matrix),
                math::vec_transform(hitbox_list[id][j].max, matrix),
                hitbox_list[id][j].radius
            );

            if (status) {
                return i;
            }
        }
    }
    return 0;
}

static vec3 get_target_angle(cs_player self, cs_player target)
{
    matrix3x4_t m;
    vec3        c, p;

    target.bone_matrix(8, &m);
    c = self.get_eye_pos();
    m[0][3] -= c.x, m[1][3] -= c.y, m[2][3] -= c.z;
    c.x = m[0][3], c.y = m[1][3], c.z = m[2][3];
    math::vec_normalize(&c);
    math::vec_angles(c, &c);
    if (self.get_shots_fired() > 1) {
        p = self.get_vec_punch();
        c.x -= p.x * 2.0f, c.y -= p.y * 2.0f, c.z -= p.z * 2.0f;
    }
    math::vec_clamp(&c);
    return c;
}

static bool get_target(cs_player self, vec3 vangle)
{
    float     best_fov;
    int       i;
    cs_player entity;
    float     fov;

    best_fov = 9999.0f;
    for (i = 1; i < engine::get_max_clients(); i++) {
        entity = entity::get_client_ent(i);
        if (!entity.is_valid())
            continue;
        if (!_mp_teammates_are_enemies.get_int() && self.get_team() == entity.get_team())
            continue;
        fov = math::get_fov(vangle, get_target_angle(self, entity));
        if (fov < best_fov) {
            best_fov = fov;
            _target = entity;
        }
    }
    return best_fov != 9999.0f;
}

static void mouse_move(int x, int y)
{
    mouse_event(0x0001, x, y, 0, 0);
}

static void mouse1_down(void)
{
    mouse_event(0x0002, 0, 0, 0, 0);
}

static void mouse1_up(void)
{
    mouse_event(0x0004, 0, 0, 0, 0);
}


static void aim_at_target(vec3 vangle, vec3 angle)
{
    float x, y, sx, sy;

    y = vangle.x - angle.x, x = vangle.y - angle.y;
    if (y > 89.0f)   y = 89.0f;   else if (y < -89.0f)  y = -89.0f;
    if (x > 180.0f)  x -= 360.0f; else if (x < -180.0f) x += 360.0f;

    if (fabs(x) / 180.0f >= _aimbotic_fov)
        return;

    if (fabs(y) / 89.0f >= _aimbotic_fov)
        return;

    x = ((x / _flsensitivity) / 0.022f);
    y = ((y / _flsensitivity) / -0.022f);

    if (_aimbotic_smooth) {
        sx = 0.0f, sy = 0.0f;
        if (sx < x) {
            sx += 1.0f + (x / _aimbotic_smooth);
        }
        else if (sx > x) {
            sx -= 1.0f - (x / _aimbotic_smooth);
        }
        if (sy < y) {
            sy += 1.0f + (y / _aimbotic_smooth);
        }
        else if (sy > y) {
            sy -= 1.0f - (y / _aimbotic_smooth);
        }
    }
    else {
        sx = x, sy = y;
    }
    if (_current_tick - _previous_tick > 0) {
        _previous_tick = _current_tick;

        mouse_move((int)sx, (int)sy); // alternative
    }
}

static int cross_esp(void)
{
    cs_player self;
    vec3      vangle;

    self = entity::get_client_ent(engine::get_local_player());
    vangle = engine::get_view_angles();

    float     best_fov;
    int       i;
    cs_player entity;
    float     fov;

    best_fov = 1.5f;

    for (i = 1; i < engine::get_max_clients(); i++) {
        entity = entity::get_client_ent(i);

        if (!entity.is_valid())
            continue;

        if (!_mp_teammates_are_enemies.get_int() && self.get_team() == entity.get_team())
            continue;

        fov = math::get_fov(vangle, get_target_angle(self, entity));

        if (fov < best_fov) {
            esp::_crosshair_set_color(255, 22, 22);
        }
        else {
            esp::_crosshair_set_color(255, 255, 255);
        }

    }
    return 1;
}

static void mouse(const char* aim_enables, inputsystem::_CS_BUTTONCODE aim_key, 
   const char* trigger_enables, inputsystem::_CS_BUTTONCODE trig_key)
{
    cs_player self;
    vec3      vangle;
    bool      a;

    self = entity::get_client_ent(engine::get_local_player());
    vangle = engine::get_view_angles();
    _current_tick = self.get_tick_count();
    _flsensitivity = _sensitivity.get_float();

    if (self.is_scoped()) {
        _flsensitivity = (self.get_fov() / 90.0f) * _flsensitivity;
    }

    if (inputsystem::is_button_down(trig_key) && trigger_enables == xorstr("trigger_on")) {
        a = false;
        if (crosshair_id(vangle, self)) {
            mouse1_down();
            Sleep(1);
            mouse1_up();
        }
    }
    else {
        a = true;
    }

    if (a == true && inputsystem::is_button_down(aim_key) && aim_enables == xorstr("aimbot_on")) {
        if (!_target.is_valid() && !get_target(self, vangle))
            return;
        aim_at_target(vangle, get_target_angle(self, _target));
    }
    else {
        _target = {};
    }
}

static int __stdcall initialize(BOOL status) {

    if (!cs_initialize()) {
        printf(xorstr("[!] game is not running!\n"));
    }
    else {
        printf(xorstr("[+] game is running!\n"));
    }

    if (engine::is_in_insecure()) {
        printf(xorstr("[!] is insecure runned!\n"));
    }

    _sensitivity = cvar::find(xorstr("sensitivity"));
    _mp_teammates_are_enemies = cvar::find(xorstr("mp_teammates_are_enemies"));

    while (engine::is_running()) {
        if (engine::is_in_game()) {

            mouse(xorstr("aimbot_on"), inputsystem::KEY_C, xorstr("trigger_on"), inputsystem::KEY_X);

            if (!cross_esp()) {
                printf(xorstr("[!] cross esp is not initialized!\n"));
            } else {
                printf(xorstr("[+] cross esp is initialized!\n"));
            }
        }
    }

    return 1;
}

static bool NT_STATUS() {
    return false;
}

static bool _hide() {
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    return true;
}

NTSTATUS main(uint16_t argc,
    char** argv,
    NTSTATUS status) {

    Sleep(25); 
    
    if (!NT_STATUS()) {
        status = STATUS_SUCCESS;
    }

    if (bool(_hide() == true)) {
        status = initialize(status);
    }
}