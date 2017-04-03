/*
 * Bailey Strom-Pillar
 * Milestone1
 * Limited query types handled
 */ 
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "db_cxx.h"
#include "SQLParser.h"
using namespace std; 
using namespace hsql;

class Shell {
	string QUIT = "quit";
	bool initialized = false; 
	
	/*Initializes and opens a DB env at the prompted location
	 * File must exist
	 */
	void initialize_db_env(string file)
	{
	    string environment_dir = file; 
		
		DbEnv env(0U);
		env.set_message_stream(&cout);
		env.set_error_stream(&cerr);
		env.open(environment_dir.c_str(), DB_CREATE | DB_INIT_MPOOL, 0);
		
		cout << "Environment created at " << "(" << file << ")"<< endl; 
		cout << endl;
		initialized = true; 
	}

	/**
	 * Determines expression type and handles appropriately
	 */
	void handleExpressions(const Expr* ex)
	{
		switch (ex->type) {
		case kExprStar:
			cout << "*"; 
			break;
		case kExprColumnRef:
			if (ex->table != NULL) 
			{
				cout << ex->table << ".";
			}
			cout << ex->name; 
			break;
		case kExprOperator:
			handleExpressions(ex->expr);
			cout << " " << ex->opChar << " ";
			handleExpressions(ex->expr2);
			break;
		case kExprLiteralFloat:
			cout << ex->fval;
			break;
		case kExprLiteralInt:
			cout << ex->ival;
			break;
		case kExprLiteralString:
			cout << ex->name;
			break;
		default:
			cout << "/nInvalid expression" << endl;
			return;
		}
		if (ex->alias != NULL) {
			cout << "AS ";
			cout << ex->alias << " "; 
		}
	}

	/*
	 * Determines join type
	 */
	void handleJoin(JoinDefinition* j_def)
	{
		handleTableInfo(j_def->left);
		switch(j_def->type)
		{
		case kJoinInner: 
			cout << "INNER JOIN ";
			break;
		case kJoinOuter:
			cout << "OUTER JOIN ";
			break;
		case kJoinLeft:
			cout << "LEFT JOIN ";
			break;
		case kJoinRight: 
			cout << "RIGHT JOIN ";
			break;
		case kJoinLeftOuter: 
			cout << "LEFT OUTER JOIN ";
			break;
		case kJoinRightOuter:
			cout << "RIGHT OUTER JOIN ";
			break;
		case kJoinCross: 
			cout << "JOIN CROSS";
		case kJoinNatural: 
			cout << "NATURAL JOIN ";
			break;
		default: 
			break;
		}

		handleTableInfo(j_def->right);
		cout << "ON ";
		handleExpressions(j_def->condition);
		cout << " ";
	}

	/*
	 *  Determines table type and pulls out appropriate information
	 */
	void handleTableInfo(TableRef * t_info)
	{
		switch (t_info->type)
		{
		case kTableName:
			cout << t_info->name << " ";
			break;
		case kTableJoin:
			handleJoin(t_info->join);
			break;
		case kTableCrossProduct: 
			for (TableRef* tbl : *t_info->list)
			{
				handleTableInfo(tbl);
			}
			break;
		default: 
			cout << "/nInvalid table information" << endl;
			break;
		}	

		if (t_info->alias != NULL) {
			cout << "AS ";
			cout << t_info->alias << " ";
		}
	}

	/*
	 * Handles select statements
	 */
	void handleSelectStatement(const SelectStatement* s_stmt)
	{
		cout << "SELECT ";
		int size = (*s_stmt->selectList).size();
		int i = 0;
		for (Expr* expr : *s_stmt->selectList)
		{
			handleExpressions(expr);
			i++;
			if(i < size) cout << ", ";
		}
		cout << " FROM ";
		handleTableInfo(s_stmt->fromTable);

		if(s_stmt->whereClause != NULL)
		{
			cout << "WHERE ";
			handleExpressions(s_stmt->whereClause->expr);
			cout << " " << s_stmt->whereClause->opChar << " ";
			handleExpressions(s_stmt->whereClause->expr2);
		}
	}

	/*
	 * Handles create statements
	 */
	void handleCreateStatement(const CreateStatement* c_stmt)
	{
		cout << "CREATE ";
		int size = (*c_stmt->columns).size();
		int i = 0;
		switch(c_stmt->type)
		{
		case CreateStatement::kTable: 
			cout << "TABLE " << c_stmt->tableName << " (";
			for (ColumnDefinition* c_def : *c_stmt->columns)
			{
				cout << *c_def->name;
				switch(c_def->type)
				{
				case ColumnDefinition::UNKNOWN: 
					cout << " UNKNOWN";
					break;
				case ColumnDefinition::TEXT: 
					cout << " TEXT";
					break;
				case ColumnDefinition::INT: 
					cout << " INT";
					break;
				case ColumnDefinition::DOUBLE: 
					cout << " DOUBLE";
					break;
				default: ;
				}
				i++;
				if (i < size) cout << ", ";
			}
			cout << ")";
			break;
		default: 
			cout << "/nInvalid create statement" << endl;
			break;
		}
	}
	
	/*
	 * Determins the type of SQLStatement and hands to correct function for handling
	 * Currently only select and create statements are handled
	 */
	void handleQuery(const SQLStatement * stmt)
	{
		switch(stmt->type())
		{
			case kStmtSelect:
				handleSelectStatement((const SelectStatement*)stmt);
				break;
			case kStmtCreate:
				handleCreateStatement((const CreateStatement*)stmt);
				break;
			default:
				break;
		}
		cout << endl;
	}
	
	/*
	 * Handles the control flow; sets up DB env and runs shell
	 */
	public: 
        void run(string file)
		{
			if(!initialized)
			{
				initialize_db_env(file);
				if(!initialized)
				{
					return; 
				}
			}
			
			cout << "Type quit to end" << endl;
			
			while(true)
			{
				string sql;
				cout << "SQL> ";
				getline(cin, sql);
				
				if(sql.compare(QUIT) == 0)
				{
					return; 
				} else{
					SQLParserResult * result = SQLParser::parseSQLString(sql);
					if(result->isValid()){
						handleQuery(result->getStatement(0));
					} else{
						cout << "Invalid SQL: " << sql << endl; 
					}
				}
			}	
		}
};

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cout << "Please enter DB file path." << endl;
	}

	string file(argv[1]);
	cout << file << endl;
	Shell ss;
	ss.run(file);

	//for screenshotting the output 
	system("pause");
}
