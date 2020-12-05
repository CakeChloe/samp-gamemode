enum E_ACCOUNT {
    accountId,
    accountPassword[BCRYPT_HASH_LENGTH + 1],

    bool:accountVerified
};

new Accounts[MAX_PLAYERS + 1][E_ACCOUNT];

public OnPlayerConnect(playerid) {
    new name[MAX_PLAYER_NAME + 1];
    GetPlayerName(playerid, name, sizeof name);
    
    new query[41 + sizeof name + 1];
    mysql_format(Database, query, sizeof query, "SELECT id, password FROM accounts WHERE name = '%e'", name);
    mysql_tquery(Database, query, "OnPlayerAccountCheckQuery", "i", playerid);

	#if defined Accounts_OnPlayerConnect
		Accounts_OnPlayerConnect(playerid);
	#endif

    return 1;
}

#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect Accounts_OnPlayerConnect

#if defined Accounts_OnPlayerConnect
	forward Accounts_OnPlayerConnect(playerid);
#endif

public OnPlayerDisconnect(playerid, reason) {
    #if defined Accounts_OnPlayerDisconnect
        Accounts_OnPlayerDisconnect(playerid, reason);
    #endif

    Accounts[playerid] = Accounts[MAX_PLAYERS];

    return 1;
}

#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect Accounts_OnPlayerDisconnect

#if defined Accounts_OnPlayerDisconnect
    forward Accounts_OnPlayerDisconnect(playerid, reason);
#endif

forward OnPlayerAccountCheckQuery(playerid);
public OnPlayerAccountCheckQuery(playerid) {
    if(!IsPlayerConnected(playerid))
        return 0;

    ShowPlayerLogoTextdraw(playerid);

    // 'clear' the chat for the play, 30 = maximum /pagesize
    for(new message = 0; message < 30; message++)
        SendClientMessage(playerid, 0xFFFFFFFF, " ");

    if(cache_num_rows()) {
        cache_get_value_name_int(0, "id", Accounts[playerid][accountId]);
        cache_get_value_name(0, "password", Accounts[playerid][accountPassword], BCRYPT_HASH_LENGTH);

        // before continuing, make sure there isn't another player logged onto the same account
        // while on a character:

        for(new targetid = 0, maxid = GetPlayerPoolSize() + 1; targetid < maxid; targetid++) {
            if(targetid == playerid)
                continue;

            if(!IsPlayerConnected(targetid))
                continue;

            if(Accounts[targetid][accountId] != Accounts[playerid][accountId])
                continue;

            SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"Another player is already connected using this account!");

            return KickPlayer(playerid);
        }

        // good to go

        return ShowPlayerLoginDialog(playerid);
    }

    ShowPlayerRegistrationDialog(playerid);

    return 1;
}

// use methods to show the dialogs in-case of having to show it again

ShowPlayerLoginDialog(playerid) {
    new name[MAX_PLAYER_NAME + 1], info[191 + MAX_PLAYER_NAME + 1];

    GetPlayerName(playerid, name, sizeof name);

    format(info, sizeof info,
        "An account with your name (%s) exists on the server, please enter\n"\
        "your password if this is your account.\n \n"\
        "If this is not your account, please try another name!",
        
        name);

    return ShowPlayerDialog(playerid, DIALOG_ID_LOGIN, DIALOG_STYLE_PASSWORD, "Los Santos Roleplay - Login", info, "Submit", "Cancel");
}

