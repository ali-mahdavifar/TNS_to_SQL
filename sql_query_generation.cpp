#include "shared.h"




// the query can be only 'select' now
// but later can be extended to 'insert' and 'delete'
string generate_sql_query(string tns_query) {

    if(!is_applied()) {
        return "Error:{there are unapplied changes}";
    }

//    bool ok = check_for_injection_and_remove_braces(tns_query);
//    if(!ok) {
//        return "Error:{query suspected for injection}";
//    }

    tns_query = place_arguments(tns_query);

    string select_part, from_part, where_part;
    parse_to_phases(tns_query, select_part, from_part, where_part);

    bool has_no_explicit_from_name = !has_explicit_from_name(from_part);
    auto tokenized_select = tokenize_select(select_part);
    auto tokenized_from = tokenize_from(from_part);
    auto tokenized_where = tokenize_where(where_part);

    if(has_no_explicit_from_name) {
        add_target_to_references(tokenized_select, tokenized_where);
    }

    for(auto p : tokenized_from) {
        bool ok = check_table_for_existence(p.second);
        if(!ok) {
            return "Error:{table " + p.second + " doesn't exist}";
        }
    }

    fill_from_tables_names(tokenized_from);

    string res;
    vector<string> compounds;

    for(auto elem : tokenized_select) {
        add_all_needed_compounds(compounds, elem);
    }
    for(auto elem : tokenized_where) {
        if(!is_plural_condition(elem)) {
            add_all_needed_compounds(compounds, elem);
        }
    }

    res += generate_sql_select(tokenized_select);
    res += generate_sql_from(compounds);
    res += generate_sql_where(tokenized_where);
    res += ";";

    return res;
}


// 'container' is not empty, check not to duplicate something
void add_all_needed_compounds(vector<string>& container, string dotted_input) {

    vector<string> dot_seperated = seperate_by_dots(dotted_input);

    for(auto it = dot_seperated.begin(); it != dot_seperated.end(); it++) {
        // for each one, create the underlined chain of names from
        // begin to that.
        string temp;
        for(auto jt = dot_seperated.begin(); jt != it; jt++) {
            temp += (*jt + "_");
        }
        if(temp.size() >= 1) {
            temp.pop_back();
            // check the container, then add
            auto ft = find(container.begin(), container.end(), temp);
            if(ft == container.end()) {
                container.push_back(temp);
            }
        }
    }
}


string join_condition(string compound) {

    string temp = compound;
    for(auto it = temp.end()-1; it != temp.begin(); it--) {
        if(*it == '_') {
            *it = '.';
            break;
        }
    }
    string res = " on ";
    res += temp + " = ";
    res += compound + ".id";
    return res;
}





string generate_sql_select(vector<string> tns_select_vec) {

    string res = "select\n";
    res += spaces(5);

    for(auto it = tns_select_vec.begin(); it != tns_select_vec.end(); it++) {
        auto dot_seperated = seperate_by_dots(*it);
        for(auto jt = dot_seperated.begin(); jt != dot_seperated.end() - 1; jt++) {
            res += *jt;
            res += "_";
        }
        res.pop_back();
        res += ".";
        res += dot_seperated.back();
        res += ",\n";
        if(it != tns_select_vec.end() - 1) {
            res += spaces(5);
        }
    }

    return res;
}


string generate_sql_from(vector<string> compounds) {

    string res;
    res += "from\n";
    res += spaces(5);

    auto splitted  = split_compounnds(compounds);
    for(auto compound_set : splitted) {
        for(auto it = compound_set.begin(); it != compound_set.end(); it++) {
            if(it != compound_set.begin()) {
                res += "join ";
            }
            res += sqlize_table_name(table_name_from_compound(*it)) + " as ";
            res += *it;
            if(it != compound_set.begin()) {
                res += join_condition(*it);
            }
            res += "\n";
            if(it != compound_set.end() - 1) {
                res += spaces(5);
            }
        }
        res += spaces(5) + ",\n" + spaces(5);
    }
    for(int i = 0; i < 12; i++) {
        res.pop_back();
    }

    return res;
}


