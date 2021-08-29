#include <common.h>
#include <lib.h>
#include <algorithm.h>
#include <arch.h>
#include <modules/compile.h>
#include <modules/rt.h>
#include <modules/mm.h>
#include <modules/lang_cs.h>
#include <modules/lang_lq.h>
#include <modules/exec.h>

#include <iostream>
#include <type_traits>
#include <iomanip>
#include <locale>
#include <string>
#include <stdio.h>
#include <codecvt>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <time.h>


using namespace april;
using namespace april::lib;
using namespace april::algorithm;
using namespace april::modules::compile;
using namespace april::modules::lang_cs;
using namespace april::modules::lang_lq;
using namespace april::modules::mm;
using namespace april::modules::exec;
using namespace april::modules::rt;

using namespace std::placeholders;

int main(int argc, char ** argv)
{

    auto h = 123U;

    try
    {
        tvalue_t v = al::parse_numeric(_T("123HU"));
        _PP(v);
    }
    catch (const error_t & e)
    {
        _PP(e);
    }

    return 0;
}

