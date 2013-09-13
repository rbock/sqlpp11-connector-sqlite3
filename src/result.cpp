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
#include <sqlpp11/sqlite3/result.h>
#include <sqlpp11/exception.h>
#include "detail/result_handle.h"


namespace sqlpp
{
	namespace sqlite3
	{
		result::result(std::unique_ptr<detail::result_handle>&& handle, const bool debug):
			_handle(std::move(handle)),
			_debug(debug)
		{
			if (_debug)
				std::cerr << "Sqlite3 debug: Constructing result, using handle at " << _handle.get() << std::endl;

			_raw_fields.resize(num_cols());
			_raw_sizes.resize(num_cols());
		}

		result::~result() = default;
		result::result(result&& rhs) = default;
		result& result::operator=(result&&) = default;

		raw_result_row_t result::next()
		{
			if (_debug)
				std::cerr << "Sqlite3 debug: Accessing next row of handle at " << _handle.get() << std::endl;

			auto rc = sqlite3_step(_handle->sqlite_statement);
			switch(rc)
			{
			case SQLITE_ROW:
				{
					for (size_t i = 0; i < num_cols(); ++i)
					{
						_raw_fields[i] = reinterpret_cast<const char*>(sqlite3_column_text(_handle->sqlite_statement, i));
						_raw_sizes[i] = sqlite3_column_bytes(_handle->sqlite_statement, i);
					}
					return raw_result_row_t{ _raw_fields.data(), _raw_sizes.data() };
				}
				break;
			case SQLITE_DONE:
				return raw_result_row_t{ nullptr, nullptr };
			}
			throw sqlpp::exception("Sqlite3 error: Could not get next row: " + std::string(sqlite3_errmsg(sqlite3_db_handle(_handle->sqlite_statement))));
		}

		size_t result::num_cols() const
		{
			return _handle
				? sqlite3_column_count(_handle->sqlite_statement)
				: 0;
		}

	}
}

