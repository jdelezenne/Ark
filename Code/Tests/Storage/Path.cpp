#include "Ark/Testing/Test.hpp"

#include "Ark/Storage/Path.hpp"

using Ark::Storage::Path;

ARK_TEST_CASE("Path", "[path]")
{
    SECTION("basics")
    {
        Path empty;
        REQUIRE(empty.isEmpty());

        Path p("/a/b/../c/./d//");
        Path n = p.normalized();
        REQUIRE(n.isAbsolute());
        REQUIRE(n.string() == "/a/c/d/");

        Path parent = n.parent();
        REQUIRE(parent.string() == "/a/c");

        Path j = parent.join("file.txt");
        REQUIRE(j.string() == "/a/c/file.txt");

        REQUIRE(j.filename() == "file.txt");
        REQUIRE(j.stem() == "file");
        REQUIRE(j.extension() == "txt");
    }

    SECTION("relative join and normalize")
    {
        Path base("a/b");
        Path j = base.join("../c");
        REQUIRE(j.string() == "a/b/../c");

        Path n = j.normalized();
        REQUIRE(n.string() == "a/c");
    }
}
