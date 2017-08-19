#include "shared.h"




void parse_to_phases(string tns_query, string& select_part, string& from_part, string& where_part) {

    stringstream ss(tns_query);
    int flag = 0;

    while(1) {

        string temp;
        ss >> temp;

        if(temp == "select") {
            flag = 0;
            continue;
        }
        if(temp == "from") {
            flag = 1;
            continue;
        }
        if(temp == "where") {
            flag = 2;
            continue;
        }

        if(flag == 0) {
            select_part += (spaces(1) + temp);
        }
        else if(flag == 1) {
            from_part += (spaces(1) + temp);
        }
        else if(flag == 2) {
            where_part += (spaces(1) + temp);
        }

        if(temp.back() == ';') {
            break;
        }
    }
}


// tokenize by comma
vector<string> tokenize_select(string select_part) {

    vector<string> res;
    // do character by character
    string temp;
    for(auto it = select_part.begin(); it != select_part.end(); it++) {
        if( *it == ',') {
            res.push_back(temp);
            temp = "";
        }
        else if( *it == ' ') {
            continue;
        }
        else {
            temp += *it;
        }
    }

    res.push_back(temp);

    return res;
}

// tokenize by 'and' and 'or'
vector<string> tokenize_where(string where_part) {

    vector<string> res;
    // do character by character
    string temp;
    for(auto it = where_part.begin(); it != where_part.end(); it++) {

        if( (*it == 'a' && *(it+1) == 'n' && *(it+2) == 'd')
                        || (*it == 'o' && *(it+1) == 'r') ) {
            res.push_back(temp);
            if(*it == 'a') {
                it = it + 2;
                res.push_back("and");
            }
            else {
                it++;
                res.push_back("or");
            }
            temp = "";
        }
        else if((*it == 's' && *(it+1) == 'o' && *(it+2) == 'm' && *(it+3) == 'e')
              ||(*it == 'n' && *(it+1) == 'o' && *(it+2) == 'n' && *(it+3) == 'e')
              ||(*it == 'a' && *(it+1) == 'l' && *(it+2) == 'l')) {

            auto jt = it;
            while(true) {
                jt++;
                if(*(jt-1) == 'd' && *(jt-2) == 'n' && *(jt-3) == 'e') {
                    break;
                }
            }

            res.push_back(temp);
            temp = "";
            temp.assign(it, jt);

            res.push_back(temp);
            temp = "";
            it = jt;
        }
        else if(*it == ' ') {
            continue;
        }
        else {
            temp += *it;
        }
    }


    if(temp.size() > 0) {
        if(temp.back() == ';') {
            temp.pop_back();
        }
    }

    res.push_back(temp);

    for(int i = 0; i < res.size(); i++) {
        if(res[i] == "") {
            res.erase(res.begin() + i);
            i--;
        }
    }

    return res;
}


vector<pair<string, string>> tokenize_from(string from_part) {

    // for now, just return the input
    vector<pair<string, string>> res;
    format_spaces(from_part);

    if(!string_contains(from_part, "as")) {
        res.push_back(make_pair(string("target"), from_part));
    }
    else {
        //  Here  to be implemented
        auto comma_seperated = seperate_by_commas(from_part);
        for(auto exp : comma_seperated) {
            format_spaces(exp);
            string table_name;
            string table_nickname;
            int index = exp.find("as");
            for(int i = 0; i < index - 1; i++) {
                table_name += exp[i];
            }
            for(int i = index + 3; exp[i] != ' ' && i < exp.size(); i++) {
                table_nickname += exp[i];
            }
            res.push_back(make_pair(table_nickname, table_name));
        }
    }

    return res;
}


vector<string> seperate_by_dots(string input) {

    vector<string> res;
    string temp;

    for(auto it = input.begin(); it != input.end(); it++) {
        if(*it == '.') {
            res.push_back(temp);
            temp = "";
            continue;
        }
        else {
            temp += *it;
        }
    }

    res.push_back(temp);

    return res;
}

vector<string> seperate_by_spaces(string input) {

    vector<string> res;
    string temp;

    for(auto it = input.begin(); it != input.end(); it++) {
        if(*it == ' ') {
            res.push_back(temp);
            temp = "";
            continue;
        }
        else {
            temp += *it;
        }
    }

    res.push_back(temp);

    return res;
}

vector<string> seperate_by_commas(string input) {

    vector<string> res;
    string temp;

    for(auto it = input.begin(); it != input.end(); it++) {

        if(*it == ',') {
            res.push_back(temp);
            temp = "";
            continue;
        }
        else {
            temp += *it;
        }
    }

    res.push_back(temp);

    return res;
}

