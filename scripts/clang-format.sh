#!/usr/bin/env sh

find . -not -path "./build/*" -regex '.*\.\(cpp\|hpp\|cc\|cxx\|h\)' -exec clang-format -verbose -style=file -i {} \;
