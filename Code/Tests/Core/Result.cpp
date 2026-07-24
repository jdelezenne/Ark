#include "Ark/Testing/Test.hpp"

#include "Ark/Core/Result.hpp"

ARK_TEST_CASE("Result", "[core][result]")
{
    SECTION("basic ok and error construction")
    {
        Ark::Result<int, int> ok{123};
        REQUIRE(ok.isOk());
        REQUIRE(static_cast<bool>(ok));
        REQUIRE(*ok == 123);
        REQUIRE(ok.getValue() == 123);
        REQUIRE(ok.getValueOr(7) == 123);
        REQUIRE(ok.errorOr(9) == 9);

        Ark::Error<int> errVal{5};
        Ark::Result<int, int> err{errVal};
        REQUIRE(err.isError());
        REQUIRE(!static_cast<bool>(err));
        REQUIRE(err.getError() == 5);
        REQUIRE(err.getValueOr(33) == 33);
        REQUIRE(err.errorOr(0) == 5);
    }

    SECTION("comparisons and swap")
    {
        Ark::Result<int, int> a{10};
        Ark::Result<int, int> b{Ark::Error<int>{2}};
        REQUIRE(a != b);
        REQUIRE(a == 10);
        REQUIRE(b == Ark::Error<int>{2});

        Ark::swap(a, b);
        REQUIRE(b == 10);
        REQUIRE(a == Ark::Error<int>{2});
    }

    SECTION("map and mapError")
    {
        Ark::Result<int, int> ok{3};
        auto mappedOk = ok.map([](int v)
                               {
                                   return v * 2;
                               });
        REQUIRE(mappedOk == 6);

        Ark::Result<int, int> err{Ark::Error<int>{7}};
        auto mappedErr = err.map([](int v)
                                 {
                                     return v * 2;
                                 });
        REQUIRE(mappedErr == Ark::Error<int>{7});

        auto mappedErr2 = err.mapError([](int e)
                                       {
                                           return e + 1;
                                       });
        REQUIRE(mappedErr2 == Ark::Error<int>{8});

        auto mappedOk2 = ok.mapError([](int e)
                                     {
                                         return e + 1;
                                     });
        REQUIRE(mappedOk2 == 3);
    }

    SECTION("dereference and arrow operators")
    {
        struct TestStruct
        {
            int value;
            int getValue() const
            {
                return value;
            }
        };

        Ark::Result<TestStruct, int> ok{TestStruct{42}};
        REQUIRE((*ok).value == 42);
        REQUIRE(ok->value == 42);
        REQUIRE(ok->getValue() == 42);

        TestStruct& ref = *ok;
        ref.value = 100;
        REQUIRE(ok->value == 100);
    }

    SECTION("copy and move semantics")
    {
        Ark::Result<int, int> ok1{123};
        Ark::Result<int, int> ok2{Ark::move(ok1)};
        REQUIRE(ok2.isOk());
        REQUIRE(*ok2 == 123);

        Ark::Result<int, int> err1{Ark::Error<int>{999}};
        Ark::Result<int, int> err2{Ark::move(err1)};
        REQUIRE(err2.isError());
        REQUIRE(err2.getError() == 999);

        Ark::Result<int, int> ok3{42};
        auto moved = Ark::move(ok3).map([](int v)
                                        {
                                            return v * 2;
                                        });
        REQUIRE(moved == 84);

        Ark::Result<int, int> copySource{10};
        Ark::Result<int, int> copyTarget{20};
        copyTarget = copySource;
        REQUIRE(copyTarget == 10);

        Ark::Result<int, int> errSource{Ark::Error<int>{5}};
        Ark::Result<int, int> errTarget{Ark::Error<int>{6}};
        errTarget = errSource;
        REQUIRE(errTarget.getError() == 5);
    }

    SECTION("value and error in-place construction")
    {
        struct Complex
        {
            int a, b;
            Complex(int x, int y)
                : a(x)
                , b(y)
            {
            }
        };

        Ark::Result<Complex, int> result{Complex{10, 20}};
        REQUIRE(result.isOk());
        REQUIRE(result->a == 10);
        REQUIRE(result->b == 20);

        Ark::Result<int, Complex> errResult{Ark::unexpectedResult, Complex{30, 40}};
        REQUIRE(errResult.isError());
        REQUIRE(errResult.getError().a == 30);
        REQUIRE(errResult.getError().b == 40);
    }

    SECTION("outcome specialization")
    {
        Ark::Outcome success = Ark::makeOutcome();
        REQUIRE(success.isOk());
        REQUIRE(!success.isError());
        REQUIRE(static_cast<bool>(success));

        Ark::Outcome failure = Ark::makeError();
        REQUIRE(!failure.isOk());
        REQUIRE(failure.isError());
        REQUIRE(!static_cast<bool>(failure));

        Ark::Outcome copied = success;
        REQUIRE(copied.isOk());

        Ark::Outcome moved = Ark::move(failure);
        REQUIRE(moved.isError());
    }

    SECTION("void value specialization")
    {
        Ark::Result<Ark::Void, int> ok;
        REQUIRE(ok.isOk());
        REQUIRE(!ok.isError());

        Ark::Result<Ark::Void, int> err{Ark::Error<int>{404}};
        REQUIRE(err.isError());
        REQUIRE(err.getError() == 404);

        Ark::Result<Ark::Void, int> errCopy{err};
        REQUIRE(errCopy.isError());
        REQUIRE(errCopy.getError() == 404);

        Ark::Result<Ark::Void, int> errAssigned;
        errAssigned = err;
        REQUIRE(errAssigned.isError());
        REQUIRE(errAssigned.getError() == 404);

        errAssigned = Ark::Result<Ark::Void, int>{};
        REQUIRE(errAssigned.isOk());

        struct NonTrivialError
        {
            int code = 0;
            NonTrivialError() = default;
            explicit NonTrivialError(int value)
                : code(value)
            {
            }
            NonTrivialError(NonTrivialError const&) = default;
            NonTrivialError& operator=(NonTrivialError const&) = default;
            ~NonTrivialError()
            {
                code = -1;
            }
        };

        Ark::Result<Ark::Void, NonTrivialError> nonTrivial{Ark::Error<NonTrivialError>{NonTrivialError{7}}};
        Ark::Result<Ark::Void, NonTrivialError> nonTrivialCopy{nonTrivial};
        REQUIRE(nonTrivialCopy.isError());
        REQUIRE(nonTrivialCopy.getError().code == 7);
    }

    SECTION("map and mapError type transformations")
    {
        Ark::Result<int, int> intResult{5};
        auto floatResult = intResult.map([](int v)
                                         {
                                             return static_cast<float>(v) * 2.5f;
                                         });
        REQUIRE(floatResult.isOk());
        REQUIRE(*floatResult == 12.5f);

        Ark::Result<int, int> errResult{Ark::Error<int>{42}};
        auto mappedErr = errResult.mapError([](int e)
                                            {
                                                return static_cast<float>(e) * 1.5f;
                                            });
        REQUIRE(mappedErr.isError());
        REQUIRE(mappedErr.getError() == 63.0f);
    }

    SECTION("andThen transform orElse chain")
    {
        auto ensurePositive = [](int value) -> Ark::Result<int, int> {
            if (value > 0)
            {
                return value;
            }
            return Ark::Error<int>{-1};
        };

        Ark::Result<int, int> start{21};
        auto okChain = start
                           .andThen(ensurePositive)
                           .transform([](int value) { return value * 2; })
                           .orElse([](int) { return Ark::Result<int, int>{0}; });
        REQUIRE(okChain.isOk());
        REQUIRE(okChain.getValue() == 42);

        Ark::Result<int, int> negative{-3};
        auto recovered = negative
                             .andThen(ensurePositive)
                             .orElse([](int error) { return Ark::Result<int, int>{error + 10}; });
        REQUIRE(recovered.isOk());
        REQUIRE(recovered.getValue() == 9);

        Ark::Result<int, int> err{Ark::asError, 5};
        auto transformedError = err.transformError([](int error) { return error * 3; });
        REQUIRE(transformedError.isError());
        REQUIRE(transformedError.getError() == 15);
    }
}
