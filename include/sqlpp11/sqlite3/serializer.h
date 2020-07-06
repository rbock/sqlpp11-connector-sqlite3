/*
 * Copyright (c) 2013 - 2016, Roland Bock
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

#ifndef SQLPP_SQLITE3_SERIALIZER_H
#define SQLPP_SQLITE3_SERIALIZER_H

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif
#include <sqlpp11/any.h>
#include <sqlpp11/data_types/day_point/operand.h>
#include <sqlpp11/data_types/floating_point/operand.h>
#include <sqlpp11/data_types/integral/operand.h>
#include <sqlpp11/data_types/time_point/operand.h>
#include <sqlpp11/data_types/unsigned_integral/operand.h>
#include <sqlpp11/parameter.h>
#include <sqlpp11/pre_join.h>
#include <sqlpp11/some.h>
#include <sqlpp11/with.h>

#include <cmath>

namespace sqlpp
{
#if SQLITE_VERSION_NUMBER < 3008003
  struct assert_no_with_t
  {
    using type = std::false_type;

    template <typename T = void>
    static void _()
    {
      static_assert(wrong_t<T>::value, "Sqlite3: No support for with before version 3.8.3");
    }
  };

  template <typename Database, typename... Expressions>
  struct serializer_t<sqlite3::serializer_t, with_data_t<Database, Expressions...>>
  {
    using _serialize_check = assert_no_with_t;
    using T = with_data_t<Database, Expressions...>;

    static void _(const T&, sqlite3::serializer_t&)
    {
      _serialize_check::_();
    }
  };

#endif
  template <typename ValueType, typename NameType>
  struct serializer_t<sqlite3::serializer_t, parameter_t<ValueType, NameType>>
  {
    using _serialize_check = consistent_t;
    using T = parameter_t<ValueType, NameType>;

    static sqlite3::serializer_t& _(const T& /*t*/, sqlite3::serializer_t& context)
    {
      context << "?" << context.count();
      context.pop_count();
      return context;
    }
  };

  // disable some stuff that won't work with sqlite3
  struct assert_no_any_or_some_t
  {
    using type = std::false_type;

    template <typename T = void>
    static void _()
    {
      static_assert(wrong_t<T>::value, "Sqlite3: No support for any() or some()");
    }
  };

  template <typename Select>
  struct serializer_t<sqlite3::serializer_t, any_t<Select>>
  {
    using _serialize_check = assert_no_any_or_some_t;
    using T = any_t<Select>;

    static void _(const T&, sqlite3::serializer_t&)
    {
      _serialize_check::_();
    }
  };

  template <typename Select>
  struct serializer_t<sqlite3::serializer_t, some_t<Select>>
  {
    using _serialize_check = assert_no_any_or_some_t;
    using T = some_t<Select>;

    static void _(const T&, sqlite3::serializer_t&)
    {
      _serialize_check::_();
    }
  };

  struct assert_no_outer_join_t
  {
    using type = std::false_type;

    template <typename T = void>
    static void _()
    {
      static_assert(wrong_t<T>::value, "Sqlite3: No support for outer join");
    }
  };

  template <typename Lhs, typename Rhs>
  struct serializer_t<sqlite3::serializer_t, pre_join_t<outer_join_t, Lhs, Rhs>>
  {
    using _serialize_check = assert_no_outer_join_t;
    using T = pre_join_t<outer_join_t, Lhs, Rhs>;

    static void _(const T&, sqlite3::serializer_t&)
    {
      _serialize_check::_();
    }
  };

  struct assert_no_right_outer_join_t
  {
    using type = std::false_type;

    template <typename T = void>
    static void _()
    {
      static_assert(wrong_t<T>::value, "Sqlite3: No support for right_outer join");
    }
  };

  template <typename Lhs, typename Rhs>
  struct serializer_t<sqlite3::serializer_t, pre_join_t<right_outer_join_t, Lhs, Rhs>>
  {
    using _serialize_check = assert_no_outer_join_t;
    using T = pre_join_t<right_outer_join_t, Lhs, Rhs>;

    static void _(const T&, sqlite3::serializer_t&)
    {
      _serialize_check::_();
    }
  };

  template <typename Period>
  struct serializer_t<sqlite3::serializer_t, time_point_operand<Period>>
  {
    using _serialize_check = consistent_t;
    using Operand = time_point_operand<Period>;

    static sqlite3::serializer_t& _(const Operand& t, sqlite3::serializer_t& context)
    {
      const auto dp = ::sqlpp::chrono::floor<::date::days>(t._t);
      const auto time = ::date::make_time(t._t - dp);
      const auto ymd = ::date::year_month_day{dp};
      context << "STRFTIME('%Y-%m-%d %H:%M:%f', '" << ymd << ' ' << time << "')";
      return context;
    }
  };

  template <>
  struct serializer_t<sqlite3::serializer_t, day_point_operand>
  {
    using _serialize_check = consistent_t;
    using Operand = day_point_operand;

    static sqlite3::serializer_t& _(const Operand& t, sqlite3::serializer_t& context)
    {
      const auto ymd = ::date::year_month_day{t._t};
      context << "DATE('" << ymd << "')";
      return context;
    }
  };

  template <>
  struct serializer_t<sqlite3::serializer_t, floating_point_operand>
  {
    using _serialize_check = consistent_t;
    using Operand = floating_point_operand;

    static sqlite3::serializer_t& _(const Operand& t, sqlite3::serializer_t& context)
    {
      if (std::isnan(t._t))
        context << "'NaN'";
      else if (std::isinf(t._t))
      {
        if (t._t > std::numeric_limits<double>::max())
          context << "'Inf'";
        else
          context << "'-Inf'";
      }
      else
        context << t._t;
      return context;
    }
  };

  // sqlite3 accepts only signed integers,
  // so we MUST perform a conversion from unsigned to signed
  template <>
  struct serializer_t<sqlite3::serializer_t, unsigned_integral_operand>
  {
    using _serialize_check = consistent_t;
    using Operand = unsigned_integral_operand;

    static sqlite3::serializer_t& _(const Operand& t, sqlite3::serializer_t& context)
    {
      context << static_cast<typename integral_operand::_value_t>(t._t);
      return context;
    }
  };
}

#endif
