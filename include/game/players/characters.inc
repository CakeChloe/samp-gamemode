enum E_CHARACTER {
    characterId,

    characterFirstname[MAX_PLAYER_NAME + 1],
    characterLastname[MAX_PLAYER_NAME + 1]
};

new Characters[MAX_PLAYERS + 1][E_CHARACTER];

enum E_ACCOUNT_CHARACTERS {
    accountCharacterId,

    accountCharacterFirstname[MAX_PLAYER_NAME + 1],
    accountCharacterLastname[MAX_PLAYER_NAME + 1]
};

static accountCharacters[MAX_PLAYERS + 1][MAX_ACCOUNT_CHARACTERS][E_ACCOUNT_CHARACTERS];

public OnPlayerDisconnect(playerid, reason) {
    #if defined Characters_OnPlayerDisconnect
        Characters_OnPlayerDisconnect(playerid, reason);
    #endif

    if(Accounts[playerid][accountVerified]) {
        new Float:health, Float:armour;
        GetPlayerHealth(playerid, health);
        GetPlayerArmour(playerid, armour);

        new Float:breadth, Float:height, Float:depth, Float:angle;
        GetPlayerPos(playerid, breadth, height, depth);
        GetPlayerFacingAngle(playerid, angle);

        new query[115 + (7 * 11) + 1];
        
        mysql_format(Database, query, sizeof query, "UPDATE characters SET health = %.2f, armour = %.2f, breadth = %f, height = %f, depth = %f, angle = %f WHERE id = %i",
            health, armour, breadth, height, depth, angle,
            Characters[playerid][characterId]);
        mysql_pquery(Database, query);

        mysql_format(Database, query, sizeof query, "UPDATE characters SET skin = %i, world = %i, interior = %i WHERE id = %i",
            GetPlayerSkin(playerid), GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid),
            Characters[playerid][characterId]);
        mysql_pquery(Database, query);
    }

    Characters[playerid] = Characters[MAX_PLAYERS];
    accountCharacters[playerid] = accountCharacters[MAX_PLAYERS];

    return 1;
}

#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect Characters_OnPlayerDisconnect

#if defined Characters_OnPlayerDisconnect
    forward Characters_OnPlayerDisconnect(playerid, reason);
#endif

public OnPlayerLoginQuery(playerid) {
    #if defined Characters_OnPlayerLoginQuery
        Characters_OnPlayerLoginQuery(playerid);
    #endif

    new query[53 + 11 + 11 + 1];
    mysql_format(Database, query, sizeof query, "SELECT id, firstname, lastname FROM characters WHERE account = %i LIMIT %i", Accounts[playerid][accountId], MAX_ACCOUNT_CHARACTERS);
    mysql_tquery(Database, query, "OnPlayerCharactersQuery", "i", playerid);

    return 1;
}

#if defined _ALS_OnPlayerLoginQuery
    #undef OnPlayerLoginQuery
#else
    #define _ALS_OnPlayerLoginQuery
#endif

#define OnPlayerLoginQuery Characters_OnPlayerLoginQuery

#if defined Characters_OnPlayerLoginQuery
    forward Characters_OnPlayerLoginQuery(playerid);
#endif

forward OnPlayerCharactersQuery(playerid);
public OnPlayerCharactersQuery(playerid) {
    if(!cache_num_rows())
        return ShowPlayerCharCreationDialog(playerid);

    for(new row = 0, maxRow = cache_num_rows(); row < maxRow; row++) {
        cache_get_value_name_int(row, "id", accountCharacters[playerid][row][accountCharacterId]);

        cache_get_value_name(row, "firstname", accountCharacters[playerid][row][accountCharacterFirstname], MAX_PLAYER_NAME);
        cache_get_value_name(row, "lastname", accountCharacters[playerid][row][accountCharacterLastname], MAX_PLAYER_NAME);
    }

    ShowPlayerCharactersDialog(playerid);

    return 1;
}

