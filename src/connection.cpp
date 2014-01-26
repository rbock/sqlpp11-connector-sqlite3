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
			detail::prepared_statement_handle prepare_query(detail::connection_handle& handle, const std::string& query)
			{
				if (handle.config->debug)
					std::cerr << "Sqlite3 debug: Preparing: '" << query << "'" << std::endl;

				detail::prepared_statement_handle result(nullptr, handle.config->debug);

				auto rc = sqlite3_prepare_v2(
						handle.sqlite,
						query.c_str(),
						query.size(),
						&result.sqlite_statement,
						nullptr);

				if (rc != SQLITE_OK)
        {
					throw sqlpp::exception("Sqlite3 error: Could not compile query: " + std::string(sqlite3_errmsg(handle.sqlite)) + " (query was >>" + query + "<<\n");
        }

				return result;
			}

			void execute_query(detail::connection_handle& handle, const std::string& query)
			{
				auto prepared = prepare_query(handle, query);

				if (handle.config->debug)
					std::cerr << "Sqlite3 debug: Executing: '" << query << "'" << std::endl;

				auto rc = sqlite3_step(prepared.sqlite_statement);
				if (rc != SQLITE_OK and rc != SQLITE_DONE)
				{
					std::cerr << "return code: " << rc << std::endl;
					throw sqlpp::exception("Sqlite3 error: Could not finish query: " + std::string(sqlite3_errmsg(handle.sqlite)) + " (query was >>" + query + "<<\n");
				}
			}
		}

		connection::connection(const std::shared_ptr<connection_config>& config):
			_handle(new detail::connection_handle(config))
		{
		}

		connection::~connection()
		{
		}

		char_result_t connection::select_impl(const std::string& query)
		{
			std::unique_ptr<detail::prepared_statement_handle> prepared_statement_handle(new detail::prepared_statement_handle(prepare_query(*_handle, query)));
			if (!prepared_statement_handle)
			{
				throw sqlpp::exception("Sqlite3 error: Could not store result set");
			}


			return {std::move(prepared_statement_handle)};
		}

		size_t connection::insert_impl(const std::string& query)
		{
			execute_query(*_handle, query);

			return sqlite3_last_insert_rowid(_handle->sqlite);
		}

		void connection::execute(const std::string& command)
		{
			execute_query(*_handle, command);
		}

		size_t connection::update_impl(const std::string& query)
		{
			execute_query(*_handle, query);
			return sqlite3_changes(_handle->sqlite);
		}

		size_t connection::remove_impl(const std::string& query)
		{
			execute_query(*_handle, query);
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
				throw sqlpp::exception("Cannot have more than one open transaction per connection");
			}
			execute_query(*_handle, "BEGIN");
			_transaction_active = true;
		}

		void connection::commit_transaction()
		{
			if (not _transaction_active)
			{
				throw sqlpp::exception("Cannot commit a finished or failed transaction");
			}
			_transaction_active = false;
			execute_query(*_handle, "COMMIT");
		}

		void connection::rollback_transaction(bool report)
		{
			if (not _transaction_active)
			{
				throw sqlpp::exception("Cannot rollback a finished or failed transaction");
			}
			if (report)
			{
				std::cerr << "Sqlite3 warning: Rolling back unfinished transaction" << std::endl;
			}
			_transaction_active = false;
			execute_query(*_handle, "ROLLBACK");
		}

		void connection::report_rollback_failure(const std::string message) noexcept
		{
			std::cerr << "Sqlite3 message:" << message << std::endl;
		}
	}
}

