//-----------------------------------------------------------------------------
// boost variant/detail/backup_holder.hpp header file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2003
// Eric Friedman
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#ifndef BOOST_VARIANT_DETAIL_BACKUP_HOLDER_HPP
#define BOOST_VARIANT_DETAIL_BACKUP_HOLDER_HPP

#include "boost/assert.hpp"

namespace boost {
namespace detail { namespace variant {

template <typename T>
class backup_holder
{
private: // representation

    T* backup_;

public: // structors

    ~backup_holder()
    {
        delete backup_;
    }

    explicit backup_holder(T* backup)
        : backup_(backup)
    {
    }

    backup_holder(const backup_holder&);

public: // modifiers

    backup_holder& operator=(const backup_holder& rhs)
    {
        *backup_ = rhs.get();
        return *this;
    }

    backup_holder& operator=(const T& rhs)
    {
        *backup_ = rhs;
        return *this;
    }

    void swap(backup_holder& rhs)
    {
        T* tmp = rhs.backup_;
        rhs.backup_ = this->backup_;
        this->backup_ = tmp;
    }

public: // queries

    T& get()
    {
        return *backup_;
    }

    const T& get() const
    {
        return *backup_;
    }

};

template <typename T>
backup_holder<T>::backup_holder(const backup_holder&)
    : backup_(0)
{
    // not intended for copy, but do not want to prohibit syntactically
    BOOST_ASSERT(false);
}

template <typename T>
void swap(backup_holder<T>& lhs, backup_holder<T>& rhs)
{
    lhs.swap(rhs);
}

}} // namespace detail::variant
} // namespace boost

#endif // BOOST_VARIANT_DETAIL_BACKUP_HOLDER_HPP