string generate_sql_where(vector<string> tokenized_where) {

    // input is tokenized by 'and' and 'or'
    string res = "where\n";
    res += spaces(5);

    for(auto condition : tokenized_where) {
        if(condition == "and" || condition == "or") {
            res += condition + "\n" + spaces(5);
            continue;
        }
        add_spaces_for_operators(condition);
        auto space_seperated = seperate_by_spaces(condition);

        if(is_plural_condition(condition)) {
            string first_part, domain, sub_cond;
            parse_plural_condition(condition, first_part, domain, sub_cond);
            res += generate_sql_plural_condition(first_part, domain, sub_cond);
        }
        else {
            for(auto expression : space_seperated) {

                if(is_alphabetic(expression[0])) {
                    auto dot_seperated = seperate_by_dots(expression);
                    string compound;
                    string target_nickname = dot_seperated[0];
                    compound = dot_seperated[0];

                    for(int i = 1; i < dot_seperated.size() - 1 ; i++) {
                        if(i > 1) {
                            compound += ("_" + dot_seperated[i - 1]);
                        }
                        string target_name;
                        try {
                            target_name = Data::from_table_names.at(target_nickname);
                        }
                        catch(out_of_range e) {
                            return "Error:{'" + target_nickname + "' is not defined }";
                        }
                        bool is_mul = false;

                        if(i == 0) {
                            try {
                                is_mul = Data::reference_isMultiple.at(
                                            make_pair(target_name, dot_seperated[i]));
                            }
                            catch(out_of_range e) {
                                return "Error:{table '" +
                                        target_name + "' " +
                                        "does not have the reference '" + dot_seperated[i] + "'}";
                            }
                        }
                        else {
                            try{
                                is_mul = Data::reference_isMultiple.at(
                                            make_pair(table_name_from_compound(compound), dot_seperated[i]));
                            }
                            catch(out_of_range e) {
                                return "Error:{table '" +
                                        table_name_from_compound(compound) + "' " +
                                        "does not have the reference '" + dot_seperated[i] + "'}";
                            }
                        }

                        if(is_mul == true) {
                            if(i == 0) {
                                return "Error:{ reference '" + dot_seperated[i] + "' of '" +
                                        target_name + "' is multiple}";
                            }
                            else {
                                return "Error:{ reference '" + dot_seperated[i] + "' of '" +
                                        table_name_from_compound(compound) + "' is multiple}";
                            }
                        }
                    }

                    for(auto it = dot_seperated.begin(); it != dot_seperated.end() - 1; it++) {
                        res += *it;
                        res += "_";
                    }
                    res.pop_back();
                    res += ".";
                    res += dot_seperated.back();
                }
                else {
                    res += expression;
                }

                res += " ";
            }
        }
        res += "\n";
        res += spaces(5);
    }
    for(int i = 0; i < 5; i++) {
            res.pop_back();
    }

    return res;
}


string generate_sql_plural_condition(string first_part, string domain, string sub_cond) {

    if(first_part == "all of") {
        return generate_sql_plural_condition("none of", domain, reverse_condition(sub_cond));
    }
    if(first_part == "none of") {
        return "not " + generate_sql_plural_condition("some of", domain, sub_cond);
    }
    if(first_part != "some of") {
        return "Error:{irrelevent first part '" + first_part + "'}";
    }

    // first_part is "some of"
    vector<string> compounds;
    compounds = generate_compounds_for_nested_query(domain, sub_cond);

    string res;
    res += "exists\n";
    res += spaces(5) + "select *\n";
    res += spaces(5) + "from\n";

    res += spaces(10);

    /////////////// nested from

    // something si missing here
    // first we should seperate the compounds
    // by their initial target

    compounds.erase(compounds.begin()); // remove the initial target
    string first_comp_after_initial = compounds.front();
    res += sqlize_table_name(table_name_from_compound(first_comp_after_initial));
    res += " as " + first_comp_after_initial + "_2";

    compounds.erase(compounds.begin());
    string prev_comp = first_comp_after_initial;
    for(auto comp : compounds) {
        bool is_mul = is_last_ref_multiple(comp);
        if(!is_mul) {
            res += "\n" + spaces(10) + "join ";
            res += sqlize_table_name(table_name_from_compound(comp));
            res += " as " + comp + "_2";
            res += " on " + prev_comp + "_2." + last_reference(comp) +
                                                " = " + comp + "_2.id";
            prev_comp = comp;
        }
        else {
            res += "\n" + spaces(10) + "join ";
            res += sqlize_table_name(table_name_from_compound(prev_comp) +
                                             "_" + last_reference(comp));
            res += " as " + comp + "_ref_2";
            res += " on " +  prev_comp + "_2.id" + " = " + comp + "_ref_2" +
                                "." + table_name_from_compound(prev_comp) + "_id";
            res += "\n" + spaces(10) + "join ";
            res += sqlize_table_name(table_name_from_compound(comp));
            res += " as " + comp + "_2";
            res += " on " + comp + "_ref_2" + "." + last_reference(comp) + "_id";
            res += " = " + comp + "_2.id";
        }
    }

    //////////////// nested where

    res += "\n" + spaces(5) + "where\n";
    res += spaces(10);
    res += seperate_by_dots(domain).at(0) + "." + seperate_by_dots(domain).at(1);
    res += " = ";
    res += seperate_by_dots(domain).at(0) + "_" + seperate_by_dots(domain).at(1);
    res += "_2.id\n";

    vector<string> conditions = tokenize_where(sub_cond);
    for(auto cond : conditions) {
        res += spaces(10);
        if(cond == "and" || cond == "or") {
            res += " " + cond + " ";
        }
        else {
            add_spaces_for_operators(cond);
            auto space_seperated = seperate_by_spaces(cond);
            for(auto expression : space_seperated) {
                if(is_alphabetic(expression[0])) {
                    auto dot_seperated = seperate_by_dots(domain + "." + expression);
                    for(auto it = dot_seperated.begin(); it != dot_seperated.end() - 1; it++) {
                        res += *it;
                        res += "_";
                    }
                    res += "2";

                    res += ".";
                    res += dot_seperated.back();
                }
                else {
                    res += expression;
                }
                res += " ";
            }
        }
        res += "\n";
    }


    return res;
}


