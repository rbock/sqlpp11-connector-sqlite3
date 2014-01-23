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


#ifndef SQLPP_SQLITE3_CONNECTION_H
#define SQLPP_SQLITE3_CONNECTION_H

#include <string>
#include <sstream>
#include <sqlpp11/connection.h>
#include <sqlpp11/sqlite3/char_result.h>
//#include <sqlpp11/sqlite3/prepared_query.h>
//#include <sqlpp11/sqlite3/bind_result.h>
#include <sqlpp11/sqlite3/connection_config.h>

namespace sqlpp
{
	namespace sqlite3
	{
		namespace detail
		{
			class connection_handle;
		}

		class connection;

		struct serializer_t
		{
			serializer_t(const connection& db):
				_db(db)
			{}

			template<typename T>
				std::ostream& operator<<(T t)
				{
					return _os << t;
				}

			std::string escape(std::string arg);

			std::string str() const
			{
				return _os.str();
			}

			const connection& _db;
			std::stringstream _os;
		};

		class connection: public sqlpp::connection
		{
			std::unique_ptr<detail::connection_handle> _handle;
			bool _transaction_active = false;

			// direct execution
			char_result_t select_impl(const std::string& query);
			size_t insert_impl(const std::string& query);
			size_t update_impl(const std::string& query);
			size_t remove_impl(const std::string& query);

		public:
			// FIXME: Add prepared query
			using _context_t = serializer_t;
			// join types
			static constexpr bool _supports_outer_join = false;
			static constexpr bool _supports_right_outer_join = false;

			// select
			static constexpr bool _supports_some = false;
			static constexpr bool _supports_any = false;

			connection(const std::shared_ptr<connection_config>& config);
			~connection();
			connection(const connection&) = delete;
			connection(connection&&) = delete;
			connection& operator=(const connection&) = delete;
			connection& operator=(connection&&) = delete;

			//! select returns a result (which can be iterated row by row)
			template<typename Select>
			char_result_t select(const Select& s)
			{
				_context_t context(*this);
				interpret(s, context);
				return select_impl(context.str());
			}

			//! insert returns the last auto_incremented id (or zero, if there is none)
			template<typename Insert>
			size_t insert(const Insert& i)
			{
				_context_t context(*this);
				interpret(i, context);
				return insert_impl(context.str());
			}

			//! update returns the number of affected rows
			template<typename Update>
			size_t update(const Update& u)
			{
				_context_t context(*this);
				interpret(u, context);
				return update_impl(context.str());
			}

			//! remove returns the number of removed rows
			template<typename Remove>
			size_t remove(const Remove& r)
			{
				_context_t context(*this);
				interpret(r, context);
				return remove_impl(context.str());
			}

			//! execute arbitrary command (e.g. create a table)
			void execute(const std::string& command);

			//! escape given string (does not quote, though)
			std::string escape(const std::string& s) const;

			//! call run on the argument
			template<typename T>
				auto run(const T& t) -> decltype(t.run(*this))
				{
					return t.run(*this);
				}

			//! call prepare on the argument
			/*
			template<typename T>
				auto prepare(const T& t) -> decltype(t.prepare(*this))
				{
					return t.prepare(*this);
				}
				*/

			//! start transaction
			void start_transaction();

			//! commit transaction (or throw transaction if the transaction has been finished already)
			void commit_transaction();

			//! rollback transaction with or without reporting the rollback (or throw if the transaction has been finished already)
			void rollback_transaction(bool report);

			//! report a rollback failure (will be called by transactions in case of a rollback failure in the destructor)
			void report_rollback_failure(const std::string message) noexcept;
		};

		inline std::string serializer_t::escape(std::string arg)
		{
			return _db.escape(arg);
		}

	}
}

#include <sqlpp11/sqlite3/interpreter.h>

#endif
