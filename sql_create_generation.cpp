#include "shared.h"


string flush_create_buffer() {
    // for each element in the create buffer,
    // add generate_sql_create(element) to
    // the result.
    string res;
    for(auto table : Data::sql_create_buffer) {
        res += generate_sql_create(table) + "\n\n";
    }
    for(auto table : Data::sql_create_buffer) {
        res += generate_bridge_tables(table);
    }

    Data::sql_create_buffer.clear();

    return res;
}


string generate_sql_create(pair<string, vector<string>> input_table) {
    // this function is called after the needed
    // metadata is saved, in the create buffer,
    // tables are saved the way that non-reference
    // fields are save as comma seperated, and
    // reference fields are not saved at all,
    // to define them in sql, we have to look at:
    // Data::table_relationss and
    // Data::reference_isMultiple.

    string res = "create table " + sqlize_table_name(input_table.first) + " (\n";
    res += spaces(5) + "id uint_128 not null,\n";
    for(auto field : input_table.second) {
        res += (spaces(5) + field + " not null,\n");
    }

    // search the whole Data::table_relations and
    // collect relations that start with this tale
    // name, and then use Data::reference_isMultiple
    // to find weather it's multiple or not.
    vector<pair<pair<string, string>, string>> relations;
                    // for example: (Car, owner) -> Person
    for(auto relation : Data::table_relations) {
        if(relation.first.first == input_table.first) {
            relations.push_back(relation);
        }
    }

    for(auto relation : relations) {
        if(!Data::reference_isMultiple.at(relation.first)) {
            // single reference
            res += (spaces(5) + relation.first.second + " references ");
            res += (relation.second + ",\n");
        }
    }
    res.pop_back();
    res.pop_back();
    res += "\n);";

    return res;
}


string generate_bridge_tables(pair<string, vector<string>> input_table) {

    vector<pair<pair<string, string>, string>> relations;
                    // for example: (Car, owner) -> Person
    for(auto relation : Data::table_relations) {
        if(relation.first.first == input_table.first) {
            relations.push_back(relation);
        }
    }
    vector<pair<string, string>> bridge_tables;
    for(auto relation : relations) {
        if(Data::reference_isMultiple.at(relation.first)) {
            bridge_tables.push_back(make_pair(relation.first.first,
                                              relation.first.second));
        }
    }
    string res;
    for(auto table : bridge_tables) {
        string table_name = table.first + "_" + table.second;
        table_name = sqlize_table_name(table_name);
        res += "create table " + table_name + " (\n" + spaces(5) + table.first +
                    "_id,\n" + spaces(5) + table.second + "_id\n);\n\n";
    }
    return res;
}


string add_to_create_buffer(string tns_command) {

    // first of all: table name must start with
    // an upper case letter
    string table_name = extract_table_name_from_create(tns_command);
    bool ok = check_capital_first_letter(table_name);
    if(!ok) {
        return "Error:{table names must be started with upper case letters}";
    }

    // seperate fields by comma, then add
    // the non-reference fields to
    // Data::sql_create_buffer
    // and ignore the reference fields.
    vector<string> tokenized_create = tokenize_create(tns_command);

    vector<string> table_fields;
    // extract non-reference fields
    for(auto field : tokenized_create) {
        if(!is_reference_field(field)) {
            table_fields.push_back(field);
        }
    }

    Data::sql_create_buffer.push_back(make_pair(table_name, table_fields));

    return "";
}


bool check_capital_first_letter(string input) {

    if(input[0] >= 'A' && input[0] <= 'Z') {
        return true;
    }
    return false;
}


bool has_explicit_from_name(string from_part) {

    if(string_contains(from_part, "as")) {
        return true;
    }
    return false;
}


void add_target_to_references(vector<string>& tokenized_select,
                              vector<string>& tokenized_where) {

    for(int i = 0; i < tokenized_select.size(); i++) {
        tokenized_select[i] = "target." + tokenized_select[i];
    }

    set<string> keywords = {"some", "of", "provide", "end", "and", "or"};
    bool is_in_nested = false;
    bool is_in_provide = false;
    for(int i = 0; i < tokenized_where.size(); i++) {
        string condition = tokenized_where[i];

        add_spaces_for_operators(condition);
        auto space_seperated = seperate_by_spaces(condition);

        string changed_condition;
        for(auto expression : space_seperated) {
            if(is_alphabetic(expression[0]) &&
                            (keywords.find(expression) == keywords.end()) &&
                            (!is_in_provide)) {
                    changed_condition += ("target." + expression);
            }
            else {
                if(expression == "of") {
                    is_in_nested = true;
                }
                if(expression == "provide") {
                    is_in_provide = true;
                }
                if(expression == "end") {
                    is_in_provide = false;
                    is_in_nested = false;
                }
                changed_condition += expression;
            }
            changed_condition += " ";
        }
        tokenized_where[i] = changed_condition;
    }
}

