#pragma once

#include "Ark/Core/ArkVersion.hpp"
#include "Ark/Core/Byte.hpp"
#include "Ark/Core/Comparison.hpp"
#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Configuration.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Function.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/NumericLimits.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Pair.hpp"
#include "Ark/Core/Platform.hpp"
#include "Ark/Core/Result.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Tuple.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Core/Utilities.hpp"

namespace Ark
{
    /// Default hash functor declaration used across core containers.
    /// @tparam Key Key type to hash.
    template <typename Key>
    struct Hasher;
}