// changes 'Car' to 'car_t'
string sqlize_table_name(string input) {

    if(string_contains(input, "Error")) {
        return input;
    }

    string res = input;
    res[0] = lower_case(res[0]);
    res += "_t";
    return res;
}


vector<string> generate_compounds_for_nested_query(string domain, string sub_cond) {

    vector<string> compounds;
    vector<string> conditions = tokenize_where(sub_cond);
    for(auto condition : conditions) {
        if(condition == "and" || condition == "or") {
            continue;
        }
        add_spaces_for_operators(condition);
        auto space_seperated = seperate_by_spaces(condition);
        for(auto expression : space_seperated) {
            if(is_alphabetic(expression[0])) {
                add_all_needed_compounds(compounds, domain + '.' + expression);
            }
        }
    }

    return compounds;
}


bool is_plural_condition(string condition) {

    vector<string> sp_sep = seperate_by_spaces(condition);

    if(sp_sep.size() < 2) {
        return false;
    }

    if(sp_sep.at(1) == "of") {
        return true;
    }
    return false;
}


string reverse_condition(string condition) {

    int index = 0;
    string op;
    for( ; index < condition.size(); index++) {
        if(condition[index] == '<') {
            if(condition[index + 1] == '=') {
                op = "<=";
            }
            else {
                op = "<";
            }
            break;
        }
        if(condition[index] == '>') {
            if(condition[index + 1] == '=') {
                op = ">=";
            }
            else {
                op = ">";
            }
            break;
        }
        if(condition[index] == '=') {
            op = "=";
            break;
        }
    }

    map<string, string> reverese;
    reverese["<"] = ">=";
    reverese["<="] = ">";
    reverese["="] = "!=";
    reverese[">="] = "<";
    reverese[">"] = "<=";

    for(int i = 0; i < op.size(); i++) {
        condition.erase(condition.begin() + index);
    }
    string rev = reverese[op];
    for(int i = 0; i < rev.size(); i++) {
        condition.insert(condition.begin() + index, rev[rev.size()-1-i]);
    }

    return condition;
}


bool is_last_ref_multiple(string compound) {

    auto sep = seperate_by_underlines(compound);
    string back = sep.back();
    sep.pop_back();
    string comp_minus_back;
    for(auto elem : sep) {
        comp_minus_back += elem;
        comp_minus_back += "_";
    }
    comp_minus_back.pop_back();
    bool res = Data::reference_isMultiple.at(make_pair(
                             table_name_from_compound(comp_minus_back), back));

    return res;
}


string last_reference(string compound) {

    auto sep = seperate_by_underlines(compound);
    return sep[sep.size()-1];
}