ShowPlayerRegistrationDialog(playerid) {
    return ShowPlayerDialog(playerid, DIALOG_ID_REGISTER, DIALOG_STYLE_PASSWORD, "Los Santos Roleplay - Register",
        "Welcome to Los Santos Roleplay, this account is not registered on the server,\n"\
        "please enter a password (at least 6 characters) to register this account!",
        "Submit", "Cancel");
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) {
    #if defined Accounts_OnDialogResponse
        Accounts_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);
    #endif

    switch(dialogid) {
        case DIALOG_ID_LOGIN: {
            if(!response) {
                SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"You were kicked as you failed to login!");

                return KickPlayer(playerid);
            }
            
            if(strempty(inputtext)) {
                SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"You must enter a password to login!");

                return ShowPlayerLoginDialog(playerid);
            }

            bcrypt_check(inputtext, Accounts[playerid][accountPassword], "OnPlayerLoginPassword", "i", playerid);
        }

        case DIALOG_ID_REGISTER: {
            if(!response) {
                SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"You were kicked as you failed to register!");

                return KickPlayer(playerid);
            }
            
            if(strempty(inputtext)) {
                SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"You must enter a password to register!");

                return ShowPlayerRegistrationDialog(playerid);
            }

            if(strlen(inputtext) < 6) {
                SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"Your password must be 6 or more characters!");

                return ShowPlayerRegistrationDialog(playerid);
            }

            return bcrypt_hash(inputtext, BCRYPT_PASSWORD_COST, "OnPlayerRegisterPassword", "i", playerid);
        }
    }

    return 1;
}

#if defined _ALS_OnDialogResponse
    #undef OnDialogResponse
#else
    #define _ALS_OnDialogResponse
#endif

#define OnDialogResponse Accounts_OnDialogResponse

#if defined Accounts_OnDialogResponse
    forward Accounts_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]);
#endif

forward OnPlayerLoginPassword(playerid);
public OnPlayerLoginPassword(playerid) {
    if(!IsPlayerConnected(playerid))
        return 0;

    if(!bcrypt_is_equal()) {
        SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"Your credentials does not match up!");

        return ShowPlayerLoginDialog(playerid);
    }

    return OnPlayerLogin(playerid);
}

forward OnPlayerRegisterPassword(playerid);
public OnPlayerRegisterPassword(playerid) {
    if(!IsPlayerConnected(playerid))
        return 0;

    new name[MAX_PLAYER_NAME + 1];
    GetPlayerName(playerid, name, sizeof name);
    
	new hash[BCRYPT_HASH_LENGTH + 1];
	bcrypt_get_hash(hash);

    new query[57 + sizeof name + sizeof hash + 1];
    mysql_format(Database, query, sizeof query, "INSERT INTO accounts (name, password) VALUES ('%e', '%e')", name, hash);
    mysql_tquery(Database, query, "OnPlayerRegister", "i", playerid);

    return 1;
}

forward OnPlayerRegister(playerid);
public OnPlayerRegister(playerid) {
    if(!IsPlayerConnected(playerid))
        return 0;

    Accounts[playerid][accountId] = cache_insert_id();

    if(Accounts[playerid][accountId] == -1) {
        SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"An issue occurred, please try again later!");

        KickPlayer(playerid);

        return 1;
    }

    return OnPlayerLogin(playerid);
}

OnPlayerLogin(playerid) {
    new query[36 + 11 + 1];
    mysql_format(Database, query, sizeof query, "SELECT * FROM accounts WHERE id = %i", Accounts[playerid][accountId]);
    mysql_tquery(Database, query, "OnPlayerLoginQuery", "i", playerid);

    return 1;
}

forward OnPlayerLoginQuery(playerid);

public OnPlayerLoginQuery(playerid) {
    #if defined Accounts_OnPlayerLoginQuery
        Accounts_OnPlayerLoginQuery(playerid);
    #endif

    SendClientMessage(playerid, 0xFFFFFFFF, "Welcome back to the server, you have been signed in!");

    return 1;
}

#if defined _ALS_OnPlayerLoginQuery
    #undef OnPlayerLoginQuery
#else
    #define _ALS_OnPlayerLoginQuery
#endif

#define OnPlayerLoginQuery Accounts_OnPlayerLoginQuery

#if defined Accounts_OnPlayerLoginQuery
    forward Accounts_OnPlayerLoginQuery(playerid);
#endif
