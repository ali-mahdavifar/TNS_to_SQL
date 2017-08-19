#include "shared.h"






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












