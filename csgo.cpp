#include "csgo.h"
#include "memory.h"

#include <stdio.h>

_nt_memory        cs_p(L"csgo.exe");

namespace cs_vt {
	cs_virtual_table cs_vt_client;
	cs_virtual_table cs_vt_entity;
	cs_virtual_table cs_vt_engine;
	cs_virtual_table cs_vt_cvar;
	cs_virtual_table cs_vt_inputsystem;
}

namespace cs_os {
	csptr_t          cs_nv_dwEntityList;
	csptr_t          cs_nv_dwClientState;
	uint32_t         cs_nv_m_dwGetLocalPlayer;
	uint32_t         cs_nv_m_dwViewAngles;
	uint32_t         cs_nv_m_dwMaxClients;
	uint32_t         cs_nv_m_dwState;
	uint32_t         cs_nv_m_dwButton;
	uint32_t         cs_nv_m_dwAnalogDelta;
	uint32_t         cs_nv_m_dwAnalog;
	uint32_t         cs_nv_m_iHealth;
	uint32_t         cs_nv_m_vecViewOffset;
	uint32_t         cs_nv_m_lifeState;
	uint32_t         cs_nv_m_nTickBase;
	uint32_t         cs_nv_m_vecVelocity;
	uint32_t         cs_nv_m_vecPunch;
	uint32_t         cs_nv_m_iFOV;
	uint32_t         cs_nv_m_iTeamNum;
	uint32_t         cs_nv_m_vecOrigin;
	uint32_t         cs_nv_m_hActiveWeapon;
	uint32_t         cs_nv_m_iShotsFired;
	uint32_t         cs_nv_m_bIsScoped;
	uint32_t         cs_nv_m_dwBoneMatrix;
}


int cs_player::get_team(void)
{
	return cs_p.read<int>(self + cs_os::cs_nv_m_iTeamNum);
}

int cs_player::get_health(void)
{
	return cs_p.read<int>(self + cs_os::cs_nv_m_iHealth);
}

int cs_player::get_life_state(void)
{
	return cs_p.read<int>(self + cs_os::cs_nv_m_lifeState);
}

int cs_player::get_tick_count(void)
{
	return cs_p.read<int>(self + cs_os::cs_nv_m_nTickBase);
}

int cs_player::get_shots_fired(void)
{
	return cs_p.read<int>(self + cs_os::cs_nv_m_iShotsFired);
}

bool cs_player::is_scoped(void)
{
	return cs_p.read<bool>(self + cs_os::cs_nv_m_bIsScoped);
}

bool cs_player::is_dormant(void)
{
	csptr_t a;

	a = (csptr_t)(self + 0x8);
	return cs_p.read<bool>(a + cs_p.read<uint8_t>((*(cs_virtual_table*)&a).function(9) + 0x8));
}

csptr_t cs_player::get_weapon(void)
{
	csptr_t v;

	v = cs_p.read<csptr_t>(self + cs_os::cs_nv_m_hActiveWeapon);
	return cs_p.read<csptr_t>(cs_os::cs_nv_dwEntityList + ((v & 0xFFF) - 1) * 0x10);
}

vec3 cs_player::get_origin(void)
{
	return cs_p.read<vec3>(self + cs_os::cs_nv_m_vecOrigin);
}

vec3 cs_player::get_vec_view(void)
{
	return cs_p.read<vec3>(self + cs_os::cs_nv_m_vecViewOffset);
}

vec3 cs_player::get_eye_pos(void)
{
	vec3 v, o, r;
	v = this->get_vec_view();
	o = this->get_origin();
	r.x = v.x += o.x; r.y = v.y += o.y; r.z = v.z += o.z;
	return r;
}

vec3 cs_player::get_vec_velocity(void)
{
	return cs_p.read<vec3>(self + cs_os::cs_nv_m_vecViewOffset);
}

vec3 cs_player::get_vec_punch(void)
{
	return cs_p.read<vec3>(self + cs_os::cs_nv_m_vecPunch);
}

