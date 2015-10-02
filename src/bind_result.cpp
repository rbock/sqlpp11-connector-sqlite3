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
#include <sqlpp11/exception.h>
#include <sqlpp11/sqlite3/bind_result.h>
#include "detail/prepared_statement_handle.h"

namespace sqlpp
{
  namespace sqlite3
  {
    bind_result_t::bind_result_t(const std::shared_ptr<detail::prepared_statement_handle_t>& handle) : _handle(handle)
    {
      if (_handle and _handle->debug)
        std::cerr << "Sqlite3 debug: Constructing bind result, using handle at " << _handle.get() << std::endl;
    }

    void bind_result_t::_bind_boolean_result(size_t index, signed char* value, bool* is_null)
    {
      if (_handle->debug)
        std::cerr << "Sqlite3 debug: binding boolean result " << *value << " at index: " << index << std::endl;

      *value = static_cast<signed char>(sqlite3_column_int(_handle->sqlite_statement, static_cast<int>(index)));
      *is_null = sqlite3_column_type(_handle->sqlite_statement, static_cast<int>(index)) == SQLITE_NULL;
    }

    void bind_result_t::_bind_floating_point_result(size_t index, double* value, bool* is_null)
    {
      if (_handle->debug)
        std::cerr << "Sqlite3 debug: binding floating_point result " << *value << " at index: " << index << std::endl;

      *value = sqlite3_column_double(_handle->sqlite_statement, static_cast<int>(index));
      *is_null = sqlite3_column_type(_handle->sqlite_statement, static_cast<int>(index)) == SQLITE_NULL;
    }

    void bind_result_t::_bind_integral_result(size_t index, int64_t* value, bool* is_null)
    {
      if (_handle->debug)
        std::cerr << "Sqlite3 debug: binding integral result " << *value << " at index: " << index << std::endl;

      *value = sqlite3_column_int64(_handle->sqlite_statement, static_cast<int>(index));
      *is_null = sqlite3_column_type(_handle->sqlite_statement, static_cast<int>(index)) == SQLITE_NULL;
    }

    void bind_result_t::_bind_text_result(size_t index, const char** value, size_t* len)
    {
      if (_handle->debug)
        std::cerr << "Sqlite3 debug: binding text result at index: " << index << std::endl;

      *value = (reinterpret_cast<const char*>(sqlite3_column_text(_handle->sqlite_statement, static_cast<int>(index))));
      *len = sqlite3_column_bytes(_handle->sqlite_statement, static_cast<int>(index));
    }

    bool bind_result_t::next_impl()
    {
      if (_handle->debug)
        std::cerr << "Sqlite3 debug: Accessing next row of handle at " << _handle.get() << std::endl;

      auto rc = sqlite3_step(_handle->sqlite_statement);

      switch (rc)
      {
        case SQLITE_ROW:
          return true;
        case SQLITE_DONE:
          return false;
        default:
          throw sqlpp::exception("Sqlite3 error: Unexpected return value for sqlite3_step()");
      }
    }
  }
}
