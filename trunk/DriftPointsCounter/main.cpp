/*
* Default Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>

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

#define fENABLED (1)
int flagcheckingstatus[MAX_PLAYERS] = {fENABLED};

#define PLAYER_STATE_DRIVER	(2)

#define PLUGIN_PRIVATE_UPDATE_DRIFT (30/*10000*/)//ticks - weird that not so much ticks is 1 second? :O

#define MINIMAL_ANGLE (12.5)//in degrees
#define MAXIMAL_ANGLE (360.0)//in degrees
#define MINIMAL_SPEED (20.0)//in meters / second
#define MAXIMAL_SPEED (500.0)//in meters / second

#define DEFAULT_DMG (0)
#define START_END_DELAY (4)
#define DIVIDER (350)

float g_MinSpeed[MAX_PLAYERS] = {MINIMAL_SPEED};
float g_MaxAngle[MAX_PLAYERS] = {MAXIMAL_ANGLE};
float g_MinAngle[MAX_PLAYERS] = {MINIMAL_ANGLE};
float g_MaxSpeed[MAX_PLAYERS] = {MAXIMAL_SPEED};
int   g_divider[MAX_PLAYERS] = {DIVIDER};
int   DamageCheck[MAX_PLAYERS] = {DEFAULT_DMG};
int PlayerVehicleID[MAX_PLAYERS];

float VehicleHealth[MAX_PLAYERS];

float SpeedX[MAX_PLAYERS];
float SpeedY[MAX_PLAYERS];
float SpeedZ[MAX_PLAYERS];

float floatdata[MAX_PLAYERS][11];

int Drifting[MAX_PLAYERS];
int Drifting_precise[MAX_PLAYERS];

int DriftStartEndDelay[MAX_PLAYERS] = {START_END_DELAY};
//#define USE_VEHICLE_MODEL_CHECK
//#define CHECK_MAX_VALUES

//#define MAX_POINTS (1000)

struct BonusData
{
	int enabled;
	float xPOS;
	float yPOS;
	float zPOS;
	BonusData(int n0,float n1,float n2,float n3):enabled(n0),xPOS(n1),yPOS(n2),zPOS(n3){}
} 
//BonusPoint[MAX_POINTS]
;

vector <BonusData> BonusPoint;

int g_Ticked = 0;
int g_TickMax = PLUGIN_PRIVATE_UPDATE_DRIFT;

