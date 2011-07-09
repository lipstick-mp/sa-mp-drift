/*
* Default Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

/*
* AMX Includes
*/
#include "./SDK/amx/amx.h"
#include "./SDK/plugin.h"
/*
* Includes from Third Partys'
*/
//#pragma warning(disable:4244)

#include "./ThirdPartySource/Invoke.cpp"//
//#include "./ThirdPartySource/samp.cpp"//ZeeX

AMX * pAMX;

#define PI (314159265/100000000)

extern void * pAMXFunctions;

using namespace std;  

#define MAX_PLAYERS (500)

float GlobalPos_X[MAX_PLAYERS];
float GlobalPos_Y[MAX_PLAYERS];
float GlobalPos_Z[MAX_PLAYERS];

float GlobalPos_Angle1[MAX_PLAYERS];
float GlobalPos_Angle2[MAX_PLAYERS];

int GlobalPos_Timer[MAX_PLAYERS] = {-1};
int GlobalPos_Points[MAX_PLAYERS];
int GlobalPos_Combo[MAX_PLAYERS];

#define PLAYER_STATE_DRIVER	(2)

#define PLUGIN_PRIVATE_UPDATE_DRIFT (30/*10000*/)//ticks - weird that not so much ticks is 1 second? :O

#define MINIMAL_ANGLE (25.5)//in degrees
#define MAXIMAL_ANGLE (360.0)//in degrees
#define MINIMAL_SPEED (15.0)//in meters / second
#define MAXIMAL_SPEED (500.0)//in meters / second

#define START_END_DELAY (4)
#define DIVIDER (350)

float g_MinSpeed = MINIMAL_SPEED;
float g_MaxAngle = MAXIMAL_ANGLE;
float g_MinAngle = MINIMAL_ANGLE;
float g_MaxSpeed = MAXIMAL_SPEED;
int   g_divider = DIVIDER;

int PlayerVehicleID[MAX_PLAYERS];

float SpeedX[MAX_PLAYERS];
float SpeedY[MAX_PLAYERS];
float SpeedZ[MAX_PLAYERS];

float floatdata[MAX_PLAYERS][11];

int DriftStartEndDelay = START_END_DELAY;
//#define USE_VEHICLE_MODEL_CHECK
//#define CHECK_MAX_VALUES

int g_Ticked = 0;
int g_TickMax = PLUGIN_PRIVATE_UPDATE_DRIFT;
#if defined USE_VEHICLE_MODEL_CHECK
int PlayerVehicleModelID[playerid];
#endif


int OnPlayerDriftStart(int playerid);
int OnPlayerDriftStart(int playerid)
{
    //if (playerid == NULL)
    //    return 0;
    int idx; 
    cell ret;
    int amxerr = amx_FindPublic(pAMX, "OnPlayerDriftStart", &idx);
    if (amxerr == AMX_ERR_NONE)
    {
		amx_Push(pAMX, playerid);
		amx_Exec(pAMX, &ret, idx);
        return 1;
    }
    return 0;
}

int OnPlayerDriftUpdate(int playerid,int points,int combo);
int OnPlayerDriftUpdate(int playerid,int points,int combo)
{
    //if (playerid == NULL)
    //    return 0;
    int idx; 
    cell ret;
    int amxerr = amx_FindPublic(pAMX, "OnPlayerDriftUpdate", &idx);
    if (amxerr == AMX_ERR_NONE)
    {
		amx_Push(pAMX, combo);
		amx_Push(pAMX, points);
		amx_Push(pAMX, playerid);
		amx_Exec(pAMX, &ret, idx);
        return 1;
    }
    return 0;
}

int OnPlayerDriftEnd(int playerid,int points,int combo);
int OnPlayerDriftEnd(int playerid,int points,int combo)
{
    //if (playerid == NULL)
    //    return 0;
    int idx; 
    cell ret;
    int amxerr = amx_FindPublic(pAMX, "OnPlayerDriftEnd", &idx);
    if (amxerr == AMX_ERR_NONE)
    {
		amx_Push(pAMX, combo);
		amx_Push(pAMX, points);
		amx_Push(pAMX, playerid);
		amx_Exec(pAMX, &ret, idx);
        return 1;
    }
    return 0;
}