int cs_player::get_fov(void)
{
	return cs_p.read<int>(self + cs_os::cs_nv_m_iFOV);
}

void cs_player::bone_matrix(int index, matrix3x4_t* out)
{
	cs_p.read(cs_p.read<csptr_t>(self + cs_os::cs_nv_m_dwBoneMatrix) + 0x30 * index, out, sizeof(matrix3x4_t));
}

bool cs_player::is_valid(void)
{
	int health = this->get_health();

	return self && get_life_state() == 0 && health > 0 && health < 1337;
}


SHORT_STRING cs_convar::get_name(void)
{
	return cs_p.read_ptr32<SHORT_STRING>(self + 0xC);
}

void cs_convar::set_string(const char* v)
{
	SHORT_STRING a0;
	memcpy(&a0, v, strlen(v));
	cs_p.write_ptr<SHORT_STRING>(self + 0x24, a0);
}

void cs_convar::set_int(int v)
{
	cs_p.write<uint32_t>(self + 0x30, v ^ self);
}

void cs_convar::set_float(float v)
{
	cs_p.write<uint32_t>(self + 0x2C, *(uint32_t*)&v ^ self);
}

SHORT_STRING cs_convar::get_string(void)
{
	return cs_p.read_ptr32<SHORT_STRING>(self + 0x24);
}

int cs_convar::get_int(void)
{
	return cs_p.read<int>(self + 0x30) ^ self;
}

float cs_convar::get_float(void)
{
	uint32_t v;
	v = cs_p.read<uint32_t>(self + 0x2C) ^ self;
	return *(float*)&v;
}

cs_convar cvar::find(const char* name)
{
	csptr_t a0;

	a0 = cs_p.read_ptr32<csptr_t>(cs_vt::cs_vt_cvar.address() + 0x34);
	while ((a0 = cs_p.read<csptr_t>(a0 + 0x4))) {
		if (!strcmp(name, cs_p.read_ptr32<SHORT_STRING>(a0 + 0xc).value)) {
			return *(cs_convar*)&a0;
		}
	}
	return {};
}

bool inputsystem::is_button_down(CS_BUTTONCODE button)
{
	uint32_t v;

	v = cs_p.read<uint32_t>(cs_vt::cs_vt_inputsystem.address() + (((button >> 5) * 4) + cs_os::cs_nv_m_dwButton));
	return (v >> (button & 31)) & 1;
}

vec2i inputsystem::get_mouse_analog(void)
{
	return cs_p.read<vec2i>(cs_vt::cs_vt_inputsystem.address() + cs_os::cs_nv_m_dwAnalog);
}

vec2i inputsystem::get_mouse_analog_delta(void)
{
	return cs_p.read<vec2i>(cs_vt::cs_vt_inputsystem.address() + cs_os::cs_nv_m_dwAnalogDelta);
}


uint32_t cs_netvar_table::offset(const char* name)
{
	return offset_ex(self, name);
}

uint32_t cs_netvar_table::offset_ex(csptr_t address, const char* name)
{
	uint32_t  a0;   uint32_t  a1;
	csptr_t   a2;   uint32_t  a3;
	csptr_t   a4;   uint32_t  a5;


	a0 = 0;
	for (a1 = cs_p.read<uint32_t>(address + 0x4); a1--;) {
		a2 = a1 * 60 + cs_p.read<csptr_t>(address);
		a3 = cs_p.read<uint32_t>(a2 + 0x2C);
		if ((a4 = cs_p.read<csptr_t>(a2 + 0x28)) && cs_p.read<uint32_t>(a4 + 0x4)) {
			if ((a5 = offset_ex(a4, name))) {
				a0 += a3 + a5;
			}
		}
		if (strcmp(cs_p.read_ptr32<SHORT_STRING>(a2).value, name) == 0) {
			return a3 + a0;
		}
	}
	return a0;
}

