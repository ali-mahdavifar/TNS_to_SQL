#include "shared.h"






string place_arguments(string input) {

    if(input.find("with") == string::npos) {
        return input;
    }

    map<string, string> arguments = arguments_to_place(input);

    input = cut_the_argument_part(input);

    string res = replace_arguments_and_add_braces(input, arguments);

    bool ok = check_for_injection_and_remove_braces(res);

    if(!ok) {
        return "Error:{query suspected for injection}";
    }

    return res;
}


map<string, string> arguments_to_place(string input) {

    string part1, part2;
    int index = input.find("with");
    for(int i = 0; i < index; i++) {
        part1.push_back(input[i]);
    }
    for(int i = index + 4; i < input.size(); i++) {
        part2.push_back(input[i]);
    }

    map<string, string> arguments;
    auto sep = seperate_by_commas(part2);
    for(auto elem : sep) {
        int i = 0;
        while(true) {
            if(elem[i] == '@') {
                break;
            }
            i++;
        }
        // elem[i] == '@'
        string temp1 = "@";
        i++;
        while(true) {
            if(elem[i] == ' ' || elem[i] == '=') {
                break;
            }
            temp1.push_back(elem[i]);
            i++;
        }
        while(true) {
            if(elem[i] != ' ' && elem[i] != '=') {
                break;
            }
            i++;
        }
        string temp2;
        while(true) {
            if(elem[i] == ' ' || elem[i] == ',' || elem[i] == ';' || i >= elem.size()) {
                break;
            }
            temp2.push_back(elem[i]);
            i++;
        }
        arguments.insert(make_pair(temp1, temp2));
    }

    return arguments;
}

string cut_the_argument_part(string input) {

    int pos = input.find("with");
    string res;
    for(int i = 0; i < pos; i++) {
        res.push_back(input[i]);
    }
    res.push_back(';');
    return res;
}


string replace_arguments_and_add_braces(string input, map<string, string> arguments) {

//    string res;
//    int i = 0;
    srand(time(0));

    for(auto it = arguments.begin(); it != arguments.end(); it++) {
        string key = it -> first;
        string value = it -> second;
        int pos1 = input.find(key);
        int pos2 = pos1 + key.size();
        string temp;
        for(int i = 0; i < pos1; i++) {
            temp.push_back(input[i]);
        }
        int random = rand() % 1000000;
        temp.push_back('{');
        temp += to_string(random);
        temp.push_back('{');
        temp += value;
        temp.push_back('}');
        temp += to_string(random);
        temp.push_back('}');
        for(int i = pos2; i < input.size(); i++) {
            temp.push_back(input[i]);
        }
        input = temp;
    }

    return input;
}


bool check_for_injection_and_remove_braces(string& tns_query) {
    // 1- find begin and end indices for curly braced passkeys.
    // 2- find begin and end indices for from and where words.
    // 3- seperate gathered passkeys into three parts ('select',
    //              'from', and 'where').
    // 4- do the stack thing on each part.
    // 5- remove them from the input.

    vector<int> open_brace_indices;
    vector<int> close_brace_indices;
    for(int i = 0; i < tns_query.size(); i++) {
        if(tns_query[i] == '{') {
            open_brace_indices.push_back(i);
        }
        if(tns_query[i] == '}') {
            close_brace_indices.push_back(i);
        }
    }

    if(open_brace_indices.size() != close_brace_indices.size()) {
        return false;
    }
    if((open_brace_indices.size() % 2) != 0) {
        return false;
    }
    if(open_brace_indices.empty()) {
        return true;
    }

    vector<pair<int, string>> open_brace_keys;
    vector<pair<int, string>> close_brace_keys;
    for(int i = 0; i < open_brace_indices.size(); i += 2) {
        string temp;
        for(int j = open_brace_indices[i] + 1; j < open_brace_indices[i + 1]; j++) {
            temp += tns_query[j];
        }
        open_brace_keys.push_back(make_pair(open_brace_indices[i], temp));
    }
    for(int i = 0; i < close_brace_indices.size(); i += 2) {
        string temp;
        for(int j = close_brace_indices[i] + 1; j < close_brace_indices[i + 1]; j++) {
            temp += tns_query[j];
        }
        close_brace_keys.push_back(make_pair(close_brace_indices[i], temp));
    }

    // do stack thing
    auto merged = merge_brace_keys(open_brace_keys, close_brace_keys);
    stack<string> key_stack;
    for(auto elem : merged) {
        if(elem.first == "open") {
            key_stack.push(elem.second);
        }
        else {
            if(key_stack.empty()) {
                return false;
            }
            auto temp = key_stack.top();
            key_stack.pop();
            if(temp != elem.second) {
                return false;
            }
        }
    }
    if(!key_stack.empty()) {
        return false;
    }

    // remove braces
    auto removed = remove_braces(tns_query, open_brace_indices, close_brace_indices);
    format_spaces(removed);
    tns_query = removed;

    return true;
}


vector<pair<string, string>> merge_brace_keys(vector<pair<int, string>> open_brace_keys,
                                              vector<pair<int, string>> close_brace_keys) {

    auto it = open_brace_keys.begin();
    auto jt = close_brace_keys.begin();

    vector<pair<string, string>> res;

    res.push_back(make_pair(string("open"), it->second));
    it++;

    while(it != open_brace_keys.end() && jt != close_brace_keys.end()) {

        if(it->first < jt->first) {
            res.push_back(make_pair(string("open"), it->second));
            it++;
        }
        else {
            res.push_back(make_pair(string("close"), jt->second));
            jt++;
        }
    }
    if(it == open_brace_keys.end()) {
        while(jt != close_brace_keys.end()) {
            res.push_back(make_pair(string("close"), jt->second));
            jt++;
        }
    }
    else {
        while(it != open_brace_keys.end()) {
            res.push_back(make_pair(string("open"), it->second));
            it++;
        }
    }

    return res;
}


string remove_braces(string tns_query, vector<int> open_brace_indices,
                                       vector<int> close_brace_indices) {
    set<int> ignored_indices;
    for(int i = 0; i < open_brace_indices.size(); i += 2) {
        for(int j = open_brace_indices[i]; j <= open_brace_indices[i + 1]; j++) {
            ignored_indices.insert(j);
        }
    }
    for(int i = 0; i < close_brace_indices.size(); i += 2) {
        for(int j = close_brace_indices[i]; j <= close_brace_indices[i + 1]; j++) {
            ignored_indices.insert(j);
        }
    }

    string res;
    for(int i = 0; i < tns_query.size(); i++) {
        auto ft = ignored_indices.find(i);
        if(ft == ignored_indices.end()) {
            res += tns_query[i];
        }
    }

    return res;
}