PLUGIN_EXPORT void PLUGIN_CALL
	ProcessTick()
{
	if(g_Ticked == g_TickMax)
	{
		int playerid = 0;
		while(playerid != (MAX_PLAYERS-1))
		{
			if(g_Invoke->callNative(&PAWN::GetPlayerState,playerid) == PLAYER_STATE_DRIVER)
			{
				PlayerVehicleID[playerid] = g_Invoke->callNative(&PAWN::GetPlayerVehicleID,playerid);
				#if defined USE_VEHICLE_MODEL_CHECK
				PlayerVehicleModelID[playerid] = Invoke::callNative(&PAWN::GetVehicleModel,PlayerVehicleID);

				switch(PlayerVehicleModelID[playerid])
				{
				case 443,448,461 .. 463,521 .. 523,581,586,481,509,510,430,446,452 .. 454,472,473,484,493,595,417,425,447,465,469,487,488,497,501,548,563,406,444,556,557,573,460,464,476,511 .. 513,519,520,539,553,577,592,593,471:
			{
				#endif
				g_Invoke->callNative(&PAWN::GetVehicleVelocity,PlayerVehicleID[playerid], &SpeedX[playerid], &SpeedY[playerid], &SpeedZ[playerid]);
				floatdata[playerid][3] = GlobalPos_Angle1[playerid];
				g_Invoke->callNative(&PAWN::GetVehicleZAngle, PlayerVehicleID[playerid], &GlobalPos_Angle1[playerid]);
				bool forward = true;
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

				g_Invoke->callNative(&PAWN::GetPlayerPos, playerid, &floatdata[playerid][4], &floatdata[playerid][5], &floatdata[playerid][6]);
				floatdata[playerid][10] = sqrt(pow(SpeedX[playerid], 2)+pow(SpeedY[playerid], 2)+pow(SpeedZ[playerid], 2))*274;
				floatdata[playerid][1] = sqrt(pow((floatdata[playerid][4]-GlobalPos_X[playerid]),2)+pow((floatdata[playerid][5]-GlobalPos_Y[playerid]),2));


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
				//if(playerid == 0)cout << "forward playerid 0 is: " << forward;
				float angle = abs(GlobalPos_Angle1[playerid]-GlobalPos_Angle2[playerid]);
				if(
						#if defined CHECK_MAX_VALUES
						g_MaxAngle  > angle && 
						#endif
						angle > g_MinAngle && 
						floatdata[playerid][10] > g_MinSpeed 
						#if defined CHECK_MAX_VALUES
						&& floatdata[playerid][10] < g_MaxSpeed
						#endif
						&& forward == true
				)
		 		{
					GlobalPos_Points[playerid] += (int)(floor(angle * floatdata[playerid][10])/g_divider);
					if(GlobalPos_Timer[playerid] != -1)
					{
						GlobalPos_Combo[playerid] += 1;
						OnPlayerDriftUpdate(playerid,GlobalPos_Points[playerid],GlobalPos_Combo[playerid]);				
					}
					else
					{
						OnPlayerDriftStart(playerid);
					}
					GlobalPos_Timer[playerid] = DriftStartEndDelay;
		 		}
		 		else
		 		{
		 			if(GlobalPos_Timer[playerid] != -1)
		 			{
		 				GlobalPos_Timer[playerid]--;
		 				if(GlobalPos_Timer[playerid] == -1)
		 				{
							OnPlayerDriftEnd(playerid,GlobalPos_Points[playerid],GlobalPos_Combo[playerid]);
							GlobalPos_Combo[playerid] = 0;
							GlobalPos_Points[playerid] = 0;
		 				}
		 			}
		 		}
				#if defined USE_VEHICLE_MODEL_CHECK
			}
				}
				#endif
				GlobalPos_X[playerid] = floatdata[playerid][4];
				GlobalPos_Y[playerid] = floatdata[playerid][5];
				GlobalPos_Z[playerid] = floatdata[playerid][6];
			}
			playerid++;
		}
		g_Ticked = -1;
	}
	g_Ticked += 1;
}

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

static cell AMX_NATIVE_CALL SetDriftDivider( AMX* amx, cell* params )
{
	if(params[1] > 5000)
	{
		g_divider = 5000;
		return 1;
	}
	if(params[1] < 1)
	{
		g_divider = 1;
		return 1;
	}
	g_divider = params[1];
	return 1;
}