cs_netvar_table netvars::find(const char* name)
{
	csptr_t a0, a1;

	a0 = cs_p.read_ptr32<csptr_t>(cs_vt::cs_vt_client.function(8) + 1);
	do {
		a1 = cs_p.read<csptr_t>(a0 + 0xC);
		if (strcmp(cs_p.read_ptr32<SHORT_STRING>(a1 + 0xC).value, name) == 0) {
			return *(cs_netvar_table*)&a1;
		}
	} while ((a0 = cs_p.read<csptr_t>(a0 + 0x10)));
	throw name;
}


csptr_t cs_virtual_table::address(void) const
{
	return self;
}

csptr_t cs_virtual_table::function(int index)
{
	return cs_p.read<csptr_t>(cs_p.read<csptr_t>(self) + index * 4);
}

cs_virtual_table cs_interface_reg::find(const char* name)
{
	csptr_t a0 = self, a1[30];

	do {
		cs_p.read(cs_p.read<csptr_t>(a0 + 0x4), &a1, sizeof(a1));
		if ((_stricmp((const char*)a1, name) >> 5) == 1) {
			return cs_p.read<cs_virtual_table>(cs_p.read<csptr_t>(a0) + 1);
		}
	} while ((a0 = cs_p.read<csptr_t>(a0 + 0x8)));
	throw name;
}

cs_interface_reg interfaces::find(const wchar_t* module_name)
{
	uintptr_t v;

	v = cs_p.find_export(cs_p.find_module(module_name), xorstr("CreateInterface"));
	if (v == 0)
		throw module_name;
	return cs_p.read_ptr32<cs_interface_reg>(v - 0x6A);
}

void esp::_crosshair_set_color(int cl_crosshaircolor_r, int cl_crosshaircolor_g,
	int cl_crosshaircolor_b) {
	auto con_cl_crosshaircolor_r = cvar::find(xorstr("cl_crosshaircolor_r"));
	auto con_cl_crosshaircolor_g = cvar::find(xorstr("cl_crosshaircolor_g"));
	auto con_cl_crosshaircolor_b = cvar::find(xorstr("cl_crosshaircolor_b"));

	con_cl_crosshaircolor_r.set_int(cl_crosshaircolor_r);
	con_cl_crosshaircolor_g.set_int(cl_crosshaircolor_g);
	con_cl_crosshaircolor_b.set_int(cl_crosshaircolor_b);
}


static void initialize_vt(void);
static void initialize_nv(void);

bool cs_initialize(void)
{
	if (!cs_p.attach()) {
		return false;
	}
	
	initialize_vt();
	initialize_nv();

	return true;
}

int engine::is_running(void)
{
	return cs_p.exists();
}

int engine::get_local_player(void)
{
	return cs_p.read<int>(cs_os::cs_nv_dwClientState + cs_os::cs_nv_m_dwGetLocalPlayer);
}

vec3 engine::get_view_angles(void)
{
	return cs_p.read<vec3>(cs_os::cs_nv_dwClientState + cs_os::cs_nv_m_dwViewAngles);
}

void engine::set_view_angles(vec3 v)
{
	cs_p.write<vec3>(cs_os::cs_nv_dwClientState + cs_os::cs_nv_m_dwViewAngles, v);
}

int engine::get_max_clients(void)
{
	return cs_p.read<int>(cs_os::cs_nv_dwClientState + cs_os::cs_nv_m_dwMaxClients);
}

bool engine::is_in_insecure(void) {

	if (std::strstr(GetCommandLineA(), xorstr("-insecure")))
		return true;
}

bool engine::is_in_game(void)
{
	return cs_p.read<unsigned char>(cs_os::cs_nv_dwClientState + cs_os::cs_nv_m_dwState) >> 2;
}

cs_player entity::get_client_ent(int index)
{
	return cs_p.read<cs_player>(cs_os::cs_nv_dwEntityList + index * 0x10);
}

