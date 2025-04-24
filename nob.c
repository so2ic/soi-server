#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_DIR "build/"
#define SRC_DIR "src/"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!nob_mkdir_if_not_exists(BUILD_DIR)) return 1;

    Nob_Vector sources = {0};
    nob_find_files_recursive(&sources, SRC_DIR, nob_sv_from_cstr(".c"));

    Nob_Vector objects = {0};

    for (size_t i = 0; i < sources.count; ++i) {
        Nob_String_View src = sources.items[i];

        Nob_String_Builder obj_path = {0};
        nob_sb_append_cstr(&obj_path, BUILD_DIR);
        nob_sv_append_path(&obj_path, src);
        nob_sb_change_extension(&obj_path, ".o");

        nob_mkdir_parent_dirs(obj_path.items);

        Nob_Cmd cmd = {0};
        nob_cmd_append(&cmd, "gcc", "-Wall", "-Wextra", "-g");
        nob_cmd_append(&cmd, "-c", src.data, "-o", obj_path.items);

        nob_log(NOB_INFO, "Compiling %s", src.data);
        if (!nob_cmd_run_sync(cmd)) {
            nob_log(NOB_ERROR, "Failed to compile: %s", src.data);
            return 1;
        }

        nob_da_append(&objects, nob_sv_from_cstr(obj_path.items));
        nob_sb_free(&obj_path);
    }

    Nob_Cmd link_cmd = {0};
    nob_cmd_append(&link_cmd, "gcc", "-o", BUILD_DIR"soi-server");
    for (size_t i = 0; i < objects.count; ++i) {
        nob_cmd_append(&link_cmd, objects.items[i].data);
    }

    nob_log(NOB_INFO, "Linking to bin/exe");
    if (!nob_cmd_run_sync(link_cmd)) {
        nob_log(NOB_ERROR, "Linking failed");
        return 1;
    }

    return 0;
}

