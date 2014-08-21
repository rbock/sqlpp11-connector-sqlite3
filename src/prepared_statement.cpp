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
#include <sstream>
#include <string>
#include <sqlpp11/exception.h>
#include <sqlpp11/sqlite3/prepared_statement.h>
#include "detail/prepared_statement_handle.h"


namespace sqlpp
{
	namespace sqlite3
	{
		namespace
		{
			void check_bind_result(int result, const char* const type)
			{
				switch(result)
				{
				case SQLITE_OK:
					return;
				case SQLITE_RANGE:
					throw sqlpp::exception("Sqlite3 error: " + std::string(type) + " bind value out of range");
				case SQLITE_NOMEM:
					throw sqlpp::exception("Sqlite3 error: " + std::string(type) + " bind out of memory");
				default:
				{
					std::ostringstream stream;

					stream 
						<< "Sqlite3 error: " 
						<< type 
						<< " bind returned unexpected value: " 
						<< result;
					throw sqlpp::exception(stream.str());
				}
				}
			}
		}

		prepared_statement_t::prepared_statement_t(std::shared_ptr<detail::prepared_statement_handle_t>&& handle):
			_handle(std::move(handle))
		{
			if (_handle and _handle->debug)
				std::cerr << "Sqlite3 debug: Constructing prepared_statement, using handle at " << _handle.get() << std::endl;
		}

		void prepared_statement_t::_reset()
		{
			if (_handle->debug)
				std::cerr << "Sqlite3 debug: reseting prepared statement" << std::endl;
			sqlite3_reset(_handle->sqlite_statement);
		}

		void prepared_statement_t::_bind_boolean_parameter(size_t index, const signed char* value, bool is_null)
		{
			if (_handle->debug)
				std::cerr << "Sqlite3 debug: binding boolean parameter " << (*value ? "true":"false") << " at index: " << index << ", being " << (is_null? "" : "not ") << "null" << std::endl;

			int result;
			if (not is_null)
				result = sqlite3_bind_int(_handle->sqlite_statement, static_cast<int>(index + 1), *value);
			else
				result = sqlite3_bind_null(_handle->sqlite_statement, static_cast<int>(index + 1));
			check_bind_result(result, "boolean");
		}

		void prepared_statement_t::_bind_floating_point_parameter(size_t index, const double* value, bool is_null)
		{
			if (_handle->debug)
				std::cerr << "Sqlite3 debug: binding floating_point parameter " << *value << " at index: " << index << ", being " << (is_null? "" : "not ") << "null" << std::endl;

			int result;
			if (not is_null)
				result = sqlite3_bind_double(_handle->sqlite_statement, static_cast<int>(index + 1), *value);
			else
				result = sqlite3_bind_null(_handle->sqlite_statement, static_cast<int>(index + 1));
			check_bind_result(result, "floating_point");
		}

		void prepared_statement_t::_bind_integral_parameter(size_t index, const int64_t* value, bool is_null)
		{
			if (_handle->debug)
				std::cerr << "Sqlite3 debug: binding integral parameter " << *value << " at index: " << index << ", being " << (is_null? "" : "not ") << "null" << std::endl;

			int result;
			if (not is_null)
				result = sqlite3_bind_int64(_handle->sqlite_statement, static_cast<int>(index + 1), *value);
			else
				result = sqlite3_bind_null(_handle->sqlite_statement, static_cast<int>(index + 1));
			check_bind_result(result, "integral");
		}

		void prepared_statement_t::_bind_text_parameter(size_t index, const std::string* value, bool is_null)
		{
			if (_handle->debug)
				std::cerr << "Sqlite3 debug: binding text parameter " << *value << " at index: " << index << ", being " << (is_null? "" : "not ") << "null" << std::endl;

			int result;
			if (not is_null)
				result = sqlite3_bind_text(_handle->sqlite_statement, static_cast<int>(index + 1), value->data(), static_cast<int>(value->size()), SQLITE_STATIC);
			else
				result = sqlite3_bind_null(_handle->sqlite_statement, static_cast<int>(index + 1));
			check_bind_result(result, "text");
		}
	}
}

