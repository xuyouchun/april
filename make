#! /bin/sh

source ./tools/xfunctions

options='hdg'
longoptions='help,debug,run::,optimize::';
_expand_args "$options" "$longoptions" "$*" "__g_";

__projects=($__g___args__);

if [ "${#__projects[@]}" -eq 0 ]; then
    __projects=( all );
fi;

if _is_defined __g_h || _is_defined __g_help; then
    echo "
make [options] [projects]

options:

    -h, --help:                         Show this message.

    -d, --debug:                        Debug model.

    -g:                                 Generate source-level debug information.

        --run=\"running arguments\":      Running arguments.

        --optimize=<0|1|2|3>:           Optimize level, Default 0.

projects:

    The projects/methods which build, default for all, like:
    
        april:              Builds the project named april.

        april.clean:        Cleans the april project and it's relation projects.

        april.distclean:    Likes clean, also cleans *.dep files.
                            Be used when added/removed sources files, or added/removed
                                header files including to/from a source file.

        april.run:          Builds and Runs the project named april.

"
    exit 0;
fi;

function __make_projects()
{
    local args=$@;

    if _is_defined __g_d || _is_defined __g_debug; then
        make $args "RUN_ARGS=$__g_run";
    else
        make -s $args "RUN_ARGS=$__g_run";
    fi;
}

function __make()
{
    local args=;

    if [ "$__g_optimize" != "" ]; then
        args="$args OPTIMIZE=$__g_optimize";
    fi;

    if _is_defined __g_g; then
        args="$args COMPILE_ARGS=-g";
    fi;

    if echo "$__projects" | egrep '(\s|^)test\.' &>/dev/null; then
		method=$(echo $__projects | awk -F . '{print $3}')
		if [ "$method" == "" ] || [ "$method" == "run" ]; then
			method=all
		fi;
        __make_projects $method $args;
    fi;

    __make_projects $__projects $args;
}

__make;