void format_spaces(string& tns_query) {

    // trim initial spaces
    // trim final spaces
    // no successive spaces in the middle

    // trim initial spaces
    while(tns_query[0] == ' ') {
        tns_query.erase(tns_query.begin());
    }

    //trim final spaces
    while(tns_query[tns_query.size()-1] == ' ') {
        tns_query.pop_back();
    }

    // no successive spaces in the middle
    int i = 0;
    bool flag = false;

    while(i < tns_query.size()) {
        if(tns_query[i] == ' ') {
            if(!flag) {
                flag = true;
                i++;
            }
            else {
                tns_query.erase(tns_query.begin() + i);
            }
        }
        else {
            i++;
            flag = false;
        }
    }
}


void add_spaces_for_operators(string& input) {

    for(int i = 1; i < input.size() - 1; i++) {
        if(is_operator(input[i])) {
            if(is_alphanumeric_or_quote(input[i-1])) {
                // add space here
                input.insert(input.begin() + i, ' ');
            }
            if(is_alphanumeric_or_quote(input[i+1])) {
                // add space after
                input.insert(input.begin() + i + 1, ' ');
            }
        }
    }
}


bool is_alphabetic(char c) {

    bool res = false;
    if(((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
        res = true;
    }

    return res;
}


bool is_operator(char c) {

    bool res = false;
    if(c == '<' || c == '>' || c == '=') {
        res = true;
    }

    return res;
}


bool is_alphanumeric_or_quote(char c) {

    bool res = false;
    if(is_alphabetic(c)) {
        res = true;
    }
    if(c >= '0' && c<= '9') {
        res = true;
    }
    if(c == '"' || c == '\'') {
        return true;
    }

    return res;
}


char lower_case(char c) {

    int offset = 'A' - 'a';
    if(c >= 'A' && c <= 'Z') {
        c = c - offset;
    }
    return c;
}


string extract_table_name_from_create(string tns_command) {

    string table_name;
    for(auto it = tns_command.begin() + 13; *it != ' '; it++) {
        table_name += *it;
    }
    return table_name;
}


// returns a vector of field definitions
vector<string> tokenize_create(string tns_command) {

    vector<string> tokenized_create;
    bool start = false;
    string temp = "";

    for(auto elem : tns_command) {
        if(!start) {
            if(elem == '(') {
                start = true;
            }
            continue;
        }
        if(elem == ',') {
            format_spaces(temp);
            tokenized_create.push_back(temp);
            temp = "";
            continue;
        }
        if(elem == ')') {
            break;
        }
        temp += elem;
    }

    format_spaces(temp);
    tokenized_create.push_back(temp);

    return tokenized_create;
}

// split the input 'compounds' into
// several parts:
// starting with { a },
// starting with { b },
// etc.
vector<vector<string>> split_compounnds(vector<string> compounds) {
    // read each element in 'compounds', then save their first
    // before-undeline term, and save them in a set, then for any
    // element in the set, crete it's own part.
    set<string> first_words;
    for(auto elem : compounds) {
        string temp = first_word_of_compound(elem);
        first_words.insert(temp);
    }
    vector<vector<string>> res;
    for(auto elem : first_words) {
        vector<string> temp = filter_compounds(compounds, elem);
        res.push_back(temp);
    }
    return res;
}


string first_word_of_compound(string compound) {

    string res;
    for(int i = 0; i < compound.size(); i++) {
        if(compound[i] == '_') {
            break;
        }
        res += compound[i];
    }
    return res;
}


vector<string> filter_compounds(vector<string> compounds, string filter) {

    vector<string> res;
    for(auto elem : compounds) {
        if(first_word_of_compound(elem) == filter) {
            res.push_back(elem);
        }
    }
    return res;
}


void assign_to_raw_str(char dest[], string source) {

    for(int i = 0; i < source.size(); i++) {
        dest[i] = source[i];
    }
    dest[source.size()] = '\0';
}


void parse_plural_condition(string condition, string& first_part,
                                              string& domain, string& sub_cond) {

    auto space_seperated = seperate_by_spaces(condition);
    vector<string> temp1, temp2, temp3;

    temp1.push_back(space_seperated[0]);
    temp1.push_back(space_seperated[1]);

    temp2.push_back(space_seperated[2]);

    for(int i = 4; i < space_seperated.size() - 1; i++) {
        temp3.push_back(space_seperated[i]);
    }

    for(auto elem : temp1) {
        first_part += elem + ' ';
    }
    for(auto elem : temp2) {
        domain += elem + ' ';
    }
    if(temp3.back() == "end") {
        temp3.pop_back();
    }
    for(auto elem : temp3) {
        sub_cond += elem + ' ';
    }


    format_spaces(first_part);
    format_spaces(domain);
    format_spaces(sub_cond);

}









