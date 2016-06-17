#pragma once

#include <cassert>
#include <iostream>
#include <type_traits>

namespace ext_sort {
    template <typename T>
    void SavePOD(std::ostream& out, const T& val) {
        static_assert(std::is_pod<T>::value, "SavePOD() requires a POD type as template argument");
        out.write(reinterpret_cast<const char*>(&val), sizeof(T));
    }

    template <typename T>
    bool LoadPOD(std::istream& in, T& val) {
        static_assert(std::is_pod<T>::value, "LoadPOD() requires a POD type as template argument");
        return in.read(reinterpret_cast<char*>(&val), sizeof(T)).gcount() == sizeof(T);
    }

    template <typename T>
    struct PODTraits {
        static_assert(std::is_pod<T>::value, "PODTraits require a POD type as template argument");

        typedef void Context;

        static size_t Size(const T&) {
            return sizeof(T);
        }

        static void Save(std::ostream& out, const T& val, void*) {
            return SavePOD(out, val);
        }

        static bool Load(std::istream& in, T& val, void*) {
            return LoadPOD(in, val);
        }

        // Must be implemented in inherited traits
        static int Compare(const T& a, const T& b, const void*);
    };

    template <typename T, bool Desc = false>
    struct ArithmeticTraits: public PODTraits<T> {
        static_assert(std::is_arithmetic<T>::value, "ArithmeticTraits require an arithmetic type as template argument");

        static int Compare(const T& a, const T& b, const void*) {
            if (a < b) {
                return Desc ? 1 : -1;
            }
            if (a > b) {
                return Desc ? -1 : 1;
            }
            return 0;
        }
    };

    template <typename T>
    struct ObjectTraits {
        typedef void Context;

        static size_t Size(const T& val) {
            return val.Size();
        }

        static void Save(std::ostream& out, const T& val, void*) {
            return val.Save(out);
        }

        static bool Load(std::istream& in, T& val, void*) {
            return val.Load(in);
        }

        static int Compare(const T& a, const T& b, const void*) {
            return T::Compare(a, b);
        }
    };

    // Inherit these traits to setup default pointer semantics
    template <typename Ptr>
    struct PointerTraits {
        typedef void Context;

        static size_t Size(const Ptr& ptr) {
            assert(ptr);
            return ptr->Size();
        }

        static void Save(std::ostream& out, const Ptr& ptr, void*) {
            assert(ptr);
            ptr->Save(out);
        }

        // Must be reimplemented in inheriting traits with proper ptr initialization
        static bool Load(std::istream&, Ptr& ptr, void*) {
            assert(!ptr);
            return false;
        }

        static int Compare(const Ptr& a, const Ptr& b, const void*) {
            assert(a);
            assert(b);
            return a->Compare(*b);
        }
    };
} // namespace ext_sort
