#define NOBDEF static inline
#define NOB_IMPLEMENTATION
#define NOB_EXPERIMENTAL_DELETE_OLD
#define NOB_STRIP_PREFIX
#include "nob.h"

#define DIR "build/"
#define SOURCE_DIR "src/"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    Cmd cmd = {0};

    cmd_append(&cmd, "grim", "-h");
    if(!cmd_run(&cmd)) { nob_log(NOB_ERROR, "Install \"grim\""); return 1; }

    if(!mkdir_if_not_exists(DIR)) return 1;

    String_Builder shader = {0}, c_shader = {0};
  
    if(!read_entire_file(SOURCE_DIR"shader.fs", &shader)) return 1;
    sb_append_cstr(&c_shader, "const char *shader = \n\"");
    da_foreach(char, x, &shader) {
        if(*x == '\n') sb_append_cstr(&c_shader, "\\n\"\n\"");
        else da_append(&c_shader, *x);
    }
    c_shader.items[c_shader.count-1] = ';';

    if(!write_entire_file(DIR"shader.h", c_shader.items, c_shader.count)) return 1;

    cmd_append(&cmd, "cc", 
               "-lraylib", "-lm",
               "-o", DIR"groomer",
               SOURCE_DIR"main.c");
    if(!cmd_run(&cmd)) return 1;

    cmd_append(&cmd, "sudo", "cp", "--update", DIR"groomer", "/bin/");
    if(!cmd_run(&cmd)) return 1;

    nob_log(NOB_INFO, "Try running \"groomer\"");

    return 0;
}
