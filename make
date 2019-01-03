#! /bin/sh

source ./tools/xfunctions

options='dr::'
longoptions='debug,run::';
_expand_args "$options" "$longoptions" "$*" "__g_";

__projects=($__g___args__);

if [ "${#__projects[@]}" -eq 0 ]; then
    __projects=( all );
fi;

function __make_projects()
{
    local projects=$@;

    if _is_defined __g_d || _is_defined __g_debug; then
        make $projects "RUN_ARGS=$__g_run";
    else
        make -s $projects "RUN_ARGS=$__g_run";
    fi;
}

function __make()
{
    if echo "$__projects" | egrep '(\s|^)test\.' &>/dev/null; then
		method=$(echo $__projects | awk -F . '{print $3}')
		if [ "$method" == "" ] || [ "$method" == "run" ]; then
			method=all
		fi;
        __make_projects $method;
    fi;

    __make_projects $__projects;
}

__make;

