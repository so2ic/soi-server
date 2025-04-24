#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_DIR "build/"
#define SRC_DIR "src/"

void locate_c_files(Nob_Cmd* cmd, const char* path)
{
    Nob_File_Paths children = {0};
    
    if (!nob_read_entire_dir(path, &children)) return;
    
    for (size_t i = 0; i < children.count; ++i) 
    {
        if (*children.items[i] != '.') 
        {
            nob_log(NOB_INFO, "WATCHING %s", children.items[i]);
            Nob_String_Builder full_path = {0};
            if (nob_sv_end_with(nob_sv_from_cstr(children.items[i]), ".c"))
            {
                nob_sb_appendf(&full_path, "%s%s", path, children.items[i]);
                nob_cmd_append(cmd, full_path.items);
            }
            else if (nob_sv_end_with(nob_sv_from_cstr(children.items[i]), ".h") || nob_sv_end_with(nob_sv_from_cstr(children.items[i]), ".json"))
              continue;
            else
            {
                nob_sb_appendf(&full_path, "%s%s/", path, children.items[i]);
                locate_c_files(cmd, full_path.items); 
            }
        }
    }
}

int main(int argc, char **argv) 
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!nob_mkdir_if_not_exists(BUILD_DIR)) return 1;

    Nob_Cmd cmd = {0};

    nob_cmd_append(&cmd, "gcc", "-Wall", "-Wextra", "-g", "-o", BUILD_DIR"soi-server");

    locate_c_files(&cmd, SRC_DIR);

    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    return 0;
}

