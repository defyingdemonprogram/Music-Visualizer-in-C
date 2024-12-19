#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#define NOB_IMPLEMENTATION
#include "./nob.h"
#include "./src_build/configurer.c"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    const char *program = nob_shift_args(&argc, &argv);
    const char *build_conf_path = "./build/build.conf";
    int build_conf_exists = nob_file_exists(build_conf_path);
    if (build_conf_exists < 0) return 1;
    if (build_conf_exists) {
        // @backcomp
        nob_log(NOB_ERROR, "We found %s. That means your build folder has an old schema.", build_conf_path);
        nob_log(NOB_ERROR, "Instead of %s you are suppose to use %s to configure the build now.", build_conf_path, CONFIG_PATH);
        nob_log(NOB_ERROR, "Remove your ./build/ folder and run %s again to regenerate the folder with the new schema.", program);
        return 1;
    }

    nob_log(NOB_INFO, "--- STAGE 1 ---");

    if (!nob_mkdir_if_not_exists("build")) return 1;

    Nob_String_Builder content = {0};
    int config_exists = nob_file_exists(CONFIG_PATH);
    if (config_exists < 0) return 1;
    if (config_exists == 0) {
        nob_log(NOB_INFO, "Generating %s", CONFIG_PATH);
        generate_default_config(&content);
        if (!nob_write_entire_file(CONFIG_PATH, content.items, content.count)) return 1;
    } else {
        nob_log(NOB_INFO, "File `%s` already exists", CONFIG_PATH);
    }

    nob_log(NOB_INFO, "Generating build/config_logger.c");
    content.count = 0;
    generate_config_logger(&content);
    if (!nob_write_entire_file("build/config_logger.c", content.items, content.count)) return 1;

    Nob_Cmd cmd = {0};
    const char *configured_binary = "build/nob.configured";
    nob_cmd_append(&cmd, NOB_REBUILD_URSELF(configured_binary, "./src_build/nob_configured.c"));
    if (!nob_cmd_run_sync(cmd)) return 1;

    cmd.count = 0;
    nob_cmd_append(&cmd, configured_binary);
    nob_da_append_many(&cmd, argv, argc);
    if (!nob_cmd_run_sync(cmd)) return 1;

    return 0;
}
