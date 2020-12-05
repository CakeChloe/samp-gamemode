#include <a_samp>
#include <a_mysql>
#include <bcrypt>
#include <streamer>
#include <sscanf2>

main() {
    return 1;
}

#define DATABASE_FILE_NAME "database.ini"

#define BCRYPT_PASSWORD_COST (12)

#define MAX_ACCOUNT_CHARACTERS (12)

// used for common usage enumerators (right now just camera interpolation) 
enum E_COORDINATES {
    Float:coordinateBreadth,
    Float:coordinateHeight,
    Float:coordinateDepth
};

#include "../include/core/database.inc"
#include "../include/core/dialogs.inc"
#include "../include/core/colors.inc"
#include "../include/core/functions.inc"

#include "../include/game/players/accounts.inc"
#include "../include/game/players/characters.inc"
#include "../include/game/players/interpolation.inc"

#include "../include/game/players/textdraws/logo.inc"
