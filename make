#! /bin/sh

source ./tools/xfunctions

options='hdSwp'
longoptions='help,debug,warning,profile,run::,optimize::';
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

        --run=\"running arguments\":      Running arguments.

        --optimize=<0|1|2|3>:           Optimize level, Default 0.

    -S:                                 Output ASAM(*.s) files.

    -w, --warning:                      Show all warnings.

    -p, --profile:                      Use gperftools to do performance analysis.

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

    X_ARGS="RUN_ARGS=$__g_run"

    # Output asam files.
    if _is_defined __g_S; then
        X_ARGS="$X_ARGS ASAM=yes"
    fi;

    # Show all warnings.
    if _is_defined __g_w || _is_defined __g_warning; then
        X_ARGS="$X_ARGS WARNING=all"
    fi;

    # Do performance analysis.
    if _is_defined __g_p || _is_defined __g_profile; then
        X_ARGS="$X_ARGS PROFILE=yes"
    fi;

    # Make.
    if _is_defined __g_d || _is_defined __g_debug; then
        make $args $X_ARGS;
    else
        make -s $args $X_ARGS;
    fi;
}

function __make()
{
    local args=;

    if [ "$__g_optimize" != "" ]; then
        args="$args OPTIMIZE=$__g_optimize";
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

