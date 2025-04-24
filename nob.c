#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_DIR "build/"
#define SRC_DIR "src/"

int main(int argc, char **argv) 
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!nob_mkdir_if_not_exists(BUILD_DIR)) return 1;

    Nob_Cmd cmd = {0};

    nob_cmd_append(&cmd, "gcc", "-Wall", "-Wextra", "-g", "-o", BUILD_DIR"soi-server");

    Nob_String_Builder sources = {0};
    if(!nob_find_source_in_dir_recursively(&cmd, SRC_DIR))
    {
        nob_log(NOB_ERROR, "could not nob_find_source_in_dir_recursively with %s", SRC_DIR);
        return 1;
    }
    
    nob_cmd_append(&cmd, "-lpthread");

    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    return 0;
}

