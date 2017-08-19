#include "shared.h"

namespace Data {

string left_out_input;

set<string> apply_buffer;

set<string> tables;

map<string, string> from_table_names;    // Car as a Person as b

map<pair<string, string>, string> table_relations;  // (Car, owner) -> Person

map<pair<string, string>, bool> reference_isMultiple;

map<pair<string, string>, string> reference_opposite_direction; // bridge tables are defined both ways
                                                                // but.. consider if someone is
                                                                // added to someones's followings
                                                                // then, definitely someone is added
                                                                // to the other way's followers.

vector<pair<string, vector<string>>> sql_create_buffer;  // {table_name, {table_fields}}

//vector<pair<string, string>> bridge_tables;       //  employee_projects_t

}
