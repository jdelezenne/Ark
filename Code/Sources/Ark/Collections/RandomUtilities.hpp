#pragma once

#include "Ark/Collections/Array.hpp"

namespace Ark::Math
{
    class Random;
}

namespace Ark::Collections
{
    /// Selects a random element from a collection.
    /// @tparam T The type of elements in the collection.
    /// @param random The random number generator.
    /// @param collection The collection to select from.
    /// @return A reference to a random element.
    /// Returns a random element.
    template <typename T>
    T const& randomElement(Math::Random& random, Array<T> const& collection)
    {
        ARK_ASSERT(!collection.isEmpty(), "Cannot select from empty collection");

        usize index = random.nextUInt64(0, collection.getCount() - 1);
        return collection[index];
    }

    /// Selects a random element from a collection.
    /// @tparam T The type of elements in the collection.
    /// @param random The random number generator.
    /// @param collection The collection to select from.
    /// @return A reference to a random element.
    /// Returns a random element.
    template <typename T>
    T& randomElement(Math::Random& random, Array<T>& collection)
    {
        ARK_ASSERT(!collection.isEmpty(), "Cannot select from empty collection");

        usize index = random.nextUInt64(0, collection.getCount() - 1);
        return collection[index];
    }

    /// Shuffles a collection in place using the Fisher-Yates algorithm.
    /// @tparam T The type of elements in the collection.
    /// @param random The random number generator.
    /// @param collection The collection to shuffle.
    /// Shuffles elements in place.
    template <typename T>
    void shuffle(Math::Random& random, Array<T>& collection)
    {
        usize count = collection.getCount();

        for (usize i = count - 1; i > 0; --i)
        {
            usize j = random.nextUInt64(0, i);
            swap(collection[i], collection[j]);
        }
    }

    /// Returns a shuffled copy of the collection.
    /// @tparam T The type of elements in the collection.
    /// @param random The random number generator.
    /// @param collection The collection to shuffle.
    /// @return A shuffled copy of the collection.
    /// Returns a shuffled copy.
    template <typename T>
    Array<T> shuffled(Math::Random& random, Array<T> const& collection)
    {
        Array<T> result = collection;
        shuffle(random, result);
        return result;
    }

    /// Samples N elements from a collection without replacement.
    /// @tparam T The type of elements in the collection.
    /// @param random The random number generator.
    /// @param collection The collection to sample from.
    /// @param sampleSize The number of elements to sample.
    /// @return An array containing the sampled elements.
    /// Returns a random sample of elements.
    template <typename T>
    Array<T> sample(Math::Random& random, Array<T> const& collection, usize sampleSize)
    {
        ARK_ASSERT(sampleSize <= collection.getCount(), "Sample size cannot exceed collection size");

        Array<T> result = collection;
        shuffle(random, result);
        result.resize(sampleSize);
        return result;
    }
}
