set(SRC
    UnTL/Base/Base.h
    UnTL/Base/CompilerTraits.h
    UnTL/Base/Platform.h
    UnTL/Base/PlatformTraits.h
    UnTL/Base/Byte.h
    UnTL/Base/Flags.h

    UnTL/Buffers/Internal/PoolBucket.h
    UnTL/Buffers/ArrayPool.h

    UnTL/Containers/HeapArray.h
    UnTL/Containers/List.h
    UnTL/Containers/ArraySlice.h

    UnTL/IO/BaseIO.h
    UnTL/IO/BaseIO.cpp
    UnTL/IO/FileHandle.h
    UnTL/IO/FileHandle.cpp
    UnTL/IO/IStream.h
    UnTL/IO/FileStream.h
    UnTL/IO/FileStream.cpp
    UnTL/IO/StdoutStream.h
    UnTL/IO/StdoutStream.cpp
    UnTL/IO/StreamBase.h

    UnTL/Memory/IAllocator.h
    UnTL/Memory/Memory.h
    UnTL/Memory/Object.h
    UnTL/Memory/Ptr.h
    UnTL/Memory/ReferenceCounter.h
    UnTL/Memory/SystemAllocator.h

    UnTL/RTTI/RTTI.h

    UnTL/Strings/Format.h
    UnTL/Strings/Format.cpp
    UnTL/Strings/Internal/jeaiii_to_text.h
    UnTL/Strings/String.h
    UnTL/Strings/StringSlice.h
    UnTL/Strings/StringSlice.cpp
    UnTL/Strings/Unicode.h

    UnTL/Time/DateTime.h
    UnTL/Time/DateTime.cpp
    UnTL/Time/TimeSpan.h

    UnTL/Utils/UUID.h
    UnTL/Utils/Result.h
    UnTL/Utils/BitUtils.h
    UnTL/Utils/UUID.cpp
)

add_library(UnTL STATIC ${SRC})

un_configure_target(UnTL)
un_enable_simd_for_target(UnTL)

target_include_directories(UnTL PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(UnTL PROPERTIES FOLDER "UraniumTL")
target_link_libraries(UnTL dragonbox::dragonbox_to_chars)

get_property("TARGET_SOURCE_FILES" TARGET UnTL PROPERTY SOURCES)
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${TARGET_SOURCE_FILES})

if (UN_BUILD_TESTS)
    add_subdirectory(Tests)
endif ()
