# Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
# This file is subject to the license terms in the LICENSE file
# found in the top-level directory of this distribution.

add_custom_target(lexy_docs_tutorial
    COMMAND asciidoctor -o tutorial.html ${CMAKE_CURRENT_SOURCE_DIR}/tutorial.adoc)

add_custom_target(lexy_docs_reference
    COMMAND asciidoctor -r ${CMAKE_CURRENT_SOURCE_DIR}/godbolt.rb -o reference.html ${CMAKE_CURRENT_SOURCE_DIR}/reference.adoc)

add_custom_target(lexy_docs)
add_dependencies(lexy_docs lexy_docs_tutorial lexy_docs_reference)