static void initialize_vt(void)
{
	cs_interface_reg t;

	t = interfaces::find(L"client.dll");

	cs_vt::cs_vt_client = t.find(xorstr("VClient"));
	cs_vt::cs_vt_entity = t.find(xorstr("VClientEntityList"));

	t = interfaces::find(L"engine.dll");

	cs_vt::cs_vt_engine = t.find(xorstr("VEngineClient"));

	t = interfaces::find(L"vstdlib.dll");

	cs_vt::cs_vt_cvar = t.find(xorstr("VEngineCvar"));

	t = interfaces::find(L"inputsystem.dll");

	cs_vt::cs_vt_inputsystem = t.find(xorstr("InputSystemVersion"));
}

static csptr_t offset_entitylist(void)
{
	return cs_vt::cs_vt_entity.address() - (cs_p.read<uint32_t>(cs_vt::cs_vt_entity.function(5) + 0x22) - 0x38);
}

static csptr_t offset_clientstate(void)
{
	return cs_p.read_ptr32<csptr_t>(cs_vt::cs_vt_engine.function(18) + 0x16);
}

static void initialize_nv(void)
{
	cs_netvar_table t;

	cs_os::cs_nv_dwEntityList = offset_entitylist();
	cs_os::cs_nv_dwClientState = offset_clientstate();

	cs_os::cs_nv_m_dwGetLocalPlayer = cs_p.read<uint32_t>(cs_vt::cs_vt_engine.function(12) + 0x16);
	cs_os::cs_nv_m_dwViewAngles = cs_p.read<uint32_t>(cs_vt::cs_vt_engine.function(19) + 0xB2);
	cs_os::cs_nv_m_dwMaxClients = cs_p.read<uint32_t>(cs_vt::cs_vt_engine.function(20) + 0x07);
	cs_os::cs_nv_m_dwState = cs_p.read<uint32_t>(cs_vt::cs_vt_engine.function(26) + 0x07);
	cs_os::cs_nv_m_dwButton = cs_p.read<uint32_t>(cs_vt::cs_vt_inputsystem.function(15) + 0x21D);
	cs_os::cs_nv_m_dwAnalogDelta = cs_p.read<uint32_t>(cs_vt::cs_vt_inputsystem.function(18) + 0x29);
	cs_os::cs_nv_m_dwAnalog = cs_p.read<uint32_t>(cs_vt::cs_vt_inputsystem.function(18) + 0x09);

	t = netvars::find(xorstr("DT_BasePlayer"));

	cs_os::cs_nv_m_iHealth = t.offset(xorstr("m_iHealth"));
	cs_os::cs_nv_m_vecViewOffset = t.offset(xorstr("m_vecViewOffset[0]"));
	cs_os::cs_nv_m_lifeState = t.offset(xorstr("m_lifeState"));
	cs_os::cs_nv_m_nTickBase = t.offset(xorstr("m_nTickBase"));
	cs_os::cs_nv_m_vecVelocity = t.offset(xorstr("m_vecVelocity[0]"));
	cs_os::cs_nv_m_vecPunch = t.offset(xorstr("m_Local")) + 0x70;
	cs_os::cs_nv_m_iFOV = t.offset(xorstr("m_iFOV"));

	t = netvars::find(xorstr("DT_BaseEntity"));

	cs_os::cs_nv_m_iTeamNum = t.offset(xorstr("m_iTeamNum"));
	cs_os::cs_nv_m_vecOrigin = t.offset(xorstr("m_vecOrigin"));

	t = netvars::find(xorstr("DT_CSPlayer"));

	cs_os::cs_nv_m_hActiveWeapon = t.offset(xorstr("m_hActiveWeapon"));
	cs_os::cs_nv_m_iShotsFired = t.offset(xorstr("m_iShotsFired"));
	cs_os::cs_nv_m_bIsScoped = t.offset(xorstr("m_bIsScoped"));
	t = netvars::find(xorstr("DT_BaseAnimating"));
	cs_os::cs_nv_m_dwBoneMatrix = t.offset(xorstr("m_nForceBone")) + 0x1C;
}
