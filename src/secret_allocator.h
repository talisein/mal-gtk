/*
 *  This file is part of mal-gtk.
 *
 *  mal-gtk is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  mal-gtk is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with mal-gtk.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#include <exception>
#include <memory>
#include <string>
#include <iostream>
#define GCR_API_SUBJECT_TO_CHANGE
#include "gcr/gcr.h"


namespace MAL
{
    class bad_secret_allocation : public std::runtime_error
    {
    public:
        bad_secret_allocation(const std::string& what_arg) noexcept : std::runtime_error(what_arg) { }
        bad_secret_allocation(const char* what_arg) noexcept : std::runtime_error(what_arg) { }

        virtual ~bad_secret_allocation() noexcept {}; 
    };
    
    template<class T>
    class SecretAllocator
    {
    public:
        typedef T value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef gsize size_type;
        typedef std::ptrdiff_t difference_type;
        typedef std::true_type propagate_on_container_move_assignment;
        typedef std::true_type is_always_equal;

        template<typename _Tp1>
        struct rebind
        { typedef SecretAllocator<_Tp1> other; };

        SecretAllocator() noexcept { }
        SecretAllocator(const SecretAllocator&) noexcept { }
        template<typename _Tp1>
        SecretAllocator(const SecretAllocator<_Tp1>&) noexcept { }
        ~SecretAllocator() noexcept { }

        pointer
        address(reference x) const noexcept
        {
        return std::addressof(x);
        }

        const_pointer
        address(const_reference x) const noexcept
        {
            return std::addressof(x);
        }

        pointer
        allocate(size_type n, gconstpointer = nullptr)
        {
            auto p = gcr_secure_memory_alloc(n * sizeof(T));
            if (nullptr == p) {
                throw bad_secret_allocation("gcr failed to allocate secure memory");
            }
                
            return static_cast<T*>(p);
        }

        void
        deallocate(pointer p, size_type)
        {
            gcr_secure_memory_free(p);
        }

        template<typename _Up, typename... _Args>
        void
        construct(_Up* __p, _Args&&... __args)
        { ::new((void *)__p) _Up(std::forward<_Args>(__args)...); }

        template<typename _Up>
        void
        destroy(_Up* __p) { __p->~_Up(); }

    };

    template<typename _Tp>
    inline bool
    operator==(const SecretAllocator<_Tp>&, const SecretAllocator<_Tp>&)
    { return true; }
        
    template<typename _Tp>
    inline bool
    operator!=(const SecretAllocator<_Tp>&, const SecretAllocator<_Tp>&)
    { return false; }
        
    using secret_string = std::basic_string<char, std::char_traits<char>, SecretAllocator<char>>;
}
