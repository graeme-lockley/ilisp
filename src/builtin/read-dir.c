#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../map.h"
#include "../value.h"

Value *builtin_read_dir(Value *dir_name, Value *env)
{
    if (!IS_STRING(dir_name))
        return exceptions_invalid_argument(mkSymbol("*builtin*.read-dir"), 0, mkSymbol("string"), dir_name);

    errno = 0;
    DIR *dir = opendir(STRING(dir_name));
    if (dir == NULL)
        return exceptions_system_error("*builtin*.read-dir", dir_name);

    struct dirent *de;
    Value *root = VNull;
    Value **root_cursor = &root;
    while (1)
    {
        errno = 0;
        de = readdir(dir);
        if (de == NULL)
            break;

        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;

        Value *h = map_create(4);
        map_set_bang(h, mkKeyword(":sym-link?"), (de->d_type & DT_LNK) ? VTrue : VFalse);
        map_set_bang(h, mkKeyword(":dir?"), (de->d_type & DT_DIR) ? VTrue : VFalse);
        map_set_bang(h, mkKeyword(":file?"), (de->d_type & DT_REG) ? VTrue : VFalse);
        map_set_bang(h, mkKeyword(":name"), mkString(de->d_name));

        Value *v = mkPair(h, VNull);
        *root_cursor = v;
        root_cursor = &CDR(v);
    }
    if (errno != 0)
        root = exceptions_system_error("*builtin*.read-dir", dir_name);
    closedir(dir);

    return root;
}

Value *builtin_read_dir_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.read-dir");
    if (extract_result != NULL)
        return extract_result;

    return builtin_read_dir(parameter[0], env);
}