int OnPlayerDriftStart(int playerid);
int OnPlayerDriftStart(int playerid)
{
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

int OnPlayerDriftUpdate(int playerid,int points,int combo,int flagid,float distance);
int OnPlayerDriftUpdate(int playerid,int points,int combo,int flagid,float distance)
{
    //if (playerid == NULL)
    //    return 0;
    int idx; 
    cell ret;
    int amxerr = amx_FindPublic(pAMX, "OnPlayerDriftUpdate", &idx);
    if (amxerr == AMX_ERR_NONE)
    {
		amx_Push(pAMX, amx_ftoc(distance));
		amx_Push(pAMX, flagid);
		amx_Push(pAMX, combo);
		amx_Push(pAMX, points);
		amx_Push(pAMX, playerid);
		amx_Exec(pAMX, &ret, idx);
        return 1;
    }
    return 0;
}

int OnPlayerDriftEnd(int playerid,int points,int combo,int reason);
int OnPlayerDriftEnd(int playerid,int points,int combo,int reason)
{
    //if (playerid == NULL)
    //    return 0;
    int idx; 
    cell ret;
    int amxerr = amx_FindPublic(pAMX, "OnPlayerDriftEnd", &idx);
    if (amxerr == AMX_ERR_NONE)
    {
		amx_Push(pAMX, reason);
		amx_Push(pAMX, combo);
		amx_Push(pAMX, points);
		amx_Push(pAMX, playerid);
		amx_Exec(pAMX, &ret, idx);
        return 1;
    }
    return 0;
}

bool IsDriftingAllowedModel(int modelid);
bool IsDriftingAllowedModel(int modelid)
{
	switch(modelid)
	{
		case 448: return false;
		case 461: return false;
		case 462: return false;
		case 468: return false;
		case 481: return false;
		case 509: return false;
		case 521: return false;
		case 522: return false;
		case 581: return false;
		case 537: return false;
		case 538: return false;
		case 523: return false;
		case 417: return false;
		case 425: return false;
		case 447: return false;
		case 460: return false;
		case 469: return false;
		case 476: return false;
		case 487: return false;
		case 488: return false;
		case 497: return false;
		case 511: return false;
		case 512: return false;
		case 513: return false;
		case 519: return false;
		case 520: return false;
		case 548: return false;
		case 553: return false;
		case 563: return false;
		case 577: return false;
		case 592: return false;
		case 593: return false;
		case 430: return false;
		case 446: return false;
		case 452: return false;
		case 454: return false;
		case 472: return false;
		case 473: return false;
		case 453: return false;
		case 484: return false;
		case 493: return false;
		case 595: return false;
		case 464: return false;
		default: return true;
	}
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
				//if(playerid == 0)g_Invoke->callNative(&PAWN::SendClientMessage,playerid,0xFFFFFFFF,"1");
				PlayerVehicleID[playerid] = g_Invoke->callNative(&PAWN::GetPlayerVehicleID,playerid);
				#if defined USE_VEHICLE_MODEL_CHECK
				bool allowed = IsDriftingAllowedModel(g_Invoke->callNative(&PAWN::GetVehicleModel,PlayerVehicleID[playerid]));
				#endif
				if(DamageCheck[playerid] == 1 && Drifting[playerid] == 1)
				{
					//if(playerid == 0)g_Invoke->callNative(&PAWN::SendClientMessage,playerid,0xFFFF00FF,"2 - Damage Check initialized");
					float tempheal;
					g_Invoke->callNative(&PAWN::GetVehicleHealth,PlayerVehicleID[playerid], &tempheal);
					if(VehicleHealth[playerid] != tempheal)
					{
						//if(playerid == 0)g_Invoke->callNative(&PAWN::SendClientMessage,playerid,0xFFFF00FF,"2 - Damage Check passed");
		 				GlobalPos_Timer[playerid] = (-1);
						OnPlayerDriftEnd(playerid,GlobalPos_Points[playerid],GlobalPos_Combo[playerid],1);
						GlobalPos_Combo[playerid] = 0;
						GlobalPos_Points[playerid] = 0;
						Drifting[playerid] = 0;
						continue;
					}
				}
				g_Invoke->callNative(&PAWN::GetVehicleVelocity,PlayerVehicleID[playerid], &SpeedX[playerid], &SpeedY[playerid], &SpeedZ[playerid]);
				floatdata[playerid][3] = GlobalPos_Angle1[playerid];
				g_Invoke->callNative(&PAWN::GetVehicleZAngle, PlayerVehicleID[playerid], &GlobalPos_Angle1[playerid]);
				//if(playerid == 0)g_Invoke->callNative(&PAWN::SendClientMessage,playerid,0xFF00FFFF,"3");
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
				//if(playerid == 0)g_Invoke->callNative(&PAWN::SendClientMessage,playerid,0xFF0000FF,"4");
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
						&& allowed == true
						#endif
				)
		 		{
					GlobalPos_Points[playerid] += (int)(floor(angle * floatdata[playerid][10])/g_divider[playerid]);
					if(GlobalPos_Timer[playerid] != -1)
					{
						Drifting_precise[playerid] = 1;
						GlobalPos_Combo[playerid] += 1;
						if(flagcheckingstatus[playerid] == 1)
						{
							for(int i = 0,j = BonusPoint.size(); i < j; i++)
							{
								if(BonusPoint[i].enabled == 1)
								{
									float dist = sqrt(pow(BonusPoint[i].xPOS-floatdata[playerid][4],2)+pow(BonusPoint[i].yPOS-floatdata[playerid][5],2)+pow(BonusPoint[i].zPOS-floatdata[playerid][6],2));
									if(dist < 10.0)
									{
										OnPlayerDriftUpdate(playerid,GlobalPos_Points[playerid],GlobalPos_Combo[playerid],i,dist);
									}
								}
							}
						}
						OnPlayerDriftUpdate(playerid,GlobalPos_Points[playerid],GlobalPos_Combo[playerid],-1,-1.0);				
					}
					else
					{
						Drifting[playerid] = 1;
						//if(playerid == 0)g_Invoke->callNative(&PAWN::SendClientMessage,playerid,0x666666FF,"5 - Drift Started");
						g_Invoke->callNative(&PAWN::GetVehicleHealth,PlayerVehicleID[playerid], &VehicleHealth[playerid]);
						OnPlayerDriftStart(playerid);
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
							//if(playerid == 0)g_Invoke->callNative(&PAWN::SendClientMessage,playerid,0x0066FFFF,"5 - Drift Ended");
							OnPlayerDriftEnd(playerid,GlobalPos_Points[playerid],GlobalPos_Combo[playerid],0);
							GlobalPos_Combo[playerid] = 0;
							GlobalPos_Points[playerid] = 0;
							Drifting[playerid] = 0;
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

static cell AMX_NATIVE_CALL GetDriftMinSpeed( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[1], &cptr);
	*cptr = amx_ftoc(g_MinSpeed[params[2]]);
	return 1;
}

static cell AMX_NATIVE_CALL GetDriftMaxSpeed( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[1], &cptr);
	*cptr = amx_ftoc(g_MaxSpeed[params[2]]);
	return 1;
}

static cell AMX_NATIVE_CALL GetDriftMaxAngle( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[1], &cptr);
	*cptr = amx_ftoc(g_MaxAngle[params[2]]);
	return 1;
}

