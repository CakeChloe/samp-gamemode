static cameraInterpolationPoints[][][E_COORDINATES] = {
    {
        { 1392.8964, -895.5093, 82.3076 },
        { 1393.0862, -894.5236, 82.3825 }
    },
    {
        { 1382.5330, -906.9901, 81.2998 },
        { 1382.3821, -907.9825, 81.2446 }
    },
    {
        { 1290.7341, -1610.1327, 99.5974 },
        { 1291.6394, -1610.5670, 99.3322 }
    },
    {
        { 1811.1741, -1707.1625, 71.8775 },
        { 1812.1405, -1707.4354, 71.7873 }
    },
    {
        { 2204.4890, -1745.8256, 58.3513 },
        { 2205.4927, -1745.8331, 58.1611 }
    },
    {
        { 2685.3064, -1639.5424, 37.1898 },
        { 2685.2053, -1640.5410, 37.1396 }
    }
};

public OnPlayerConnect(playerid) {
    // add a 1 millisecond timer to execute the spectator settings directly after
    // OnPlayerConnect has fully executed; this is because SA-MP doesn't recognize
    // spectator events prior OnPlayerConnect having been executed! (most of the time)

    TogglePlayerSpectating(playerid, true);

    SetTimerEx("OnPlayerConnectAfter", 1, false, "i", playerid);

	#if defined Camera_OnPlayerConnect
		Camera_OnPlayerConnect(playerid);
	#endif

    return 1;
}

#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect Camera_OnPlayerConnect

#if defined Camera_OnPlayerConnect
	forward Camera_OnPlayerConnect(playerid);
#endif

forward OnPlayerConnectAfter(playerid);
public OnPlayerConnectAfter(playerid) {
    // set player to spectate here rather that udner OnPlayerConnect as
    // OnPlayerConnect is called before the spectating can be done in most cases

    SetPlayerCameraInterpolation(playerid, 0);

    return 1;
}

SetPlayerCameraInterpolation(playerid, point = 0) {
    if(!IsPlayerConnected(playerid))
        return 0;
    
    if(Accounts[playerid][accountVerified])
        return 0;

    if((point + 1) == sizeof cameraInterpolationPoints)
        return 0;

    // calculate the distance between the current point and the next point, then
    // multiply the distance with a set of milliseconds to gather an interval for
    // the timer, based on the distance

    new Float:distance =
        floatround(
            floatsqroot(
                ((cameraInterpolationPoints[point][0][coordinateBreadth] - cameraInterpolationPoints[point + 1][0][coordinateBreadth]) * (cameraInterpolationPoints[point][0][coordinateBreadth] - cameraInterpolationPoints[point + 1][0][coordinateBreadth])) +
                ((cameraInterpolationPoints[point][0][coordinateHeight] - cameraInterpolationPoints[point + 1][0][coordinateHeight]) * (cameraInterpolationPoints[point][0][coordinateHeight] - cameraInterpolationPoints[point + 1][0][coordinateHeight])) +
                ((cameraInterpolationPoints[point][0][coordinateDepth] - cameraInterpolationPoints[point + 1][0][coordinateDepth]) * (cameraInterpolationPoints[point][0][coordinateDepth] - cameraInterpolationPoints[point + 1][0][coordinateDepth]))
            )
        );
    
    new interval = floatround(distance * 25); // 1 meter = .025s

    if(interval < 3000) // minimum 3000ms
        interval = 3000;

    InterpolateCameraPos(playerid,
        cameraInterpolationPoints[point][0][coordinateBreadth], cameraInterpolationPoints[point][0][coordinateHeight], cameraInterpolationPoints[point][0][coordinateDepth],
        cameraInterpolationPoints[point + 1][0][coordinateBreadth], cameraInterpolationPoints[point + 1][0][coordinateHeight], cameraInterpolationPoints[point + 1][0][coordinateDepth],
        interval, CAMERA_MOVE);

    InterpolateCameraLookAt(playerid,
        cameraInterpolationPoints[point][1][coordinateBreadth], cameraInterpolationPoints[point][1][coordinateHeight], cameraInterpolationPoints[point][1][coordinateDepth],
        cameraInterpolationPoints[point + 1][1][coordinateBreadth], cameraInterpolationPoints[point + 1][1][coordinateHeight], cameraInterpolationPoints[point + 1][1][coordinateDepth],
        interval, CAMERA_MOVE);

    // reduce ~25%
    return SetTimerEx("OnPlayerCameraInterpolate", floatround(interval * 0.75), false, "ii", playerid, point + 1);
}

forward OnPlayerCameraInterpolate(playerid, point);
public OnPlayerCameraInterpolate(playerid, point) {
    return SetPlayerCameraInterpolation(playerid, point);
}
