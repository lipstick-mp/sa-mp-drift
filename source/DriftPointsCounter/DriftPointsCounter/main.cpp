/*
* Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <cstring>
#include <limits>
#include <sstream>

#include "./amx/logprintf.h" 
#include "./amx/samp.h"
#include "./amx/vehicles.h"
#include "./amx/players.h" 
#include "./amx/wrapper.h" 
#include "./amx/plugin.h" 
#include "./amx/eventhandler.h"

class Drifter : public sampgdk::EventHandler 
{
public:
    Drifter();
    virtual ~Drifter();
	virtual void OnGameModeInit();
    virtual bool OnPlayerStateChange(int playerid, int newstate, int oldstate);
    virtual bool OnPlayerDisconnect(int playerid, int reason);
};

//
#define PLAYER_STATE_DRIVER			(2)
#define PLUGIN_PRIVATE_UPDATE_DRIFT (30)//ticks - weird that not so much ticks is 1 second? :O
#define MINIMAL_ANGLE				(12.5)//in degrees
#define MAXIMAL_ANGLE				(360.0)//in degrees
#define MINIMAL_SPEED				(20.0)//in meters / second
#define MAXIMAL_SPEED				(500.0)//in meters / second
#define DEFAULT_DMG					(0)
#define START_END_DELAY				(4)
#define DIVIDER						(350)
#define fENABLED					(1)
#define MAX_PLAYERS					(500)
#define PLUGIN_MAIN_VERSION			(22)
#define PLUGIN_SUB_VERSION			(0)
#define DEFAULT_MAX_HEALTH_LOOSE	(0.0);
#define DEFAULT_CHECK				(1)
#define DEFAULT_BACKWARDS			(1)

#define USE_VEHICLE_MODEL_CHECK
#define CHECK_MAX_VALUES
//

AMX * pAMX;

#define PI (314159265/100000000)

extern void * pAMXFunctions;

using namespace std;  
using namespace sampgdk;

static Drifter theGameMode;

Drifter::Drifter()
{
    this->Register();
}

Drifter::~Drifter()
{

}

int RegisterServer();
int RegisterServer()
{
    int amxerr = (-1),idx;
    amxerr = amx_FindPublic(pAMX, "PrivateDriftRegister", &idx);
    if (amxerr == AMX_ERR_NONE)
	{
		amx_Exec(pAMX, NULL, idx);
        return 1;
    }
    return 0;
}

void Drifter::OnGameModeInit()
{
	RegisterServer();
}

int GlobalPos_Timer[MAX_PLAYERS] = {-1};
int GlobalPos_Points[MAX_PLAYERS];
int GlobalPos_Combo[MAX_PLAYERS];
int flagcheckingstatus[MAX_PLAYERS] = {fENABLED};
int g_divider[MAX_PLAYERS] = {DIVIDER};
int DamageCheck[MAX_PLAYERS] = {DEFAULT_DMG};
int PlayerVehicleID[MAX_PLAYERS];
int Drifting[MAX_PLAYERS];
int Drifting_precise[MAX_PLAYERS];
int g_Ticked = 0;
int g_TickMax = PLUGIN_PRIVATE_UPDATE_DRIFT;
int didit = 0;
int DriftStartEndDelay[MAX_PLAYERS] = {START_END_DELAY};
int WasAdded[MAX_PLAYERS];
int GlobOnUpdateIndex = (-1);
int GlobOnStartIndex = (-1);
int GlobOnEndIndex = (-1);
int CheckHim[MAX_PLAYERS];
int CheckBackwards[MAX_PLAYERS];
int GlobalCheck = 1;

float GlobalPos_X[MAX_PLAYERS];
float GlobalPos_Y[MAX_PLAYERS];
float GlobalPos_Z[MAX_PLAYERS];
float GlobalPos_Angle1[MAX_PLAYERS];
float GlobalPos_Angle2[MAX_PLAYERS];
float g_MinSpeed[MAX_PLAYERS] = {MINIMAL_SPEED};
float g_MaxAngle[MAX_PLAYERS] = {MAXIMAL_ANGLE};
float g_MinAngle[MAX_PLAYERS] = {MINIMAL_ANGLE};
float g_MaxSpeed[MAX_PLAYERS] = {MAXIMAL_SPEED};
float VehicleHealth[MAX_PLAYERS];
float SpeedX[MAX_PLAYERS];
float SpeedY[MAX_PLAYERS];
float SpeedZ[MAX_PLAYERS];
float floatdata[MAX_PLAYERS][11];
float MaxHealthLoose[MAX_PLAYERS];

struct BonusData
{
	int enabled;
	float xPOS;
	float yPOS;
	float zPOS;
	BonusData(int n0,float n1,float n2,float n3):enabled(n0),xPOS(n1),yPOS(n2),zPOS(n3){}
}
;

vector <BonusData> BonusPoint;
vector<int> PlayerLoopList;

bool AllowedModelList[613];//Yes I know too much allocated but it is only for performance!

int OnPlayerDriftStart(int playerid);
int OnPlayerDriftStart(int playerid)
{
    if (!amx_FindPublic(pAMX, "OnPlayerDriftStart", &GlobOnStartIndex))
    {
		amx_Push(pAMX, playerid);
		amx_Exec(pAMX, NULL, GlobOnStartIndex);
        return 1;
    }
    return 0;
}

int OnPlayerDriftUpdate(int playerid,int points,int combo,int flagid,float distance,float speed);
int OnPlayerDriftUpdate(int playerid,int points,int combo,int flagid,float distance,float speed)
{
    if (!amx_FindPublic(pAMX, "OnPlayerDriftUpdate", &GlobOnUpdateIndex))
    {
		amx_Push(pAMX, amx_ftoc(speed));
		amx_Push(pAMX, amx_ftoc(distance));
		amx_Push(pAMX, flagid);
		amx_Push(pAMX, combo);
		amx_Push(pAMX, points);
		amx_Push(pAMX, playerid);
		amx_Exec(pAMX, NULL, GlobOnUpdateIndex);
        return 1;
    }
    return 0;
}

int OnPlayerDriftEnd(int playerid,int points,int combo,int reason);
int OnPlayerDriftEnd(int playerid,int points,int combo,int reason)
{
    if (!amx_FindPublic(pAMX, "OnPlayerDriftEnd", &GlobOnEndIndex))
    {
		amx_Push(pAMX, reason);
		amx_Push(pAMX, combo);
		amx_Push(pAMX, points);
		amx_Push(pAMX, playerid);
		amx_Exec(pAMX, NULL, GlobOnEndIndex);
        return 1;
    }
    return 0;
}

bool AddModelToAllowedList(int modelid);
bool AddModelToAllowedList(int modelid)
{
	if(modelid > 611 || modelid < 400)return 0;
	AllowedModelList[modelid] = true;
	return 1;
}

bool ClearAllowedModelList();
bool ClearAllowedModelList()
{
	for(int i = 0; i != 612; i++)
	{
		AllowedModelList[i] = false;
	}
	return true;
}

bool ResetAllowedModelList();
bool ResetAllowedModelList()
{
	for(int i = 0; i != 612; i++)
	{
		AllowedModelList[i] = true;
	}
	AllowedModelList[448] = false;
	AllowedModelList[461] = false;
	AllowedModelList[462] = false;
	AllowedModelList[468] = false;
	AllowedModelList[481] = false;
	AllowedModelList[509] = false;
	AllowedModelList[521] = false;
	AllowedModelList[522] = false;
	AllowedModelList[581] = false;
	AllowedModelList[537] = false;
	AllowedModelList[538] = false;
	AllowedModelList[523] = false;
	AllowedModelList[417] = false;
	AllowedModelList[425] = false;
	AllowedModelList[447] = false;
	AllowedModelList[460] = false;
	AllowedModelList[469] = false;
	AllowedModelList[476] = false;
	AllowedModelList[487] = false;
	AllowedModelList[488] = false;
	AllowedModelList[497] = false;
	AllowedModelList[511] = false;
	AllowedModelList[512] = false;
	AllowedModelList[513] = false;
	AllowedModelList[519] = false;
	AllowedModelList[520] = false;
	AllowedModelList[548] = false;
	AllowedModelList[553] = false;
	AllowedModelList[563] = false;
	AllowedModelList[577] = false;
	AllowedModelList[592] = false;
	AllowedModelList[593] = false;
	AllowedModelList[430] = false;
	AllowedModelList[446] = false;
	AllowedModelList[452] = false;
	AllowedModelList[454] = false;
	AllowedModelList[472] = false;
	AllowedModelList[473] = false;
	AllowedModelList[453] = false;
	AllowedModelList[484] = false;
	AllowedModelList[493] = false;
	AllowedModelList[595] = false;
	AllowedModelList[464] = false;
	return true;
}

bool RemoveModelFromAllowedList(int modelid);
bool RemoveModelFromAllowedList(int modelid)
{
	if(modelid > 611 || modelid < 400)return 0;
	AllowedModelList[modelid] = false;
	return 1;
}

bool IsDriftingAllowedModel(int modelid);
bool IsDriftingAllowedModel(int modelid)
{
	return AllowedModelList[modelid];
}

static cell AMX_NATIVE_CALL SetDriftUpdateDelay( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL SetDriftUpdateDelay( AMX* amx, cell* params )
{
	if(params[1] > 100)
	{
		g_TickMax = 100;
		return 1;
	}
	if(params[1] < 5)
	{
		g_TickMax = 5;
		return 1;
	}
	g_TickMax = params[1];
	return 1;
}

static cell AMX_NATIVE_CALL SetDriftDivider( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL SetDriftDivider( AMX* amx, cell* params )
{
	int gg_divider = params[1];
	if(params[1] > 5000)
	{
		gg_divider = 5000;
	}
	if(params[1] < 1)
	{
		gg_divider = 1;
	}
	if(params[2] == (-1))
	{
		int playerid = 0;
		while(playerid != MAX_PLAYERS-1)
		{
			g_divider[playerid] = gg_divider;
			playerid++;
		}
	}
	else
	{
		g_divider[params[2]] = gg_divider;
	}
	return 1;
}

static cell AMX_NATIVE_CALL SetDriftMinAngle( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL SetDriftMinAngle( AMX* amx, cell* params )
{
	float val = amx_ctof(params[1]);
	float gg_MinAngle = val;
	if(val > 90.0)
	{
		gg_MinAngle = 90.0;
	}
	if(val < 0.0)
	{
		gg_MinAngle = 0.0;
	}
	if(params[2] == (-1))
	{
		int playerid = 0;
		while(playerid != MAX_PLAYERS-1)
		{
			g_MinAngle[playerid] = gg_MinAngle;
			playerid++;
		}
	}
	else
	{
		g_MinAngle[params[2]] = gg_MinAngle;
	}
	return 1;
}

static cell AMX_NATIVE_CALL SetDriftMaxAngle( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL SetDriftMaxAngle( AMX* amx, cell* params )
{
	float val = amx_ctof(params[1]);
	float gg_MaxAngle = val;
	if(val > 180.0)
	{
		gg_MaxAngle = 180.0;
	}
	if(val < 0.0)
	{
		gg_MaxAngle = 0.0;
	}
	if(params[2] == (-1))
	{
		int playerid = 0;
		while(playerid != MAX_PLAYERS-1)
		{
			g_MaxAngle[playerid] = gg_MaxAngle;
			playerid++;
		}
	}
	else
	{
		g_MaxAngle[params[2]] = gg_MaxAngle;
	}
	return 1;
}

static cell AMX_NATIVE_CALL SetDriftMinSpeed( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL SetDriftMinSpeed( AMX* amx, cell* params )
{
	float val = amx_ctof(params[1]);
	float gg_MaxAngle = val;
	if(val > 1000.0)
	{
		gg_MaxAngle = 1000.0;
	}
	if(val < 0.1)
	{
		gg_MaxAngle = 0.1f;
	}
	if(params[2] == (-1))
	{
		int playerid = 0;
		while(playerid != MAX_PLAYERS-1)
		{
			g_MinSpeed[playerid] = gg_MaxAngle;
			playerid++;
		}
	}
	else
	{
		g_MinSpeed[params[2]] = gg_MaxAngle;
	}
	return 1;
}

static cell AMX_NATIVE_CALL SetDriftMaxSpeed( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL SetDriftMaxSpeed( AMX* amx, cell* params )
{
	float val = amx_ctof(params[1]);
	float gg_MaxAngle = val;
	if(val > 1000.0)
	{
		gg_MaxAngle = 1000.0;
	}else
	if(val < 0.1)
	{
		gg_MaxAngle = 0.1f;
	}
	if(params[2] == (-1))
	{
		int playerid = 0;
		while(playerid != MAX_PLAYERS-1)
		{
			g_MaxSpeed[playerid] = gg_MaxAngle;
			playerid++;
		}
	}
	else
	{
		g_MaxSpeed[params[2]] = gg_MaxAngle;
	}
	return 1;
}

static cell AMX_NATIVE_CALL n_DriftStartEndDelay( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL n_DriftStartEndDelay( AMX* amx, cell* params )
{
	int gg_divider = params[1];
	if(params[1] > 1000)
	{
		gg_divider = 1000;
	}
	if(params[1] <= 1)
	{
		gg_divider = 1;
	}
	if(params[2] == (-1))
	{
		int playerid = 0;
		while(playerid != MAX_PLAYERS-1)
		{
			DriftStartEndDelay[playerid] = gg_divider;
			playerid++;
		}
	}
	else
	{
		DriftStartEndDelay[params[2]] = gg_divider;
	}
	return 1;
}

static cell AMX_NATIVE_CALL GetDriftMinSpeed( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL GetDriftMinSpeed( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[1], &cptr);
	*cptr = amx_ftoc(g_MinSpeed[params[2]]);
	return 1;
}

static cell AMX_NATIVE_CALL GetDriftMaxSpeed( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL GetDriftMaxSpeed( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[1], &cptr);
	*cptr = amx_ftoc(g_MaxSpeed[params[2]]);
	return 1;
}

static cell AMX_NATIVE_CALL GetDriftMaxAngle( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL GetDriftMaxAngle( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[1], &cptr);
	*cptr = amx_ftoc(g_MaxAngle[params[2]]);
	return 1;
}

static cell AMX_NATIVE_CALL GetDriftMinAngle( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL GetDriftMinAngle( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[1], &cptr);
	*cptr = amx_ftoc(g_MinAngle[params[2]]);
	return 1;
}

static cell AMX_NATIVE_CALL n_GetDriftStartEndDelay( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL n_GetDriftStartEndDelay( AMX* amx, cell* params )
{
	return DriftStartEndDelay[params[1]];
}

static cell AMX_NATIVE_CALL GetDriftUpdateDelay( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL GetDriftUpdateDelay( AMX* amx, cell* params )
{
	return g_TickMax;
}

static cell AMX_NATIVE_CALL GetDriftDivider( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL GetDriftDivider( AMX* amx, cell* params )
{
	return g_divider[params[1]];
}

static cell AMX_NATIVE_CALL SetDamageCheck( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL SetDamageCheck( AMX* amx, cell* params )
{
	int gg_divider = params[1];
	float MLH = amx_ctof(params[3]);
	if(MLH < 0.0)
	{
		MLH = 0.0;
	}
	if(MLH > 1000.0)
	{
		MLH = 1000.0;
	}
	if(params[1] >= 1)
	{
		gg_divider = 1;
	}
	if(params[1] <= 0)
	{
		gg_divider = 0;
	}
	if(params[2] == (-1))
	{
		int playerid = 0;
		while(playerid != MAX_PLAYERS-1)
		{
			DamageCheck[playerid] = gg_divider;
			MaxHealthLoose[playerid] = MLH;
			playerid++;
		}
	}
	else
	{
		DamageCheck[params[2]] = gg_divider;
		MaxHealthLoose[params[2]] = MLH;
	}
	return 1;
}

static cell AMX_NATIVE_CALL SetGlobalCheck( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL SetGlobalCheck( AMX* amx, cell* params )
{
	int gg_divider = params[1];
	if(params[1] >= 1)
	{
		gg_divider = 1;
	}
	if(params[1] <= 0)
	{
		gg_divider = 0;
	}
	GlobalCheck = gg_divider;
	return 1;
}

static cell AMX_NATIVE_CALL SetPlayerCheck( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL SetPlayerCheck( AMX* amx, cell* params )
{
	int gg_divider = params[2];
	if(params[2] >= 1)
	{
		gg_divider = 1;
	}
	if(params[2] <= 0)
	{
		gg_divider = 0;
	}
	if(params[1] == (-1))
	{
		int playerid = 0;
		while(playerid != MAX_PLAYERS-1)
		{
			CheckHim[playerid] = gg_divider;
			playerid++;
		}
	}
	else
	{
		CheckHim[params[1]] = gg_divider;
	}
	return 1;
}

static cell AMX_NATIVE_CALL GetPlayerCheck( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL GetPlayerCheck( AMX* amx, cell* params )
{
	return CheckHim[params[1]];
}

static cell AMX_NATIVE_CALL SetBackwardsCheck( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL SetBackwardsCheck( AMX* amx, cell* params )
{
	int gg_divider = params[2];
	if(params[2] >= 1)
	{
		gg_divider = 1;
	}
	if(params[2] <= 0)
	{
		gg_divider = 0;
	}
	if(params[1] == (-1))
	{
		int playerid = 0;
		while(playerid != MAX_PLAYERS-1)
		{
			CheckBackwards[playerid] = gg_divider;
			playerid++;
		}
	}
	else
	{
		CheckBackwards[params[1]] = gg_divider;
	}
	return 1;
}

static cell AMX_NATIVE_CALL GetBackwardsCheck( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL GetBackwardsCheck( AMX* amx, cell* params )
{
	return CheckBackwards[params[1]];
}

static cell AMX_NATIVE_CALL GetGlobalCheck( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL GetGlobalCheck( AMX* amx, cell* params )
{
	return GlobalCheck;
}

static cell AMX_NATIVE_CALL GetDamageCheck( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL GetDamageCheck( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[2], &cptr);
	*cptr = amx_ftoc(MaxHealthLoose[params[1]]);
	return DamageCheck[params[1]];
}

static cell AMX_NATIVE_CALL IsPlayerDrifting( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL IsPlayerDrifting( AMX* amx, cell* params )
{
	return Drifting[params[1]];
}

static cell AMX_NATIVE_CALL IsPlayerDrifting_precise( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL IsPlayerDrifting_precise( AMX* amx, cell* params )
{
	return Drifting_precise[params[1]];
}

static cell AMX_NATIVE_CALL AddFLAG( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL AddFLAG( AMX* amx, cell* params )
{
	float posx = amx_ctof(params[1]);
	float posy = amx_ctof(params[2]);
	float posz = amx_ctof(params[3]);
	for(int i = 0,j = BonusPoint.size(); i < j; i++)
	{
		if(BonusPoint[i].enabled == -1)
		{
			BonusPoint[i].enabled = 1;
			BonusPoint[i].xPOS = posx;
			BonusPoint[i].yPOS = posy;
			BonusPoint[i].zPOS = posz;
			return i;
		}
	}
	BonusPoint.push_back(BonusData(1,posx,posy,posz));
	return BonusPoint.size() - 1;
}

static cell AMX_NATIVE_CALL DisableFLAG( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL DisableFLAG( AMX* amx, cell* params )
{
	BonusPoint[params[1]].enabled = 0;
	return 1;
}

static cell AMX_NATIVE_CALL DeleteFLAG( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL DeleteFLAG( AMX* amx, cell* params )
{
	BonusPoint[params[1]].enabled = -1;
	return 1;
}

static cell AMX_NATIVE_CALL EnableFLAG( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL EnableFLAG( AMX* amx, cell* params )
{
	BonusPoint[params[1]].enabled = 1;
	return 1;
}

static cell AMX_NATIVE_CALL GetPluginVersion( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL GetPluginVersion( AMX* amx, cell* params )
{
	return PLUGIN_MAIN_VERSION;
}

static cell AMX_NATIVE_CALL MoveFLAG( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL MoveFLAG( AMX* amx, cell* params )
{
	float posx = amx_ctof(params[2]);
	float posy = amx_ctof(params[3]);
	float posz = amx_ctof(params[4]);
	BonusPoint[params[1]].xPOS = posx;
	BonusPoint[params[1]].yPOS = posy;
	BonusPoint[params[1]].zPOS = posz;
	return 1;
}

int PrivChangeState( int playerid, int newstate);
int PrivChangeState( int playerid, int newstate)
{
	if(newstate == PLAYER_STATE_DRIVER && WasAdded[playerid] == 0)
	{
		WasAdded[playerid] = 1;
		int size = PlayerLoopList.size();
		for (int index=0; index < size; ++index)
		{
			if(PlayerLoopList.at(index) == playerid)
			{
				return 1;//avoid double adding
			}
		}
		PlayerLoopList.push_back(playerid);
		return 1;
	}
	else
	if(WasAdded[playerid] == 1)
	{
		WasAdded[playerid] = 0;
		int size = PlayerLoopList.size();
		for (int index=0; index < size; ++index)
		{
			if(PlayerLoopList.at(index) == playerid)
			{
				PlayerLoopList.erase(PlayerLoopList.begin()+index);
				if(Drifting[playerid] == 1)
				{
		 			if(GlobalPos_Timer[playerid] != -1)
		 			{
						Drifting_precise[playerid] = 0;
		 				GlobalPos_Timer[playerid] = -1;
						OnPlayerDriftEnd(playerid,GlobalPos_Points[playerid],GlobalPos_Combo[playerid],0);
						GlobalPos_Combo[playerid] = 0;
						GlobalPos_Points[playerid] = 0;
						Drifting[playerid] = 0;
						return 3;
		 			}
					return 2;
				}
				return 1;
			}
		}
		return 0;
	}
	return 1;
}

static cell AMX_NATIVE_CALL SetCheckForFlags( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL SetCheckForFlags( AMX* amx, cell* params )
{
	int gg_divider = params[1];
	if(params[1] >= 1)
	{
		gg_divider = 1;
	}
	if(params[1] <= 0)
	{
		gg_divider = 0;
	}
	if(params[2] == (-1))
	{
		int playerid = 0;
		while(playerid != MAX_PLAYERS-1)
		{
			flagcheckingstatus[playerid] = gg_divider;
			playerid++;
		}
	}
	else
	{
		flagcheckingstatus[params[2]] = gg_divider;
	}
	return 1;
}

static cell AMX_NATIVE_CALL GetCheckForFlags( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL GetCheckForFlags( AMX* amx, cell* params )
{
	return flagcheckingstatus[params[1]];
}

static cell AMX_NATIVE_CALL SetDriftMaxHealthLoose( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL SetDriftMaxHealthLoose( AMX* amx, cell* params )
{
	float val = amx_ctof(params[1]);
	float gg_MinAngle = val;
	if(val >= 1000.0)
	{
		gg_MinAngle = 1000.0;
	}
	if(val <= 0.0)
	{
		gg_MinAngle = 0.0;
	}
	if(params[2] == (-1))
	{
		int playerid = 0;
		while(playerid != MAX_PLAYERS-1)
		{
			MaxHealthLoose[playerid] = gg_MinAngle;
			playerid++;
		}
	}
	else
	{
		MaxHealthLoose[params[2]] = gg_MinAngle;
	}
	return 1;
}

static cell AMX_NATIVE_CALL GetDriftMaxHealthLoose( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL GetDriftMaxHealthLoose( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[1], &cptr);
	*cptr = amx_ftoc(MaxHealthLoose[params[2]]);
	return 1;
}

static cell AMX_NATIVE_CALL AddDriftModel( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL DelDriftModel( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL ClearModelLst( AMX* amx, cell* params );
static cell AMX_NATIVE_CALL ResetModelLst( AMX* amx, cell* params );

static cell AMX_NATIVE_CALL AddDriftModel( AMX* amx, cell* params )
{
	return AddModelToAllowedList(params[1]);
}

static cell AMX_NATIVE_CALL DelDriftModel( AMX* amx, cell* params )
{
	return RemoveModelFromAllowedList(params[1]);
}

static cell AMX_NATIVE_CALL ClearModelLst( AMX* amx, cell* params )
{
	return ClearAllowedModelList();
}

static cell AMX_NATIVE_CALL ResetModelLst( AMX* amx, cell* params )
{
	return ResetAllowedModelList();
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load( void **ppData ) 
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	Wrapper::GetInstance()->Initialize(ppData);
	int loop = 0;
	while(loop != MAX_PLAYERS-1)
	{
		GlobalPos_Timer[loop] = (-1);
		g_MinSpeed[loop] = MINIMAL_SPEED;
		g_MaxAngle[loop] = MAXIMAL_ANGLE;
		g_MinAngle[loop] = MINIMAL_ANGLE;
		g_MaxSpeed[loop] = MAXIMAL_SPEED;
		g_divider[loop] = DIVIDER;
		DamageCheck[loop] = DEFAULT_DMG;
		CheckHim[loop] = DEFAULT_CHECK;
		CheckBackwards[loop] = DEFAULT_BACKWARDS;
		flagcheckingstatus[loop] = fENABLED;
		DriftStartEndDelay[loop] = START_END_DELAY;
		MaxHealthLoose[loop] = DEFAULT_MAX_HEALTH_LOOSE;
		loop++;
	}
	ResetAllowedModelList();
	cout << "    Drift Counter Plugin 2011 by Gamer_Z loaded";
	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload( )
{
	cout << "    Drift Counter Plugin 2011 by Gamer_Z unloaded";
}

AMX_NATIVE_INFO driftAMXNatives[ ] =
{
	{"DriftSet_UpdateDelay", SetDriftUpdateDelay},
	{"DriftSet_MinimalSpeed", SetDriftMinSpeed},
	{"DriftSet_MinimalAngle", SetDriftMinAngle},
	#if defined CHECK_MAX_VALUES
	{"DriftSet_MaximalSpeed", SetDriftMaxSpeed},
	{"DriftSet_MaximalAngle", SetDriftMaxAngle},

	{"DriftGet_MaximalSpeed", GetDriftMaxSpeed},
	{"DriftGet_MaximalAngle", GetDriftMaxAngle},
	#endif
	{"DriftSet_StartEndDelay", n_DriftStartEndDelay},

	{"DriftGet_StartEndDelay", n_GetDriftStartEndDelay},
	{"DriftGet_UpdateDelay", GetDriftUpdateDelay},

	{"DriftGet_MinimalSpeed", GetDriftMinSpeed},
	{"DriftGet_MinimalAngle", GetDriftMinAngle},

	{"DriftSet_Divider", SetDriftDivider},
	{"DriftGet_Divider", GetDriftDivider},

	{"DriftSet_DamageCheck", SetDamageCheck},
	{"DriftGet_DamageCheck", GetDamageCheck},

	{"IsPlayerDrifting", IsPlayerDrifting},
	{"IsPlayerDrifting_precise", IsPlayerDrifting_precise},

	{"Drift_AddFLAG",AddFLAG},
	{"Drift_DisableFLAG",DisableFLAG},
	{"Drift_DeleteFLAG",DeleteFLAG},
	{"Drift_EnableFLAG",EnableFLAG},
	{"Drift_MoveFLAG",MoveFLAG},

	{"Drift_SetCheckForFlags",SetCheckForFlags},
	{"Drift_GetCheckForFlags",GetCheckForFlags},

	//

	{"Drift_SetGlobalCheck", SetGlobalCheck},
	{"Drift_GetGlobalCheck", GetGlobalCheck},

	{"Drift_SetPlayerCheck", SetPlayerCheck},
	{"Drift_GetPlayerCheck", GetPlayerCheck},

	{"Drift_SetBackwardsCheck", SetBackwardsCheck},
	{"Drift_GetBackwardsCheck", GetBackwardsCheck},

	{"Drift_SetDriftMaxHealthLoose", SetDriftMaxHealthLoose},
	{"Drift_GetDriftMaxHealthLoose", GetDriftMaxHealthLoose},

	//
	{"Private_DriftGetVersion",GetPluginVersion},

	{"Drift_AllowModel",AddDriftModel},
	{"Drift_DisallowModel",DelDriftModel},
	{"Drift_ClearModelList",ClearModelLst},
	{"Drift_ResetDefaultModelList",ResetModelLst},

	{0,                0}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad( AMX *amx ) 
{
	pAMX = amx;
	return amx_Register( amx, driftAMXNatives, -1 );
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload( AMX *amx ) 
{
	return AMX_ERR_NONE;
}

bool Drifter::OnPlayerStateChange(int playerid, int newstate, int oldstate)
{
	PrivChangeState( playerid, newstate);
	return true;
}

bool Drifter::OnPlayerDisconnect(int playerid, int reason)
{
	PrivChangeState( playerid, 0);
	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL
	ProcessTick()
{
	if(GlobalCheck == 1)
	{
		if(g_Ticked == g_TickMax)
		{
			int playerid = 0;
			int size = PlayerLoopList.size();
			int index = 0;
			for (index = 0; index < size; ++index)
			{
				playerid = PlayerLoopList.at(index);
				if(CheckHim[playerid] == 0)continue;
				if(IsPlayerConnected(playerid) == false)//anti crash
				{
					PrivChangeState( playerid, 0);
					continue;
				}
				//logprintf("1");
				PlayerVehicleID[playerid] = GetPlayerVehicleID(playerid);
				//logprintf("2");
				if(DamageCheck[playerid] == 1 && Drifting[playerid] == 1)
				{
					float tempheal;
					GetVehicleHealth(PlayerVehicleID[playerid], tempheal);
					//logprintf("3");
					if(VehicleHealth[playerid] != tempheal)
					{
						if(MaxHealthLoose[playerid] < abs(VehicleHealth[playerid]-tempheal))
						{
							//logprintf("4");
		 					GlobalPos_Timer[playerid] = (-1);
							OnPlayerDriftEnd(playerid,GlobalPos_Points[playerid],GlobalPos_Combo[playerid],1);
							GlobalPos_Combo[playerid] = 0;
							GlobalPos_Points[playerid] = 0;
							Drifting[playerid] = 0;
						}
						else
						{
							VehicleHealth[playerid] = tempheal;
						}
						continue;
					}
				}
				//logprintf("5");
				GetVehicleVelocity(PlayerVehicleID[playerid], SpeedX[playerid], SpeedY[playerid], SpeedZ[playerid]);
				floatdata[playerid][3] = GlobalPos_Angle1[playerid];
				GetVehicleZAngle(PlayerVehicleID[playerid], GlobalPos_Angle1[playerid]);
				//logprintf("6");

				bool forward = true;
				if(CheckBackwards[playerid] == 1)
				{
					if(GlobalPos_Angle1[playerid] < 90)
					{
						if(SpeedX[playerid] > 0 && SpeedY[playerid] < 0) forward = false;
					}
					else if(GlobalPos_Angle1[playerid] < 180)
					{
						if(SpeedX[playerid] > 0 && SpeedY[playerid] > 0) forward = false;
					}
					else if(GlobalPos_Angle1[playerid] < 270)
					{
						if(SpeedX[playerid] < 0 && SpeedY[playerid] > 0) forward = false;
					}
					else if(SpeedX[playerid] < 0 && SpeedY[playerid] < 0) forward = false;
				}
				//logprintf("7");

				floatdata[playerid][10] = sqrt(pow(SpeedX[playerid], 2)+pow(SpeedY[playerid], 2)+pow(SpeedZ[playerid], 2))*274;
				GetPlayerPos(playerid, floatdata[playerid][4], floatdata[playerid][5], floatdata[playerid][6]);
				floatdata[playerid][1] = sqrt(pow((floatdata[playerid][4]-GlobalPos_X[playerid]),2)+pow((floatdata[playerid][5]-GlobalPos_Y[playerid]),2));

				//logprintf("8");

				if(floatdata[playerid][4]>GlobalPos_X[playerid]){floatdata[playerid][7]=floatdata[playerid][4]-GlobalPos_X[playerid];}else{floatdata[playerid][7]=GlobalPos_X[playerid]-floatdata[playerid][4];}
			
				if(floatdata[playerid][5]>GlobalPos_Y[playerid]){floatdata[playerid][8]=floatdata[playerid][5]-GlobalPos_Y[playerid];}else{floatdata[playerid][8]=GlobalPos_Y[playerid]-floatdata[playerid][5];}
			
				if(GlobalPos_Y[playerid]>floatdata[playerid][5] && GlobalPos_X[playerid]>floatdata[playerid][4]){ //1
					floatdata[playerid][0] = asin(floatdata[playerid][7]/floatdata[playerid][1])* 180 / PI;
					floatdata[playerid][9] = (floatdata[playerid][0]+90)-(floatdata[playerid][0]*2)+90;
				}
				if(GlobalPos_Y[playerid]<floatdata[playerid][5] && GlobalPos_X[playerid]>floatdata[playerid][4]){ //2
					floatdata[playerid][0] = asin(floatdata[playerid][7]/floatdata[playerid][1])* 180 / PI;
					floatdata[playerid][9] = (floatdata[playerid][0]+180)-180;
				}
				if(GlobalPos_Y[playerid]<floatdata[playerid][5] && GlobalPos_X[playerid]<floatdata[playerid][4]){ //3
					floatdata[playerid][0] = acos(floatdata[playerid][8]/floatdata[playerid][1])* 180 / PI;
					floatdata[playerid][9] = (floatdata[playerid][0]+360)-(floatdata[playerid][0]*2);
				}
				if(GlobalPos_Y[playerid]>floatdata[playerid][5] && GlobalPos_X[playerid]<floatdata[playerid][4]){ //4
					floatdata[playerid][0] = asin(floatdata[playerid][7]/floatdata[playerid][1])* 180 / PI;
					floatdata[playerid][9] = floatdata[playerid][0]+180;
				}
				if(floatdata[playerid][9] == 0.0)
				{
					GlobalPos_Angle2[playerid] =  GlobalPos_Angle1[playerid];
				} else {
					GlobalPos_Angle2[playerid]  =  floatdata[playerid][9];
				}
				//logprintf("9");

				float angle = abs(GlobalPos_Angle1[playerid]-GlobalPos_Angle2[playerid]);
				if(
						#if defined CHECK_MAX_VALUES
						g_MaxAngle[playerid]  > angle && 
						#endif
						angle > g_MinAngle[playerid] && 
						floatdata[playerid][10] > g_MinSpeed[playerid] 
						#if defined CHECK_MAX_VALUES
						&& floatdata[playerid][10] < g_MaxSpeed[playerid]
						#endif
						&& forward == true
						#if defined USE_VEHICLE_MODEL_CHECK
						&& AllowedModelList[GetVehicleModel(PlayerVehicleID[playerid])] == true
						#endif
				)
		 		{
					//logprintf("10");
					GlobalPos_Points[playerid] += (int)(floor(angle * floatdata[playerid][10])/g_divider[playerid]);
					if(GlobalPos_Timer[playerid] != -1)
					{
						//logprintf("11");
						Drifting_precise[playerid] = 1;
						GlobalPos_Combo[playerid] += 1;
						if(flagcheckingstatus[playerid] == 1)
						{
							didit = 0;
							for(int i = 0,j = BonusPoint.size(); i < j; i++)
							{
								if(BonusPoint[i].enabled == 1)
								{
									float dist = sqrt(pow(BonusPoint[i].xPOS-floatdata[playerid][4],2)+pow(BonusPoint[i].yPOS-floatdata[playerid][5],2)+pow(BonusPoint[i].zPOS-floatdata[playerid][6],2));
									if(dist < 10.0)
									{
										OnPlayerDriftUpdate(playerid,GlobalPos_Points[playerid],GlobalPos_Combo[playerid],i,dist,floatdata[playerid][10]);
										didit = 1;
									}
								}
							}
							if(didit == 0)
							{
								OnPlayerDriftUpdate(playerid,GlobalPos_Points[playerid],GlobalPos_Combo[playerid],-1,-1.0,floatdata[playerid][10]);
							}
						}
						else
						{
							OnPlayerDriftUpdate(playerid,GlobalPos_Points[playerid],GlobalPos_Combo[playerid],-1,-1.0,floatdata[playerid][10]);
						}
					}
					else
					{
						//logprintf("10b");
						Drifting[playerid] = 1;
						GetVehicleHealth(PlayerVehicleID[playerid], VehicleHealth[playerid]);
						//logprintf("10c");
						OnPlayerDriftStart(playerid);
						//logprintf("10d");
					}
					GlobalPos_Timer[playerid] = DriftStartEndDelay[playerid];
		 		}
		 		else
		 		{
		 			if(GlobalPos_Timer[playerid] != -1)
		 			{
						Drifting_precise[playerid] = 0;
		 				GlobalPos_Timer[playerid]--;
		 				if(GlobalPos_Timer[playerid] == -1)
		 				{
							OnPlayerDriftEnd(playerid,GlobalPos_Points[playerid],GlobalPos_Combo[playerid],0);
							GlobalPos_Combo[playerid] = 0;
							GlobalPos_Points[playerid] = 0;
							Drifting[playerid] = 0;
		 				}
		 			}
		 		}
				GlobalPos_X[playerid] = floatdata[playerid][4];
				GlobalPos_Y[playerid] = floatdata[playerid][5];
				GlobalPos_Z[playerid] = floatdata[playerid][6];
				//logprintf("12");
			}
			g_Ticked = -1;
		}
		g_Ticked += 1;
	}
}