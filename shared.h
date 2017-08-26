#ifndef SHARED
#define SHARED


#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<set>
#include<sstream>
#include<utility>
#include<algorithm>
#include<stack>
#include<cstdlib>
#include<ctime>
#include "format.h"
using namespace std;

void ui_infinite_loop();

string sql_result(string tns_query);

string get_tns_query();

void parse_to_phases(string tns_query, string& select_part, string& from_part, string& where_part);

vector<string> tokenize_select(string select_part);

vector<string> tokenize_where(string where_part);

vector<pair<string, string>> tokenize_from(string from_part);

vector<string> seperate_by_dots(string input);

void add_all_needed_compounds(vector<string>& container, string dotted_input);

string join_condition(string compound);

string table_name_from_compound(string compound);

vector<string> seperate_by_underlines(string input);

vector<string> seperate_by_commas(string input);

void fill_from_tables_names(vector<pair<string, string>> tokenized_from);

string save_table_metadata(string tns_query);

string create_result(string tns_query);

vector<string> seperate_by_spaces(string input);

bool is_reference_field(string field_def);

string save_reference_metadata(string table_name, string field_def);

void format_spaces(string& tns_query);

void print_existing_metadata();

string generate_sql_select(vector<string> tns_select_vec);

string generate_sql_from(vector<string> compounds);

string generate_sql_where(vector<string> tokenized_where);

bool is_alphabetic(char c);

bool is_operator(char c);

void add_spaces_for_operators(string& input);

bool is_alphanumeric_or_quote(char c);

char lower_case(char c);

bool check_table_for_existence(string name);

string apply();

void roll_back();

bool is_applied();

void set_output_color(string result);

void prune_result(string& result);

string sqlize_table_name(string input);

string generate_sql_command(string tns_command);

string generate_sql_query(string tns_query);

bool is_query(string tns_command);

string generate_sql_create(pair<string, vector<string> > table);

bool is_apply(string tns_command);

bool is_create(string tns_command);

vector<string> tokenize_create(string tns_command);

string extract_table_name_from_create(string tns_command);

string add_to_create_buffer(string tns_command);

string flush_create_buffer();

string extract_table_name_from_create(string tns_command);

bool check_for_injection_and_remove_braces(string& tns_query);

vector<pair<string, string>> merge_brace_keys(vector<pair<int, string>> open_brace_keys,
                                              vector<pair<int, string>> close_brace_keys);

string remove_braces(string tns_query, vector<int> open_brace_indices,
                                       vector<int> close_brace_indices);

bool has_explicit_from_name(string from_part);

void add_target_to_references(vector<string>& tokenized_select,
                              vector<string>& tokenized_where);

vector<vector<string>> split_compounnds(vector<string> compounds);

string first_word_of_compound(string compound);

vector<string> filter_compounds(vector<string> compounds, string filter);

string extract_error(string input);

string generate_bridge_tables(pair<string, vector<string>> input_table);

void assign_to_raw_str(char dest[], string source);

bool check_capital_first_letter(string input);

string generate_sql_plural_condition(string first_part, string domain, string sub_cond);

string sqlize_table_name(string input);

vector<string> generate_compounds_for_nested_query(string domain, string sub_cond);

void parse_plural_condition(string condition, string& first_part,
                                              string& domain, string& sub_cond);

bool is_plural_condition(string condition);

string reverse_condition(string condition);

bool is_last_ref_multiple(string compound);

string last_reference(string compound);

string drep_vector_string(vector<string> input);

string place_arguments(string input);

map<string, string> arguments_to_place(string input);

string cut_the_argument_part(string input);

string replace_arguments_and_add_braces(string input, map<string, string> arguments);


// Global Data

namespace Data{

extern string left_out_input;

extern set<string> apply_buffer;

extern set<string> tables;

extern map<string, string> from_table_names;

extern map<pair<string, string>, string> table_relations;

extern map<pair<string, string>, bool> reference_isMultiple;

extern vector<pair<string, vector<string>>> sql_create_buffer;

//extern vector<pair<string, string>> bridge_tables;

}





#endif // SHARED

