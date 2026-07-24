#include "Ark/Testing/Test.hpp"

#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Core/Variant.hpp"

ARK_TEST_CASE("Variant basic holds and get_if", "[variant]")
{
    using V = Ark::Variant<int, Ark::Monostate, Ark::UInt64>;

    V v = 42;
    REQUIRE(Ark::getIf<int>(&v) != nullptr);
    REQUIRE(*Ark::getIf<int>(&v) == 42);
    REQUIRE(Ark::getIf<Ark::Monostate>(&v) == nullptr);

    v = Ark::Monostate{};
    REQUIRE(Ark::getIf<Ark::Monostate>(&v) != nullptr);
    REQUIRE(Ark::getIf<int>(&v) == nullptr);

    v = static_cast<Ark::UInt64>(1234);
    REQUIRE(Ark::getIf<Ark::UInt64>(&v) != nullptr);
    REQUIRE(*Ark::getIf<Ark::UInt64>(&v) == 1234);
}

ARK_TEST_CASE("Variant visit dispatch", "[variant]")
{
    using V = Ark::Variant<int, Ark::Monostate, Ark::UInt64>;
    V v = 7;

    auto toString = [](auto const& value) -> const char*
    {
        using T = Ark::Traits::DecayType<decltype(value)>;
        if constexpr (Ark::Traits::isSame<T, int>)
        {
            return "int";
        }
        else if constexpr (Ark::Traits::isSame<T, Ark::Monostate>)
        {
            return "monostate";
        }
        else
        {
            return "u64";
        }
    };

    REQUIRE(std::string(Ark::visit(toString, v)) == std::string("int"));
    v = Ark::Monostate{};
    REQUIRE(std::string(Ark::visit(toString, v)) == std::string("monostate"));
    v = static_cast<Ark::UInt64>(9);
    REQUIRE(std::string(Ark::visit(toString, v)) == std::string("u64"));
}

ARK_TEST_CASE("Variant get emplace and equality", "[variant]")
{
    using V = Ark::Variant<int, Ark::Monostate, Ark::UInt64>;

    V v = 11;
    REQUIRE(v.get<int>() == 11);
    REQUIRE(Ark::get<int>(v) == 11);

    v.emplace<Ark::Monostate>();
    REQUIRE(v.is<Ark::Monostate>());

    v.emplace<Ark::UInt64>(static_cast<Ark::UInt64>(99));
    REQUIRE(v.get<Ark::UInt64>() == 99);

    V same = static_cast<Ark::UInt64>(99);
    V other = 1;
    REQUIRE(v == same);
    REQUIRE(v != other);
}
