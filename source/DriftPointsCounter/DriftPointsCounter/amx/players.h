// Copyright (c) 2011 SA:MP GDK Project
// See file LICENSE for license details.

#ifndef SAMPGDK_PLAYERS_H
#define SAMPGDK_PLAYERS_H

#include <cstddef> // size_t

#include "export.h"
#include "samp.h"

namespace sampgdk {

#define SPECIAL_ACTION_NONE              0
#define SPECIAL_ACTION_DUCK              1
#define SPECIAL_ACTION_USEJETPACK        2
#define SPECIAL_ACTION_ENTER_VEHICLE     3
#define SPECIAL_ACTION_EXIT_VEHICLE      4
#define SPECIAL_ACTION_DANCE1            5
#define SPECIAL_ACTION_DANCE2            6
#define SPECIAL_ACTION_DANCE3            7
#define SPECIAL_ACTION_DANCE4            8
#define SPECIAL_ACTION_HANDSUP           10
#define SPECIAL_ACTION_USECELLPHONE      11
#define SPECIAL_ACTION_SITTING           12
#define SPECIAL_ACTION_STOPUSECELLPHONE  13
#define SPECIAL_ACTION_DRINK_BEER        20
#define SPECIAL_ACTION_SMOKE_CIGGY       21
#define SPECIAL_ACTION_DRINK_WINE        22
#define SPECIAL_ACTION_DRINK_SPRUNK      23

#define FIGHT_STYLE_NORMAL               4
#define FIGHT_STYLE_BOXING               5
#define FIGHT_STYLE_KUNGFU               6
#define FIGHT_STYLE_KNEEHEAD             7
#define FIGHT_STYLE_GRABKICK             15
#define FIGHT_STYLE_ELBOW                16

#define WEAPONSKILL_PISTOL                0
#define WEAPONSKILL_PISTOL_SILENCED       1
#define WEAPONSKILL_DESERT_EAGLE          2
#define WEAPONSKILL_SHOTGUN               3
#define WEAPONSKILL_SAWNOFF_SHOTGUN       4
#define WEAPONSKILL_SPAS12_SHOTGUN        5
#define WEAPONSKILL_MICRO_UZI             6
#define WEAPONSKILL_MP5                   7
#define WEAPONSKILL_AK47                  8
#define WEAPONSKILL_M4                    9
#define WEAPONSKILL_SNIPERRIFLE           10

#define WEAPONSTATE_UNKNOWN              -1
#define WEAPONSTATE_NO_BULLETS            0
#define WEAPONSTATE_LAST_BULLET           1
#define WEAPONSTATE_MORE_BULLETS          2
#define WEAPONSTATE_RELOADING             3

// Player
SAMPGDK_EXPORT bool SAMPGDK_CALL SetSpawnInfo(int playerid, int team, int skin, float x, float y, float z, float rotation, int weapon1, int weapon1_ammo, int weapon2, int weapon2_ammo, int weapon3, int weapon3_ammo);
SAMPGDK_EXPORT bool SAMPGDK_CALL SpawnPlayer(int playerid);

// Player info
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerPos(int playerid, float x, float y, float z);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerPosFindZ(int playerid, float x, float y, float z);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPlayerPos(int playerid, float &x, float &y, float &z);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerFacingAngle(int playerid, float angle);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPlayerFacingAngle(int playerid, float &angle);
SAMPGDK_EXPORT bool SAMPGDK_CALL IsPlayerInRangeOfPoint(int playerid, float range, float x, float y, float z);
SAMPGDK_EXPORT float SAMPGDK_CALL GetPlayerDistanceFromPoint(int playerid, float x, float y, float z);
SAMPGDK_EXPORT bool SAMPGDK_CALL IsPlayerStreamedIn(int playerid, int forplayerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerInterior(int playerid, int interiorid);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerInterior(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerHealth(int playerid, float health);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPlayerHealth(int playerid, float &health);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerArmour(int playerid, float armour);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPlayerArmour(int playerid, float &armour);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerAmmo(int playerid, int weaponslot, int ammo);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerAmmo(int playerid);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerWeaponState(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerTeam(int playerid, int teamid);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerTeam(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerScore(int playerid, long score);
SAMPGDK_EXPORT long SAMPGDK_CALL GetPlayerScore(int playerid);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerDrunkLevel(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerDrunkLevel(int playerid, int level);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerColor(int playerid, long color);
SAMPGDK_EXPORT long SAMPGDK_CALL GetPlayerColor(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerSkin(int playerid, int skinid);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerSkin(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL GivePlayerWeapon(int playerid, int weaponid, int ammo);
SAMPGDK_EXPORT bool SAMPGDK_CALL ResetPlayerWeapons(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerArmedWeapon(int playerid, int weaponid);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPlayerWeaponData(int playerid, int slot, int &weapon, int &ammo);
SAMPGDK_EXPORT bool SAMPGDK_CALL GivePlayerMoney(int playerid, long money);
SAMPGDK_EXPORT bool SAMPGDK_CALL ResetPlayerMoney(int playerid);
SAMPGDK_EXPORT int SAMPGDK_CALL SetPlayerName(int playerid, const char *name);
SAMPGDK_EXPORT long SAMPGDK_CALL GetPlayerMoney(int playerid);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerState(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPlayerIp(int playerid, char *ip, size_t size);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerPing(int playerid);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerWeapon(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPlayerKeys(int playerid, int &keys, int &updown, int &leftright);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPlayerName(int playerid, char *name, size_t size);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerTime(int playerid, int hour, int minute);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPlayerTime(int playerid, int &hour, int &minute);
SAMPGDK_EXPORT bool SAMPGDK_CALL TogglePlayerClock(int playerid, bool toggle);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerWeather(int playerid, int weather);
SAMPGDK_EXPORT bool SAMPGDK_CALL ForceClassSelection(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerWantedLevel(int playerid, int level);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerWantedLevel(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerFightingStyle(int playerid, int style);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerFightingStyle(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerVelocity(int playerid, float x, float y, float z);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPlayerVelocity( int playerid, float &x, float &y, float &z);
SAMPGDK_EXPORT bool SAMPGDK_CALL PlayCrimeReportForPlayer(int playerid, int suspectid, int crime);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerShopName(int playerid, const char *shopname);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerSkillLevel(int playerid, int skill, int level);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerSurfingVehicleID(int playerid);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerSurfingObjectID(int playerid);

// Attached to bone objects

#define MAX_PLAYER_ATTACHED_OBJECTS 5 // This is the number of attached indexes available ie 5 = 0-4

SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerAttachedObject(int playerid, int index, int modelid, int bone, float fOffsetX = 0.0, float fOffsetY = 0.0, float fOffsetZ = 0.0, float fRotX = 0.0, float fRotY = 0.0, float fRotZ = 0.0, float fScaleX = 1.0, float fScaleY = 1.0, float fScaleZ = 1.0);
SAMPGDK_EXPORT bool SAMPGDK_CALL RemovePlayerAttachedObject(int playerid, int index);
SAMPGDK_EXPORT bool SAMPGDK_CALL IsPlayerAttachedObjectSlotUsed(int playerid, int index);

// Per-player variable system (PVars)
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPVarInt(int playerid, const char *varname, int value);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPVarInt(int playerid, const char *varname);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPVarString(int playerid, const char *varname, const char *value);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPVarString(int playerid, const char *varname, char *value, size_t size);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPVarFloat(int playerid, const char *varname, float value);
SAMPGDK_EXPORT float SAMPGDK_CALL GetPVarFloat(int playerid, const char *varname);
SAMPGDK_EXPORT bool SAMPGDK_CALL DeletePVar(int playerid, const char *varname);

// PVar enumeration
#define PLAYER_VARTYPE_NONE         0
#define PLAYER_VARTYPE_INT          1
#define PLAYER_VARTYPE_STRING       2
#define PLAYER_VARTYPE_FLOAT        3

SAMPGDK_EXPORT int SAMPGDK_CALL GetPVarsUpperIndex(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPVarNameAtIndex(int playerid, int index, char *varname, size_t size);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPVarType(int playerid, const char *varname);

#define MAX_CHATBUBBLE_LENGTH 144
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerChatBubble(int playerid, const char *text, long color, float drawdistance, long expiretime);

// Player controls
SAMPGDK_EXPORT bool SAMPGDK_CALL PutPlayerInVehicle(int playerid, int vehicleid, int seatid);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerVehicleID(int playerid);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerVehicleSeat(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL RemovePlayerFromVehicle(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL TogglePlayerControllable(int playerid, bool toggle);
SAMPGDK_EXPORT bool SAMPGDK_CALL PlayerPlaySound(int playerid, int soundid, float x, float y, float z);
SAMPGDK_EXPORT bool SAMPGDK_CALL ApplyAnimation(int playerid, const char *animlib, const char *animname, float fDelta, bool loop, bool lockx, bool locky, bool freeze, long time, bool forcesync = false);
SAMPGDK_EXPORT bool SAMPGDK_CALL ClearAnimations(int playerid, bool forcesync = false);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerAnimationIndex(int playerid); // return the index of any running applied animations (0 if none are running)
SAMPGDK_EXPORT bool SAMPGDK_CALL GetAnimationName(int index, char *animlib, size_t animlib_size, char *animname, size_t animname_size); // get the animation lib/name for the index
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerSpecialAction(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerSpecialAction(int playerid, int actionid);

// Player world/map related
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerCheckpoint(int playerid, float x, float y, float z, float size);
SAMPGDK_EXPORT bool SAMPGDK_CALL DisablePlayerCheckpoint(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerRaceCheckpoint(int playerid, int type, float x, float y, float z, float nextx, float nexty, float nextz, float size);
SAMPGDK_EXPORT bool SAMPGDK_CALL DisablePlayerRaceCheckpoint(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerWorldBounds(int playerid, float x_max, float x_min, float y_max, float y_min);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerMarkerForPlayer(int playerid, int showplayerid, long color);
SAMPGDK_EXPORT bool SAMPGDK_CALL ShowPlayerNameTagForPlayer(int playerid, int showplayerid, bool show);

#define MAPICON_LOCAL             0 // displays in the player's local are
#define MAPICON_GLOBAL            1 // displays always
#define MAPICON_LOCAL_CHECKPOINT  2 // displays in the player's local area and has a checkpoint marker
#define MAPICON_GLOBAL_CHECKPOINT 3 // displays always and has a checkpoint marker

SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerMapIcon(int playerid, int iconid, float x, float y, float z, int markertype, long color, int style = MAPICON_LOCAL);
SAMPGDK_EXPORT bool SAMPGDK_CALL RemovePlayerMapIcon(int playerid, int iconid);

SAMPGDK_EXPORT bool SAMPGDK_CALL AllowPlayerTeleport(int playerid, bool allow);

// Player camera
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerCameraPos(int playerid, float x, float y, float z);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerCameraLookAt(int playerid, float x, float y, float z);
SAMPGDK_EXPORT bool SAMPGDK_CALL SetCameraBehindPlayer(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPlayerCameraPos(int playerid, float &x, float &y, float &z);
SAMPGDK_EXPORT bool SAMPGDK_CALL GetPlayerCameraFrontVector(int playerid, float &x, float &y, float &z);
SAMPGDK_EXPORT int SAMPGDK_CALL GetPlayerCameraMode(int playerid);

// Player conditionals
SAMPGDK_EXPORT bool SAMPGDK_CALL IsPlayerConnected(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL IsPlayerInVehicle(int playerid, int vehicleid);
SAMPGDK_EXPORT bool SAMPGDK_CALL IsPlayerInAnyVehicle(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL IsPlayerInCheckpoint(int playerid);
SAMPGDK_EXPORT bool SAMPGDK_CALL IsPlayerInRaceCheckpoint(int playerid);

// Virtual Worlds
SAMPGDK_EXPORT bool SAMPGDK_CALL SetPlayerVirtualWorld(int playerid, long worldid);
SAMPGDK_EXPORT long SAMPGDK_CALL GetPlayerVirtualWorld(int playerid);

// Insane Stunts
SAMPGDK_EXPORT bool SAMPGDK_CALL EnableStuntBonusForPlayer(int playerid, bool enable);
SAMPGDK_EXPORT void SAMPGDK_CALL EnableStuntBonusForAll(bool enable);

// Spectating
#define SPECTATE_MODE_NORMAL    1
#define SPECTATE_MODE_FIXED     2
#define SPECTATE_MODE_SIDE      3

SAMPGDK_EXPORT bool SAMPGDK_CALL TogglePlayerSpectating(int playerid, bool toggle);
SAMPGDK_EXPORT bool SAMPGDK_CALL PlayerSpectatePlayer(int playerid, int targetplayerid, int mode = SPECTATE_MODE_NORMAL);
SAMPGDK_EXPORT bool SAMPGDK_CALL PlayerSpectateVehicle(int playerid, int targetvehicleid, int mode = SPECTATE_MODE_NORMAL);

// Recording for NPC playback
#define PLAYER_RECORDING_TYPE_NONE      0
#define PLAYER_RECORDING_TYPE_DRIVER    1
#define PLAYER_RECORDING_TYPE_ONFOOT    2

SAMPGDK_EXPORT bool SAMPGDK_CALL StartRecordingPlayerData(int playerid, int recordtype, const char *recordname);
SAMPGDK_EXPORT bool SAMPGDK_CALL StopRecordingPlayerData(int playerid);

// Convenience templates for some functions (to avoid specifying output buffer size explicitly
// when it's known at compile time (i.e. fixed-size buffers))
template<size_t N> bool GetPlayerName(int playerid, char (&name)[N]) { 
    return GetPlayerName(playerid, name, N); 
}
template<size_t N> bool GetPlayerIp(int playerid, char (&ip)[N]) { 
    return GetPlayerIp(playerid, ip, N); 
}
template<size_t N> bool GetPVarString(int playerid, const char *varname, char (&value)[N]) {
    return GetPVarString(playerid, varname, value, N);
}
template<size_t N> bool GetPVarNameAtIndex(int playerid, int index, char (&varname)[N]) {
    return GetPVarNameAtIndex(playerid, index, varname, N);
}
template<size_t N1, size_t N2> bool GetAnimationName(int index, char (&animlib)[N1], char (&animname)[N2]) {
    return GetAnimationName(index, animlib, N1, animname, N2);
}

} // namespace sampgdk

#endif

