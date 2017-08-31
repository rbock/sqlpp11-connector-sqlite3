/*
 * Copyright (c) 2013 - 2016, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/connection.h>

#include "FpSample.h"
#include <sqlite3.h>
#include <iostream>

namespace sql = sqlpp::sqlite3;

const auto fp = FpSample{};

template <typename L, typename R>
auto require_equal(int line, const L& l, const R& r) -> void
{
  if (l != r)
  {
    std::cerr << line << ": ";
    serialize(::sqlpp::wrap_operand_t<L>{l}, std::cerr);
    std::cerr << " != ";
    serialize(::sqlpp::wrap_operand_t<R>{r}, std::cerr);
    throw std::runtime_error("Unexpected result");
  }
}

static auto require(int line, bool condition) -> void
{
  if (!condition)
  {
    std::cerr << line << " condition violated";
    throw std::runtime_error("Unexpected result");
  }
}

int main()
{
  sql::connection_config config;
  config.path_to_database = ":memory:";
  config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  config.debug = true;

  sql::connection db(config);
  db.execute(R"(CREATE TABLE fp_sample (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      fp REAL
  ))");

  db.execute("INSERT into fp_sample (id, fp) values(NULL, 1.0)");
  db.execute("INSERT into fp_sample (id, fp) values(NULL, 'Inf')");
  db.execute("INSERT into fp_sample (id, fp) values(NULL, 'Nan')");
  db.execute("INSERT into fp_sample (id, fp) values(NULL, 'SomeString')");

  auto q = select(fp.fp).from(fp).unconditionally();
  auto rows = db(q);
  require_equal(__LINE__, rows.front().fp, 1.0);
  rows.pop_front();
  require(__LINE__, !std::isfinite(rows.front().fp.value()));
  rows.pop_front();
  require(__LINE__, std::isnan(rows.front().fp.value()));
  rows.pop_front();
  require_equal(__LINE__, rows.front().fp, 0.0);

  return 0;
}