ShowPlayerCharCreationDialog(playerid) {
    new info[154 + 11 + 1];

    format(info, sizeof info,
        "Enter a character name in the format of Firstname Lastname to register\n"\
        "a new character! Your character's name cannot exceed %i characters!",
        
        MAX_PLAYER_NAME);

    return ShowPlayerDialog(playerid, DIALOG_ID_CHARACTER_CREATE, DIALOG_STYLE_INPUT, "Los Santos Roleplay - Character Creation", info, "Submit", "Cancel");
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) {
    #if defined Characters_OnDialogResponse
        Characters_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);
    #endif

    switch(dialogid) {
        case DIALOG_ID_CHARACTER_CREATE: {
            if(!response) {
                for(new character = 0; character < MAX_ACCOUNT_CHARACTERS; character++) {
                    if(accountCharacters[playerid][character][accountCharacterId] == 0)
                        continue;
                        
                    SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"You cancelled the character creation process!");

                    return ShowPlayerCharactersDialog(playerid);
                }
                
                SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"You cancelled the character creation process and was kicked!");

                return KickPlayer(playerid);
            }

            if(strempty(inputtext)) {
                SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"You must enter a character name to continue!");

                return ShowPlayerCharCreationDialog(playerid);
            }

            if(strlen(inputtext) > MAX_PLAYER_NAME) {
                SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"Your entered character name exceeds the acceptable limit!");

                return ShowPlayerCharCreationDialog(playerid);
            }

            new firstname[MAX_PLAYER_NAME + 1], lastname[MAX_PLAYER_NAME + 1];

            if(sscanf(inputtext, "s[24]s[24]", firstname, lastname)) {
                SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"You character name must be in the format of Firstname Lastname!");
                
                return ShowPlayerCharCreationDialog(playerid);
            }

            new query[74 + sizeof firstname + sizeof lastname + 1];
            mysql_format(Database, query, sizeof query, "SELECT id FROM characters WHERE firstname = '%e' AND lastname = '%e'", firstname, lastname);
            mysql_tquery(Database, query, "OnPlayerRegisterCharacter", "iss", playerid, firstname, lastname);
        }

        case DIALOG_ID_CHARACTERS: {
            if(!response) {
                SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"You cancelled the character selection and was kicked!");

                return KickPlayer(playerid);
            }

            if(inputtext[0] == ' ')
                return ShowPlayerCharactersDialog(playerid);

            if(!strcmp(inputtext, "Create a character", true))
                return ShowPlayerCharCreationDialog(playerid);

            // iteration because a character might be deleted (in the 'future') without affecting the indexing of the array
            new item = 0;

            for(new character = 0; character < MAX_ACCOUNT_CHARACTERS; character++) {
                if(accountCharacters[playerid][character][accountCharacterId] == 0)
                    continue;

                if(item != listitem) {
                    item++;

                    continue;
                }

                return SpawnPlayerCharacter(playerid, accountCharacters[playerid][character][accountCharacterId]);
            }

            // this should never occur but just as a backsafe method
            return ShowPlayerCharactersDialog(playerid);
        }
    }

    return 1;
}

#if defined _ALS_OnDialogResponse
    #undef OnDialogResponse
#else
    #define _ALS_OnDialogResponse
#endif

#define OnDialogResponse Characters_OnDialogResponse

#if defined Characters_OnDialogResponse
    forward Characters_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]);
#endif

forward OnPlayerRegisterCharacter(playerid, firstname[], lastname[]);
public OnPlayerRegisterCharacter(playerid, firstname[], lastname[]) {
    if(!IsPlayerConnected(playerid))
        return 0;

    if(cache_num_rows()) {
        SendClientMessage(playerid, 0xFFFFFFFF, ""COLOR_EMBED_RED"[!] "COLOR_EMBED_WHITE"You character name is already in use, try another one!");
                
        return ShowPlayerCharCreationDialog(playerid);
    }

    for(new character = 0; character < strlen(firstname); character++)
        firstname[character] = (character == 0)?(toupper(firstname[character])):(tolower(firstname[character]));

    for(new character = 0; character < strlen(lastname); character++)
        lastname[character] = (character == 0)?(toupper(lastname[character])):(tolower(lastname[character]));

    new query[77 + 11 + (2 * MAX_PLAYER_NAME) + 1];
    mysql_format(Database, query, sizeof query, "INSERT INTO characters (account, firstname, lastname) VALUES (%i, '%e', '%e')", Accounts[playerid][accountId], firstname, lastname);
    mysql_tquery(Database, query, "OnPlayerRegisterCharQuery", "i", playerid);

    return 1;
}

