#pragma once

#include <catch2/catch_test_macros.hpp>

#ifndef ARK_TEST_MODULE
#define ARK_TEST_MODULE Unknown
#endif

#define ARK_PP_STRINGIFY_IMPL(x) #x
#define ARK_PP_STRINGIFY(x) ARK_PP_STRINGIFY_IMPL(x)

#define ARK_TEST_TAGS "["   \
                      "Ark" \
                      "][" ARK_PP_STRINGIFY(ARK_TEST_MODULE) "]"

#define ARK_TEST_CASE(name, tags) TEST_CASE(name, ARK_TEST_TAGS tags)
