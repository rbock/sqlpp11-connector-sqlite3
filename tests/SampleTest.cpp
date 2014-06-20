/*
 * Copyright (c) 2013, Roland Bock
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

#include "TabSample.h"
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>

#include <sqlite3.h>
#include <iostream>
#include <vector>


SQLPP_ALIAS_PROVIDER(left);

namespace sql = sqlpp::sqlite3;
int main()
{
	auto config = std::make_shared<sql::connection_config>();
 	config->path_to_database = ":memory:";
	config->flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	config->debug = true;

	sql::connection db(config);
	db.execute(R"(CREATE TABLE tab_sample (
		alpha bigint(20) DEFAULT NULL,
			beta bool DEFAULT NULL,
			gamma varchar(255) DEFAULT NULL
			))");
	db.execute(R"(CREATE TABLE tab_foo (
		omega bigint(20) DEFAULT NULL
			))");

	TabSample tab;
	// clear the table
	db.run(remove_from(tab).where(true));

	// explicit all_of(tab)
	for(const auto& row : db.run(select(all_of(tab)).from(tab).where(true)))
	{
		std::cerr << "row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma <<  std::endl;
	};
	std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
	// selecting a table implicitly expands to all_of(tab)
	for(const auto& row : db.run(select(all_of(tab)).from(tab).where(true)))
	{
		std::cerr << "row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma <<  std::endl;
	};
	// selecting two multicolumns
	for(const auto& row : db.run(select(multi_column(tab.alpha, tab.beta, tab.gamma).as(left), multi_column(all_of(tab)).as(tab)).from(tab).where(true)))
	{
		std::cerr << "row.left.alpha: " << row.left.alpha << ", row.left.beta: " << row.left.beta << ", row.left.gamma: " << row.left.gamma <<  std::endl;
		std::cerr << "row.tabSample.alpha: " << row.tabSample.alpha << ", row.tabSample.beta: " << row.tabSample.beta << ", row.tabSample.gamma: " << row.tabSample.gamma <<  std::endl;
	};

	// insert
	std::cerr << "no of required columns: " << TabSample::_required_insert_columns::size::value << std::endl;
	db.run(insert_into(tab).default_values());
	db.run(insert_into(tab).set(tab.gamma = true));
	auto di = dynamic_insert_into(db, tab).dynamic_set(tab.gamma = true);
	di.insert_list.add(tab.beta = "");
	db.run(di);

	// update
	db.run(update(tab).set(tab.gamma = false).where(tab.alpha.in(1)));
	db.run(update(tab).set(tab.gamma = false).where(tab.alpha.in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));

	// remove
	db.run(remove_from(tab).where(tab.alpha == tab.alpha + 3));


	decltype(db.run(select(all_of(tab)))) result;
	result = db.run(select(all_of(tab)).from(tab).where(true));
	std::cerr << "Accessing a field directly from the result (using the current row): " << result.begin()->alpha << std::endl;
	std::cerr << "Can do that again, no problem: " << result.begin()->alpha << std::endl;

	auto tx = start_transaction(db);
	TabFoo foo;
	for (const auto& row : db.run(
				select(all_of(tab), select(max(foo.omega)).from(foo).where(foo.omega > tab.alpha)).from(tab).where(true)))
	{
		int x = row.alpha;
		int a = row.max;
		//std::cerr << "-----------------------------" << row.beta << std::endl;
	}
	tx.commit();

	for (const auto& row : db.run(select(tab.alpha).from(tab.join(foo).on(tab.alpha == foo.omega)).where(true)))
	{
		std::cerr << row.alpha << std::endl;
	}

	for (const auto& row : db.run(select(tab.alpha).from(tab.left_outer_join(foo).on(tab.alpha == foo.omega)).where(true)))
	{
		std::cerr << row.alpha << std::endl;
	}

	auto ps = db.prepare(select(all_of(tab)).from(tab).where(tab.alpha != parameter(tab.alpha) and tab.beta != parameter(tab.beta) and tab.gamma != parameter(tab.gamma)));
	ps.params.alpha = 7;
	ps.params.beta = "wurzelbrunft";
	ps.params.gamma = true;
	for (const auto& row: db.run(ps))
	{
		std::cerr << "bound result: alpha: " << row.alpha << std::endl;
		std::cerr << "bound result: beta: " << row.beta << std::endl;
		std::cerr << "bound result: gamma: " << row.gamma << std::endl;
	}

	std::cerr << "--------" << std::endl;
	ps.params.gamma = "false";
	for (const auto& row: db.run(ps))
	{
		std::cerr << "bound result: alpha: " << row.alpha << std::endl;
		std::cerr << "bound result: beta: " << row.beta << std::endl;
		std::cerr << "bound result: gamma: " << row.gamma << std::endl;
	}

	std::cerr << "--------" << std::endl;
	ps.params.beta = "kaesekuchen";
	for (const auto& row: db.run(ps))
	{
		std::cerr << "bound result: alpha: " << row.alpha << std::endl;
		std::cerr << "bound result: beta: " << row.beta << std::endl;
		std::cerr << "bound result: gamma: " << row.gamma << std::endl;
	}

	auto pi = db.prepare(insert_into(tab).set(tab.beta = parameter(tab.beta), tab.gamma = true));
	pi.params.beta = "prepared cake";
	std::cerr << "Inserted: " << db.run(pi) << std::endl;

	auto pu = db.prepare(update(tab).set(tab.gamma = parameter(tab.gamma)).where(tab.beta == "prepared cake"));
	pu.params.gamma = false;
	std::cerr << "Updated: " << db.run(pu) << std::endl;

	auto pr = db.prepare(remove_from(tab).where(tab.beta != parameter(tab.beta)));
	pr.params.beta = "prepared cake";
	std::cerr << "Deleted lines: " << db.run(pr) << std::endl;

	return 0;
}
