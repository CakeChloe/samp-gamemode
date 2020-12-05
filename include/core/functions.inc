KickPlayer(playerid, bool:instant = false) {
    if(instant)
        return Kick(playerid);

    // get the ping and check if it's over 500 (most importantly to check for the SAMP issue
    // of pings being seen as 65535 but also to make sure the kick isn't delayed forever)

    new ping = GetPlayerPing(playerid);

    HidePlayerLogoTextdraw(playerid);

    return SetTimerEx("OnPlayerKickElapse", (ping > 500)?(500):(ping), false, "i", playerid);
}

forward OnPlayerKickElapse(playerid);
public OnPlayerKickElapse(playerid) {
    if(!IsPlayerConnected(playerid))
        return 0;

    return Kick(playerid);
}

SetPlayerUncontrollable(playerid, interval) {
    TogglePlayerControllable(playerid, false);

    return SetTimerEx("OnPlayerUncontrollableElapse", interval, false, "i", playerid);
}

forward OnPlayerUncontrollableElapse(playerid);
public OnPlayerUncontrollableElapse(playerid) {
    return TogglePlayerControllable(playerid, true);
}

strempty(string[]) {
    return ((!(string[0])) || (((string[0]) == '\1') && (!(string[1]))));
}