forward OnPlayerRegisterCharQuery(playerid);
public OnPlayerRegisterCharQuery(playerid) {
    SendClientMessage(playerid, 0xFFFFFFFF, "You have registered your character and you're being spawned at the standard spawn point!")

    return SpawnPlayerCharacter(playerid, cache_insert_id());
}

ShowPlayerCharactersDialog(playerid) {
    new info[30 + (MAX_ACCOUNT_CHARACTERS * (MAX_PLAYER_NAME + 3)) + 23 + 1] = "Select a character to spawn:";

    new characters = 0;

    for(new character = 0; character < MAX_ACCOUNT_CHARACTERS; character++) {
        if(accountCharacters[playerid][character][accountCharacterId] == 0)
            continue;

        characters++;

        format(info, sizeof info, "%s\n%s %s", info, accountCharacters[playerid][character][accountCharacterFirstname], accountCharacters[playerid][character][accountCharacterLastname]);
    }

    if(characters < MAX_ACCOUNT_CHARACTERS)
        strcat(info, "\n \nCreate a character", sizeof info);

    ShowPlayerDialog(playerid, DIALOG_ID_CHARACTERS, DIALOG_STYLE_TABLIST_HEADERS, "Los Santos Roleplay - Character Selection", info, "Select", "Cancel");

    return 1;
}

SpawnPlayerCharacter(playerid, character) {
    new query[38 + 11 + 1];
    mysql_format(Database, query, sizeof query, "SELECT * FROM characters WHERE id = %i", character);
    mysql_tquery(Database, query, "OnPlayerSpawnCharacterQuery", "i", playerid);

    return 1;
}

forward OnPlayerSpawnCharacterQuery(playerid);
public OnPlayerSpawnCharacterQuery(playerid) {
    // put the player under verified mode
    Accounts[playerid][accountVerified] = true;

    cache_get_value_name_int(0, "id", Characters[playerid][characterId]);
    
    cache_get_value_name(0, "firstname", Characters[playerid][characterFirstname], MAX_PLAYER_NAME);
    cache_get_value_name(0, "lastname", Characters[playerid][characterLastname], MAX_PLAYER_NAME);

    // set player's name to a temporary name that exceeds 20 characters, making it impossible for another player
    // to have an identical name (with the same identifier) while being on the login screen as samp client restricts
    // names to 20 characters, while server restricts to 24

    new name[MAX_PLAYER_NAME + 1];

    // add the player's identifier (keeping the last 4 characters unused as player ids range from to 0 to 1000)

    format(name, sizeof name, "TemporaryPlayerName%04d", playerid);
    SetPlayerName(playerid, name);

    // this is done because a player might have the same account name as character name, e.g. john_doe and John Doe;
    // setting the name to John_Doe then would not do anything R.E. uppercases; thus, we free the name to make sure
    // the name is uppercased properly

    format(name, sizeof name, "%s_%s", Characters[playerid][characterFirstname], Characters[playerid][characterLastname]);
    SetPlayerName(playerid, name);

    new skin, Float:breadth, Float:height, Float:depth, Float:angle;
    
    cache_get_value_name_int(0, "skin", skin);
    cache_get_value_name_float(0, "breadth", breadth);
    cache_get_value_name_float(0, "height", height);
    cache_get_value_name_float(0, "depth", depth);
    cache_get_value_name_float(0, "angle", angle);

    SetSpawnInfo(playerid, playerid, skin, breadth, height, depth, angle, 0, 0, 0, 0, 0, 0);

    new message[38 + MAX_PLAYER_NAME + 1];
    format(message, sizeof message, "You have spawned your character %s %s!", Characters[playerid][characterFirstname], Characters[playerid][characterLastname]);
    SendClientMessage(playerid, 0xFFFFFFFF, message);

    TogglePlayerSpectating(playerid, false);

    HidePlayerLogoTextdraw(playerid);

    SpawnPlayer(playerid);

    SetPlayerUncontrollable(playerid, 3000);

    new interior, world;
    cache_get_value_name_int(0, "interior", interior);
    cache_get_value_name_int(0, "world", world);

    SetPlayerInterior(playerid, interior);
    SetPlayerVirtualWorld(playerid, world);

    new Float:health, Float:armour;
    
    cache_get_value_name_float(0, "health", health);
    cache_get_value_name_float(0, "armour", armour);

    SetPlayerHealth(playerid, health);
    SetPlayerArmour(playerid, armour);

    return 1;
}
