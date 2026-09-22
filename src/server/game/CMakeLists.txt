# Project Ambrose by Imjustchico
# Adds the game layer's libraries: globals, shared by every game system, scripting, which holds the hooks content scripts hang off, the game's own session and the world that owns and drains it, and accounts and characters, each built on the database layer, which the login server links on their own.
add_subdirectory(Globals)
add_subdirectory(Scripting)
add_subdirectory(Server)
add_subdirectory(World)
add_subdirectory(Accounts)
add_subdirectory(Characters)
