new MySQL:Database = MySQL:MYSQL_INVALID_HANDLE;

public OnGameModeInit() {
    OpenConnection();

    #if defined Database_OnGameModeInit
        Database_OnGameModeInit();
    #endif

    return 1;
}

#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif

#define OnGameModeInit Database_OnGameModeInit

#if defined Database_OnGameModeInit
    forward Database_OnGameModeInit();
#endif

public OnGameModeExit() {
    if(Database == MYSQL_INVALID_HANDLE)
        return 1;

    print("Closing the database connection...");
    
    mysql_close(Database);

    #if defined Database_OnGameModeExit
        Database_OnGameModeExit();
    #endif

    return 1;
}

#if defined _ALS_OnGameModeExit
    #undef OnGameModeExit
#else
    #define _ALS_OnGameModeExit
#endif

#define OnGameModeExit Database_OnGameModeExit

#if defined Database_OnGameModeExit
    forward Database_OnGameModeExit();
#endif

static OpenConnection() {
    print("Connecting to the database using "DATABASE_FILE_NAME"...");

    new const count = tickcount();
    
    if((Database = mysql_connect_file(DATABASE_FILE_NAME)) == MySQL:MYSQL_INVALID_HANDLE) {
        printf("Failed to connect to the database after %ims!", (tickcount() - count));

        SendRconCommand("exit");

        return 0;
    }
    
    print("Estabilished a succesful connection with the database!");

    return 1;
}

public OnQueryError(errorid, const error[], const callback[], const query[], MySQL:handle) {
	switch(errorid) {
		case CR_SERVER_GONE_ERROR: {
			print("Database lost connection, attempting to re-open or shut down the server...");

            OpenConnection();
		}
        
        default: {
            printf("A database query exception has occured, error code %i:", errorid);
            print(error);
        }
	}

    #if defined Database_OnQueryError
        Database_OnQueryError(errorid, error, callback, query, handle);
    #endif

	return 1;
}

#if defined _ALS_OnQueryError
    #undef OnQueryError
#else
    #define _ALS_OnQueryError
#endif

#define OnQueryError Database_OnQueryError

#if defined Database_OnQueryError
    forward Database_OnQueryError(errorid, const error[], const callback[], const query[], MySQL:handle);
#endif