static cell AMX_NATIVE_CALL GetDriftMinAngle( AMX* amx, cell* params )
{
	cell* cptr;
	amx_GetAddr(amx, params[1], &cptr);
	*cptr = amx_ftoc(g_MinAngle[params[2]]);
	return 1;
}

static cell AMX_NATIVE_CALL n_GetDriftStartEndDelay( AMX* amx, cell* params )
{
	return DriftStartEndDelay[params[1]];
}

static cell AMX_NATIVE_CALL GetDriftUpdateDelay( AMX* amx, cell* params )
{
	return /*(*/g_TickMax/*/10000)*/;
}

static cell AMX_NATIVE_CALL GetDriftDivider( AMX* amx, cell* params )
{
	return g_divider[params[1]];
}

static cell AMX_NATIVE_CALL SetDamageCheck( AMX* amx, cell* params )
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
			DamageCheck[playerid] = gg_divider;
			playerid++;
		}
	}
	else
	{
		DamageCheck[params[2]] = gg_divider;
	}
	return 1;
}

static cell AMX_NATIVE_CALL GetDamageCheck( AMX* amx, cell* params )
{
	return DamageCheck[params[1]];
}

static cell AMX_NATIVE_CALL IsPlayerDrifting( AMX* amx, cell* params )
{
	return Drifting[params[1]];
}

static cell AMX_NATIVE_CALL IsPlayerDrifting_precise( AMX* amx, cell* params )
{
	return Drifting_precise[params[1]];
}

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

static cell AMX_NATIVE_CALL DisableFLAG( AMX* amx, cell* params )
{
	BonusPoint[params[1]].enabled = 0;
	return 1;
}

static cell AMX_NATIVE_CALL DeleteFLAG( AMX* amx, cell* params )
{
	BonusPoint[params[1]].enabled = -1;
	return 1;
}

static cell AMX_NATIVE_CALL EnableFLAG( AMX* amx, cell* params )
{
	BonusPoint[params[1]].enabled = 1;
	return 1;
}

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

static cell AMX_NATIVE_CALL GetCheckForFlags( AMX* amx, cell* params )
{
	return flagcheckingstatus[params[1]];
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
		g_MinSpeed[loop] = MINIMAL_SPEED;
		g_MaxAngle[loop] = MAXIMAL_ANGLE;
		g_MinAngle[loop] = MINIMAL_ANGLE;
		g_MaxSpeed[loop] = MAXIMAL_SPEED;
		g_divider[loop] = DIVIDER;
		DamageCheck[loop] = DEFAULT_DMG;
		flagcheckingstatus[loop] = fENABLED;
		DriftStartEndDelay[loop] = START_END_DELAY;
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