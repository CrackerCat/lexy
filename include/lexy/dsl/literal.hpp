// Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef LEXY_DSL_LITERAL_HPP_INCLUDED
#define LEXY_DSL_LITERAL_HPP_INCLUDED

#include <lexy/_detail/nttp_string.hpp>
#include <lexy/_detail/string_view.hpp>
#include <lexy/dsl/base.hpp>
#include <lexy/dsl/whitespace.hpp>

namespace lexyd
{
template <typename String>
struct _lit : atom_base<_lit<String>>
{
    template <typename Reader>
    static LEXY_CONSTEVAL bool _string_compatible()
    {
        using encoding = typename Reader::encoding;
        if (lexy::char_type_compatible_with_reader<Reader, typename String::char_type>)
            return true;

        // The string and the input have incompatible character types.
        // We then only allow ASCII characters in the string literal.
        for (auto c : String::get())
        {
            auto value = encoding::to_int_type(c);
            if (value < encoding::to_int_type(0) || value > encoding::to_int_type(0x7F))
                return false;
        }

        return true;
    }

    template <typename Reader>
    LEXY_DSL_FUNC bool match(Reader& reader)
    {
        static_assert(_string_compatible<Reader>(),
                      "literal contains characters not compatible with input encoding");

        for (auto c : String::get())
        {
            if (reader.peek() != Reader::encoding::to_int_type(c))
                return false;
            reader.bump();
        }

        return true;
    }

    // We need to store the string in the correct char type for the error message.
    template <typename CharT>
    struct _literal_storage_t
    {
        CharT array[String::get().size()];

        LEXY_CONSTEVAL _literal_storage_t()
        {
            auto i = 0u;
            for (auto c : String::get())
                array[i++] = CharT(c);
        }

        LEXY_CONSTEVAL auto view() const
        {
            return lexy::_detail::basic_string_view<CharT>(array, String::get().size());
        }
    };
    template <typename CharT>
    static constexpr _literal_storage_t<CharT> _literal_storage;

    template <typename Reader>
    LEXY_DSL_FUNC auto error(const Reader& reader, typename Reader::iterator pos)
    {
        using error = lexy::error<typename Reader::canonical_reader, lexy::expected_literal>;
        auto idx    = lexy::_detail::range_size(pos, reader.cur());

        using reader_char_type = typename Reader::encoding::char_type;
        if constexpr (std::is_same_v<reader_char_type, typename String::char_type>)
            return error(pos, String::get(), idx);
        else
            return error(pos, _literal_storage<reader_char_type>.view(), idx);
    }

    //=== dsl ===//
    template <typename Whitespace>
    LEXY_CONSTEVAL auto operator[](Whitespace ws) const
    {
        return whitespaced(*this, ws);
    }
};

template <auto C>
constexpr auto lit_c = _lit<lexy::_detail::type_char<C>>{};

#if LEXY_HAS_NTTP
/// Matches the literal string.
template <lexy::_detail::string_literal Str>
constexpr auto lit = _lit<lexy::_detail::type_string<Str>>{};
#endif

#define LEXY_LIT(Str)                                                                              \
    ::lexyd::_lit<LEXY_NTTP_STRING(Str)> {}
} // namespace lexyd

#endif // LEXY_DSL_LITERAL_HPP_INCLUDED

