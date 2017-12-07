//
// vcruntime_typeinfo.h
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// <typeinfo> functionality that is implemented in the VCRuntime.
//
#pragma once

#include <vcruntime.h>
#include <vcruntime_exception.h>

#pragma pack(push, _CRT_PACKING)
#ifndef RC_INVOKED

#if defined _M_CEE_MIXED && !defined _VCRT_BUILD
    // Provide a fake definition of __type_info_node to suppress linker warning
    // LNK4248: unresolved typeref token for '__type_info_node'; image may not run.
    struct __type_info_node { };
#else
    struct __type_info_node;
#endif

#ifdef _M_CEE_PURE
    extern System::IntPtr __type_info_root_node;
#else
    extern __type_info_node __type_info_root_node;
#endif



_CRT_BEGIN_C_HEADER

struct __std_type_info_data
{
    const char * _UndecoratedName;
    const char   _DecoratedName[1];
    __std_type_info_data() = delete;
    __std_type_info_data(const __std_type_info_data&) = delete;
    __std_type_info_data(__std_type_info_data&&) = delete;

    __std_type_info_data& operator=(const __std_type_info_data&) = delete;
    __std_type_info_data& operator=(__std_type_info_data&&) = delete;
};

_VCRTIMP int __cdecl __std_type_info_compare(
    _In_ const __std_type_info_data* _Lhs,
    _In_ const __std_type_info_data* _Rhs
    );

_VCRTIMP size_t __cdecl __std_type_info_hash(
    _In_ const __std_type_info_data* _Data
    );

_VCRTIMP const char* __cdecl __std_type_info_name(
    _Inout_ __std_type_info_data* _Data,
    _Inout_ __type_info_node*     _RootNode
    );

_CRT_END_C_HEADER



class type_info
{
public:

    type_info(const type_info&) = delete;
    type_info& operator=(const type_info&) = delete;

    size_t hash_code() const throw()
    {
		// FNV-1a hash function for the undecorated name

#ifdef _WIN64
		static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
		size_t const fnv_offset_basis = 14695981039346656037ULL;
		size_t const fnv_prime = 1099511628211ULL;
#else
		static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
		size_t const fnv_offset_basis = 2166136261U;
		size_t const fnv_prime = 16777619U;
#endif

		size_t value = fnv_offset_basis;
		for (char const* it = raw_name() + 1; *it != '\0'; ++it)
		{
			value ^= static_cast<size_t>(static_cast<unsigned char>(*it));
			value *= fnv_prime;
		}

#ifdef _WIN64
		static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
		value ^= value >> 32;
#else
		static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
#endif

		return value;
    }

	int operator==(const type_info& _Other) const throw();

	int operator!=(const type_info& _Other) const throw();

	int before(const type_info& _Other) const throw();

	const char* name() const throw();

	const char* raw_name() const throw();

    virtual ~type_info() throw();

private:

    mutable __std_type_info_data _Data;
};

namespace std {
	using ::type_info;
}

#if _HAS_EXCEPTIONS

namespace std {

class bad_cast
    : public exception
{
public:

    bad_cast() throw()
        : exception("bad cast", 1)
    {
    }

    static bad_cast __construct_from_string_literal(const char* const _Message) throw()
    {
        return bad_cast(_Message, 1);
    }

private:

    bad_cast(const char* const _Message, int) throw()
        : exception(_Message, 1)
    {
    }
};

class bad_typeid
    : public exception
{
public:

    bad_typeid() throw()
        : exception("bad typeid", 1)
    {
    }

    static bad_typeid __construct_from_string_literal(const char* const _Message) throw()
    {
        return bad_typeid(_Message, 1);
    }

private:

    friend class __non_rtti_object;

    bad_typeid(const char* const _Message, int) throw()
        : exception(_Message, 1)
    {
    }
};

class __non_rtti_object
    : public bad_typeid
{
public:

    static __non_rtti_object __construct_from_string_literal(const char* const _Message) throw()
    {
        return __non_rtti_object(_Message, 1);
    }

private:

    __non_rtti_object(const char* const _Message, int) throw()
        : bad_typeid(_Message, 1)
    {
    }
};

} // namespace std

#endif // _HAS_EXCEPTIONS
#endif // !RC_INVOKED
#pragma pack(pop)