static cell AMX_NATIVE_CALL SetDriftMinAngle( AMX* amx, cell* params )
{
	float val = amx_ctof(params[1]);
	if(val > 90.0)
	{
		g_MinAngle = 90.0;
		return 1;
	}
	if(val < 0.0)
	{
		g_MinAngle = 0.0;
		return 1;
	}
	g_MinAngle = val;
	return 1;
}

static cell AMX_NATIVE_CALL SetDriftMaxAngle( AMX* amx, cell* params )
{
	float val = amx_ctof(params[1]);
	if(val > 89.0)
	{
		g_MaxAngle = 89.0;
		return 1;
	}
	if(val < 0.1)
	{
		g_MaxAngle = 0.1f;
		return 1;
	}
	g_MaxAngle = val;
	return 1;
}

static cell AMX_NATIVE_CALL SetDriftMinSpeed( AMX* amx, cell* params )
{
	float val = amx_ctof(params[1]);
	if(val > 1000.0)
	{
		g_MinSpeed = 1000.0;
		return 1;
	}
	if(val < 0.1)
	{
		g_MinSpeed = 0.1f;
		return 1;
	}
	g_MinSpeed = val;
	return 1;
}

static cell AMX_NATIVE_CALL SetDriftMaxSpeed( AMX* amx, cell* params )
{
	float val = amx_ctof(params[1]);
	if(val > 1000.0)
	{
		g_MaxSpeed = 1000.0;
		return 1;
	}
	if(val < 0.1)
	{
		g_MaxSpeed = 0.1f;
		return 1;
	}
	g_MaxSpeed = val;
	return 1;
}

static cell AMX_NATIVE_CALL n_DriftStartEndDelay( AMX* amx, cell* params )
{
	if(params[1] > 1000)
	{
		DriftStartEndDelay = 1000;
	}
	else if(params[1] <= 1)
	{
		DriftStartEndDelay = 1;
	}
	else
	{
		DriftStartEndDelay = params[1];
	}
	return 1;
}

static cell AMX_NATIVE_CALL GetDriftMinSpeed( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[1], &cptr);
	*cptr = amx_ftoc(g_MinSpeed);
	return 1;
}

static cell AMX_NATIVE_CALL GetDriftMaxSpeed( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[1], &cptr);
	*cptr = amx_ftoc(g_MaxSpeed);
	return 1;
}

static cell AMX_NATIVE_CALL GetDriftMaxAngle( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[1], &cptr);
	*cptr = amx_ftoc(g_MaxAngle);
	return 1;
}

static cell AMX_NATIVE_CALL GetDriftMinAngle( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[1], &cptr);
	*cptr = amx_ftoc(g_MinAngle);
	return 1;
}

static cell AMX_NATIVE_CALL n_GetDriftStartEndDelay( AMX* amx, cell* params )
{
	return DriftStartEndDelay;
}

static cell AMX_NATIVE_CALL GetDriftUpdateDelay( AMX* amx, cell* params )
{
	return /*(*/g_TickMax/*/10000)*/;
}

static cell AMX_NATIVE_CALL GetDriftDivider( AMX* amx, cell* params )
{
	return g_divider;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load( void **ppData ) 
{
	g_Invoke = new Invoke;
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	int loop = 0;
	while(loop != MAX_PLAYERS-1)
	{
		GlobalPos_Timer[loop] = (-1);
		loop++;
	}
	cout << "    Drift Counter Plugin 2011 by Gamer_Z loaded";
	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload( )
{
	list<AMX*>::iterator i = g_Invoke->amx_list.begin();
	list<AMX*>::iterator l;

	for(l = g_Invoke->amx_list.end(); i != l; i++)
	{
		if(*i == pAMX)
		{
			g_Invoke->amx_list.erase(i);
			break;
		}
	}
	if(g_Invoke) delete g_Invoke;
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


	{0,                0}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad( AMX *amx ) 
{
	pAMX = amx;
	g_Invoke->amx_list.push_back(amx);
	g_Invoke->getAddresses();
	return amx_Register( amx, driftAMXNatives, -1 );
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload( AMX *amx ) 
{
	return AMX_ERR_NONE;
}