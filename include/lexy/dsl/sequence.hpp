// Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef LEXY_DSL_SEQUENCE_HPP_INCLUDED
#define LEXY_DSL_SEQUENCE_HPP_INCLUDED

#include <lexy/dsl/base.hpp>

namespace lexyd
{
template <typename... R>
struct _seq : dsl_base
{
    struct matcher
    {
        static constexpr auto max_capture_count = (R::matcher::max_capture_count + ...);

        template <typename Context, typename Input>
        LEXY_DSL_FUNC bool match(Context& context, Input& input)
        {
            auto reset = input;
            if ((R::matcher::match(context, input) && ...))
                return true;

            input = LEXY_MOV(reset);
            return false;
        }
    };

    LEXY_CONSTEVAL _seq() = default;
    LEXY_CONSTEVAL _seq(R...) {}
};

template <typename R1, typename R2>
LEXY_CONSTEVAL auto operator+(R1 r1, R2 r2)
{
    return _seq(r1, r2);
}
template <typename... R, typename Other>
LEXY_CONSTEVAL auto operator+(_seq<R...>, Other other)
{
    return _seq(R{}..., other);
}
template <typename Other, typename... R>
LEXY_CONSTEVAL auto operator+(Other other, _seq<R...>)
{
    return _seq(other, R{}...);
}
template <typename... R, typename... S>
LEXY_CONSTEVAL auto operator+(_seq<R...>, _seq<S...>)
{
    return _seq(R{}..., S{}...);
}
} // namespace lexyd

#endif // LEXY_DSL_SEQUENCE_HPP_INCLUDED