#if defined DRIFTER_PLUGIN_INCLUDED

#else

#define DRIFTER_PLUGIN_INCLUDED
#pragma library "DriftPointsCounter"
//-1 means set for all players.

#define DRIFT_REASON_TIME_EXPIRE			(0)
#define DRIFT_REASON_CAR_DAMAGED			(1)
#define DRIFT_REASON_PLAYER_EXIT_VEHICLE	(2)

forward OnPlayerDriftStart(playerid);
forward OnPlayerDriftUpdate(playerid,value,combo,flagid,Float:distance,Float:speed);
forward OnPlayerDriftEnd(playerid,value,combo,reason);

native DriftSet_UpdateDelay(delay);
native DriftGet_UpdateDelay();

native DriftSet_MinimalSpeed(Float:MinimalSpeed,playerid = (-1));
native DriftGet_MinimalSpeed(&Float:MinimalSpeed,playerid = (0));

native DriftSet_MinimalAngle(Float:MinimalAngle,playerid = (-1));
native DriftGet_MinimalAngle(&Float:MinimalAngle,playerid = (0));

native DriftSet_MaximalSpeed(Float:MaximalSpeed,playerid = (-1));
native DriftGet_MaximalSpeed(&Float:MaximalSpeed,playerid = (0));

native DriftSet_MaximalAngle(Float:MaximalAngle,playerid = (-1));
native DriftGet_MaximalAngle(&Float:MaximalAngle,playerid = (0));

native DriftSet_StartEndDelay(delay,playerid = (-1));
native DriftGet_StartEndDelay(playerid = (0));

native DriftSet_Divider(divider,playerid = (-1));
native DriftGet_Divider(playerid = (0));

native DriftSet_DamageCheck(enable = 1,playerid = (-1),Float:MaxHealthLoose = 0.0);//default = disabled (0), enabled = 1
native DriftGet_DamageCheck(playerid = (0),&Float:MaxHealthLoose = 0.0);

native IsPlayerDrifting(playerid);
native IsPlayerDrifting_precise(playerid);

native Drift_AddFLAG(Float:X,Float:Y,Float:Z);//returns flagid
native Drift_DisableFLAG(flagid);
native Drift_DeleteFLAG(flagid);
native Drift_EnableFLAG(flagid);
native Drift_MoveFLAG(flagid,Float:X,Float:Y,Float:Z);

native Drift_SetCheckForFlags(enabled = 1,playerid =(-1));
native Drift_GetCheckForFlags(playerid = 0);

native Drift_SetGlobalCheck(enable = 1);
native Drift_GetGlobalCheck();

native Drift_SetPlayerCheck(playerid = (-1),enable = 1);
native Drift_GetPlayerCheck(playerid = (0));

native Drift_SetBackwardsCheck(enabled = 1,playerid =(-1));
native Drift_GetBackwardsCheck(playerid = 0);

native Drift_SetDriftMaxHealthLoose(Float:MaximalHealthLoose = 0.0,playerid = (-1));
native Drift_GetDriftMaxHealthLoose(&Float:MaximalHealthLoose,playerid = (0));

native Private_DriftGetVersion();

native Drift_AllowModel(modelid);
native Drift_DisallowModel(modelid);
native Drift_ClearModelList();
native Drift_ResetDefaultModelList();

#define ALLOW_SERVER_LISTING

#if defined ALLOW_SERVER_LISTING

#include <a_http>
forward PrivateDriftRegister();
public PrivateDriftRegister()
{
	new url[256],port;
	port = GetServerVarAsInt("port");
	format(url,256,"wow.demonworld.nl/driftingmasterlist/register.php?port=%d&rev=%d",port,Private_DriftGetVersion());
	return HTTP(0,HTTP_GET,url,"No_Data","None");
}

#endif

#endif
