static PlayerText:playerLogoTextdraws[MAX_PLAYERS + 1] = { PlayerText:-1, ... };

ShowPlayerLogoTextdraw(playerid) {
    playerLogoTextdraws[playerid] = CreatePlayerTextDraw(playerid, 320.0 - 65.0, 100.0, "mdl-2001:logo256x128");
	PlayerTextDrawFont(playerid, playerLogoTextdraws[playerid], 4);
	PlayerTextDrawUseBox(playerid, playerLogoTextdraws[playerid], 1);
	PlayerTextDrawTextSize(playerid, playerLogoTextdraws[playerid], 130.0, 70.0);
    PlayerTextDrawShow(playerid, playerLogoTextdraws[playerid]);

    return 1;
}

HidePlayerLogoTextdraw(playerid) {
    if(playerLogoTextdraws[playerid] == playerLogoTextdraws[MAX_PLAYERS])
        return 0;

    PlayerTextDrawDestroy(playerid, playerLogoTextdraws[playerid]);

    playerLogoTextdraws[playerid] = playerLogoTextdraws[MAX_PLAYERS];

    return 1;
}

public OnPlayerDisconnect(playerid, reason) {
    #if defined TDLogo_OnPlayerDisconnect
        TDLogo_OnPlayerDisconnect(playerid, reason);
    #endif
    
    playerLogoTextdraws[playerid] = playerLogoTextdraws[MAX_PLAYERS];

    return 1;
}

#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect TDLogo_OnPlayerDisconnect

#if defined TDLogo_OnPlayerDisconnect
    forward TDLogo_OnPlayerDisconnect(playerid, reason);
#endif
