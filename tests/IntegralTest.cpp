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

#include <sqlpp11/sqlite3/connection.h>
#include <sqlpp11/sqlpp11.h>

#include "IntegralSample.h"
#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif
#include <iostream>
#include <limits>

namespace sql = sqlpp::sqlite3;

const auto intSample = IntegralSample{};

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

int main()
{
  sql::connection_config config;
  config.path_to_database = ":memory:";
  config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  config.debug = true;

  sql::connection db(config);
  db.execute(R"(CREATE TABLE integral_sample (
      signed_value INTEGER,
      unsigned_value INTEGER
  ))");

  // Supported range for unsigned value (as int64 is the maximum)
  uint64_t unsignedVal = std::numeric_limits<int64_t>::max();
  auto signedVal = std::numeric_limits<int64_t>::max();

  db(insert_into(intSample).set(intSample.signedValue = unsignedVal, intSample.unsignedValue = signedVal));

  // Unsupported range for unsigned value (as int64 is the maximum)
  std::size_t unsignedValUnsupported = std::numeric_limits<uint64_t>::max();
  auto signedValNeg = std::numeric_limits<int64_t>::min();

  auto prepared_insert =
      db.prepare(insert_into(intSample).set(intSample.signedValue = parameter(intSample.signedValue),
                                            intSample.unsignedValue = parameter(intSample.unsignedValue)));
  prepared_insert.params.signedValue = signedValNeg;
  prepared_insert.params.unsignedValue = unsignedValUnsupported;
  db(prepared_insert);

  auto q = select(intSample.signedValue, intSample.unsignedValue).from(intSample).unconditionally();

  auto rows = db(q);

  require_equal(__LINE__, rows.front().signedValue.value(), signedVal);
  require_equal(__LINE__, rows.front().unsignedValue.value(), unsignedVal);
  rows.pop_front();

  require_equal(__LINE__, rows.front().signedValue.value(), signedValNeg);
  require_equal(__LINE__, rows.front().unsignedValue.value(), unsignedValUnsupported);
  rows.pop_front();

  return 0;
}
