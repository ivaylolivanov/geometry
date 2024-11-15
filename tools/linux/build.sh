#!/bin/bash

BINARY_NAME='geometry';
BINARY_NAME_SO="${BINARY_NAME}.so";

DEPENDENCY_NAME_VECTA="vecta.h";
DEPENDENCY_NAME_SP="sp";

DEPENDENCIES_URL_BASE='http://www.math.bas.bg/bantchev/';
DEPENDENCIES_URL_VECTA="${DEPENDENCIES_URL_BASE}/vecta/${DEPENDENCY_NAME_VECTA}";
DEPENDENCIES_URL_SP="${DEPENDENCIES_URL_BASE}/sp/${DEPENDENCY_NAME_SP}";

function is-in-git-repo()
{
    local directory="$1"; shift 1;

    cd "$directory" || return 1;
    git rev-parse --is-inside-work-tree >/dev/null 2>&1 || return 2;

    return 0;
}

function dependencies-install()
{
    local dependencies_dir="$1"; shift 1;

    local curl_executable="$(which -a curl | head -n 1)";
    local vecta="${dependencies_dir}/${DEPENDENCY_NAME_VECTA}";
    local sp="${dependencies_dir}/${DEPENDENCY_NAME_SP}";

    if [ ! -e "$curl_executable" ]; then
        printf -- "\nERROR: curl not found!\n";
        return 1;
    fi

    if [ ! -e "$vecta" ]; then
        "$curl_executable" -o "$vecta" -L -C - "$DEPENDENCIES_URL_VECTA" ||
        {
            printf -- "\nERROR: %s %s\n"       \
                "Missing dependency ${vecta}." \
                'Tried downloading with curl, but failed.';
            return 2;
        }
        wait;
    fi

    if [ ! -e "$sp" ]; then
        "$curl_executable" -o "$sp" -L -C - "$DEPENDENCIES_URL_SP" ||
        {
            printf -- "\nERROR: %s %s\n"    \
                "Missing dependency ${sp}." \
                'Tried downloading with curl, but failed.';
            return 3;
        }
        wait;

        chmod +x "$sp" ||
        {
            printf -- "\nERROR: Failed to make ${sp} executable.\n";
            return 3;
        }
    fi

    return 0;
}

function build()
{
    local working_dir="$1";  shift 1;
    local notebook_dir="$1"; shift 1;

    local clang_executable="$(which -a clang++ | head -n 1)";
    local -a exercises=($(find "$notebook_dir" -type f -print));
    local exercise_name='';
    local exercise_name_binary='';
    local verbose='-v';

    (
        cd "$working_dir" || exit 1;

        for exercise_name in "${exercises[@]}"; do
            exercise_name_binary="$(basename "${exercise_name}" '.cpp')";
            "$clang_executable" "$exercise_name" \
                -o "$exercise_name_binary"       \
                "$verbose" || exit 2;
        done
    ) || return $?;

    return 0;
}

function main()
{
    local current_dir="$(pwd)";

    local git_root='';
    local working_dir='';
    local source_dir='';
    local notebook_dir='';
    local dependencies_dir='';
    local build_target_filepath='';

    is-in-git-repo "$current_dir" ||
    {
        printf -- "\nERROR: %s %s\n"   \
            'Not in a git repository.' \
            'Cannot determine project root!';
        return 1;
    }

    git_root="$(git rev-parse --show-toplevel)";
    working_dir="${git_root}/build";
    source_dir="${git_root}/src";
    dependencies_dir="${git_root}/dep";
    notebook_dir="${source_dir}/notebook";

    mkdir -pv "$dependencies_dir" || return 2;
    dependencies-install "$dependencies_dir" || return 3;

    rm -rvf "$working_dir";
    mkdir -pv "$working_dir";

    build "$working_dir" "$notebook_dir" || return 4;

    return 0;
}

main || exit $?;
