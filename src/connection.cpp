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
#include <sqlpp11/sqlite3/connection.h>
#include "detail/prepared_statement_handle.h"
#include "detail/connection_handle.h"

namespace sqlpp
{
	namespace sqlite3
	{
		namespace
		{
			detail::prepared_statement_handle_t prepare_statement(detail::connection_handle& handle, const std::string& statement)
			{
				if (handle.config.debug)
					std::cerr << "Sqlite3 debug: Preparing: '" << statement << "'" << std::endl;

				detail::prepared_statement_handle_t result(nullptr, handle.config.debug);

				auto rc = sqlite3_prepare_v2(
						handle.sqlite,
						statement.c_str(),
						static_cast<int>(statement.size()),
						&result.sqlite_statement,
						nullptr);

				if (rc != SQLITE_OK)
        {
					throw sqlpp::exception("Sqlite3 error: Could not prepare statement: " + std::string(sqlite3_errmsg(handle.sqlite)) + " (statement was >>" + statement + "<<\n");
        }

				return result;
			}

			void execute_statement(detail::connection_handle& handle, detail::prepared_statement_handle_t& prepared)
			{
				auto rc = sqlite3_step(prepared.sqlite_statement);
				switch(rc)
				{
				case SQLITE_OK:
				case SQLITE_ROW: // might occur if execute is called with a select
				case SQLITE_DONE:
					return;
				default:
					std::cerr << "Sqlite3 debug: sqlite3_step return code: " << rc << std::endl;
					throw sqlpp::exception("Sqlite3 error: Could not execute statement: " + std::string(sqlite3_errmsg(handle.sqlite)));
				}
			}
		}

		connection::connection(connection_config config):
			_handle(new detail::connection_handle(config))
		{
		}

		connection::~connection()
		{
		}

		bind_result_t connection::select_impl(const std::string& statement)
		{
			std::unique_ptr<detail::prepared_statement_handle_t> prepared(new detail::prepared_statement_handle_t(prepare_statement(*_handle, statement)));
			if (!prepared)
			{
				throw sqlpp::exception("Sqlite3 error: Could not store result set");
			}

			return {std::move(prepared)};
		}

		bind_result_t connection::run_prepared_select_impl(prepared_statement_t& prepared_statement)
		{
			return { prepared_statement._handle };
		}

		size_t connection::insert_impl(const std::string& statement)
		{
			auto prepared = prepare_statement(*_handle, statement);
			execute_statement(*_handle, prepared);

			return sqlite3_last_insert_rowid(_handle->sqlite);
		}

		prepared_statement_t connection::prepare_impl(const std::string& statement)
		{
			return { std::unique_ptr<detail::prepared_statement_handle_t>(new detail::prepared_statement_handle_t(prepare_statement(*_handle, statement))) };
		}

		size_t connection::run_prepared_insert_impl(prepared_statement_t& prepared_statement)
		{
			execute_statement(*_handle, *prepared_statement._handle.get());

			return sqlite3_last_insert_rowid(_handle->sqlite);
		}

		void connection::run_prepared_execute_impl(prepared_statement_t& prepared_statement)
		{
			execute_statement(*_handle, *prepared_statement._handle.get());
		}

		void connection::execute(const std::string& statement)
		{
			auto prepared = prepare_statement(*_handle, statement);
			execute_statement(*_handle, prepared);
		}

		size_t connection::update_impl(const std::string& statement)
		{
			auto prepared = prepare_statement(*_handle, statement);
			execute_statement(*_handle, prepared);
			return sqlite3_changes(_handle->sqlite);
		}

		size_t connection::run_prepared_update_impl(prepared_statement_t& prepared_statement)
		{
			execute_statement(*_handle, *prepared_statement._handle.get());

			return sqlite3_changes(_handle->sqlite);
		}

		size_t connection::remove_impl(const std::string& statement)
		{
			auto prepared = prepare_statement(*_handle, statement);
			execute_statement(*_handle, prepared);
			return sqlite3_changes(_handle->sqlite);
		}

		size_t connection::run_prepared_remove_impl(prepared_statement_t& prepared_statement)
		{
			execute_statement(*_handle, *prepared_statement._handle.get());

			return sqlite3_changes(_handle->sqlite);
		}

		std::string connection::escape(const std::string& s) const
		{
			std::string t;
			t.reserve(s.size());

			for (const char c : s)
			{
				if (c == '\'')
					t.push_back(c);
				t.push_back(c);
			}

			return t;
		}

		void connection::start_transaction()
		{
			if (_transaction_active)
			{
				throw sqlpp::exception("Sqlite3 error: Cannot have more than one open transaction per connection");
			}
			auto prepared = prepare_statement(*_handle, "BEGIN");
			execute_statement(*_handle, prepared);
			_transaction_active = true;
		}

		void connection::commit_transaction()
		{
			if (not _transaction_active)
			{
				throw sqlpp::exception("Sqlite3 error: Cannot commit a finished or failed transaction");
			}
			_transaction_active = false;
			auto prepared = prepare_statement(*_handle, "COMMIT");
			execute_statement(*_handle, prepared);
		}

		void connection::rollback_transaction(bool report)
		{
			if (not _transaction_active)
			{
				throw sqlpp::exception("Sqlite3 error: Cannot rollback a finished or failed transaction");
			}
			if (report)
			{
				std::cerr << "Sqlite3 warning: Rolling back unfinished transaction" << std::endl;
			}
			_transaction_active = false;
			auto prepared = prepare_statement(*_handle, "ROLLBACK");
			execute_statement(*_handle, prepared);
		}

		void connection::report_rollback_failure(const std::string message) noexcept
		{
			std::cerr << "Sqlite3 message:" << message << std::endl;
		}
	}
}

