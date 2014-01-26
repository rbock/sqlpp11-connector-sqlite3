/*
 * Copyright (c) 2013, Roland Bock
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SQLPP_SQLITE3_INTERPRETER_H
#define SQLPP_SQLITE3_INTERPRETER_H

#include <sqlpp11/any.h>
#include <sqlpp11/some.h>
#include <sqlpp11/parameter.h>
#include <sqlpp11/join.h>

namespace sqlpp
{
	namespace vendor
	{
		template<typename ValueType, typename NameType>
			struct interpreter_t<sqlite3::serializer_t, parameter_t<ValueType, NameType>>
			{
				using T = parameter_t<ValueType, NameType>;

				static void _(const T& t, sqlite3::serializer_t& context)
				{
					context << "?" << context.count();
					context.pop_count();
					return context;
				}
			};

		template<typename Select>
			struct interpreter_t<sqlite3::serializer_t, any_t<Select>>
			{
				using T = any_t<Select>;

				static void _(const T& t, sqlite3::serializer_t& context)
				{
					static_assert(::sqlpp::detail::wrong<Select>::value, "No support for any()");
				}
			};

		template<typename Select>
			struct interpreter_t<sqlite3::serializer_t, some_t<Select>>
			{
				using T = some_t<Select>;

				static void _(const T& t, sqlite3::serializer_t& context)
				{
					static_assert(::sqlpp::detail::wrong<Select>::value, "No support for some()");
				}
			};


		template<typename Lhs, typename Rhs, typename On>
			struct interpreter_t<sqlite3::serializer_t, join_t<outer_join_t, Lhs, Rhs, On>>
			{
				using T = join_t<outer_join_t, Lhs, Rhs, On>;

				static void _(const T& t, sqlite3::serializer_t& context)
				{
					static_assert(::sqlpp::detail::wrong<outer_join_t, Lhs, Rhs, On>::value, "No support for outer join");
				}
			};

		template<typename Lhs, typename Rhs, typename On>
			struct interpreter_t<sqlite3::serializer_t, join_t<right_outer_join_t, Lhs, Rhs, On>>
			{
				using T = join_t<right_outer_join_t, Lhs, Rhs, On>;

				static void _(const T& t, sqlite3::serializer_t& context)
				{
					static_assert(::sqlpp::detail::wrong<right_outer_join_t, Lhs, Rhs, On>::value, "No support for right outer join");
				}
			};
	}
}

#endif
