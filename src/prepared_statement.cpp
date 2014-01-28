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


#include <iostream>
#include <sqlpp11/sqlite3/prepared_statement.h>
#include "detail/prepared_statement_handle.h"


namespace sqlpp
{
	namespace sqlite3
	{
		prepared_statement_t::prepared_statement_t(std::shared_ptr<detail::prepared_statement_handle_t>&& handle):
			_handle(std::move(handle))
		{
			if (_handle and _handle->debug)
				std::cerr << "Sqlite3 debug: Constructing prepared_statement, using handle at " << _handle.get() << std::endl;
		}

		void prepared_statement_t::_bind_boolean_parameter(size_t index, const signed char* value, bool is_null)
		{
			if (_handle->debug)
				std::cerr << "binding boolean parameter " << (*value ? "true":"false") << " at index: " << index << ", being " << (is_null? "" : "not ") << "null" << std::endl;
			if (not is_null)
				sqlite3_bind_int(_handle->sqlite_statement, index + 1, *value);
			else
				sqlite3_bind_null(_handle->sqlite_statement, index + 1);
		}

		void prepared_statement_t::_bind_floating_point_parameter(size_t index, const double* value, bool is_null)
		{
			if (_handle->debug)
				std::cerr << "binding floating_point parameter " << *value << " at index: " << index << ", being " << (is_null? "" : "not ") << "null" << std::endl;
			if (not is_null)
				sqlite3_bind_double(_handle->sqlite_statement, index + 1, *value);
			else
				sqlite3_bind_null(_handle->sqlite_statement, index + 1);
		}

		void prepared_statement_t::_bind_integral_parameter(size_t index, const int64_t* value, bool is_null)
		{
			if (_handle->debug)
				std::cerr << "binding integral parameter " << *value << " at index: " << index << ", being " << (is_null? "" : "not ") << "null" << std::endl;
			if (not is_null)
				sqlite3_bind_int64(_handle->sqlite_statement, index + 1, *value);
			else
				sqlite3_bind_null(_handle->sqlite_statement, index + 1);
		}

		void prepared_statement_t::_bind_text_parameter(size_t index, const std::string* value, bool is_null)
		{
			if (_handle->debug)
				std::cerr << "binding text parameter " << *value << " at index: " << index << ", being " << (is_null? "" : "not ") << "null" << std::endl;
			if (not is_null)
				sqlite3_bind_text(_handle->sqlite_statement, index + 1, value->data(), value->size(), SQLITE_STATIC);
			else
				sqlite3_bind_null(_handle->sqlite_statement, index + 1);
		}
	}
}

