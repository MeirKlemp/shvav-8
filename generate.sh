#!/bin/bash

function generate() {
    build_type="$1"
    gen_compile_commands="$2"
    regenerate="$3"

    if [ ! -d "$build_type" ]
    then
        mkdir "$build_type" 
    fi

    if [ "$regenerate" = true ]
    then
        rm "$build_type"/* -rf
    fi

    pushd .
    cd "$build_type"

    # if c++ compile not set, then set it as default to clang++
    if [ -z ${CXX+x} ]
    then
         CXX=clang++
    fi

    cmake_flags="-G Ninja -DCMAKE_CXX_COMPILER="$CXX" -DCMAKE_BUILD_TYPE="$build_type""
    if [ "$gen_compile_commands" = true ]
    then
	cmake .. $cmake_flags -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        cp ./compile_commands.json ../compile_commands.json
    else
        cmake .. $cmake_flags
    fi

    popd
}

regenerate=false
if [ "$1" = "regen" ]
then
    regenerate=true
fi

generate "debug" true "$regenerate"
generate "release" false "$regenerate"
