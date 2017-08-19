#include "shared.h"


bool is_applied() {
    bool res = false;
    if(Data::apply_buffer.empty()) {
        res = true;
    }
    return res;
}


string apply() {

    // check for all needed tables in 'table_relations'
    // to be existing in the 'tables', and if not,
    // roll_back().
    // then empty the buffer.

    string err_message;

    vector<string> needed;
    for(auto relation : Data::table_relations) {
        needed.push_back(relation.first.first);
        needed.push_back(relation.second);
    }

    // unify
    sort(needed.begin(),needed.end());
    auto ut = unique(needed.begin(),needed.end());
    needed.resize(ut - needed.begin());

    bool ok = true;
    for(auto element : needed) {
        auto ft = Data::tables.find(element);
        if(ft == Data::tables.end()) {
            err_message += "Error: {" + element + " doesn't exist }";
            ok = false;
            break;
        }
    }

    if(!ok) {
        roll_back();
    }

    Data::apply_buffer.clear();

    return err_message;
}



void roll_back() {

    // remove anything in 'apply_buffer' from 'tables'
    //   and 'table_relations'.

    for(auto name : Data::apply_buffer) {

        // remove from 'tables'
        auto ft = Data::tables.find(name);
        if(ft != Data::tables.end()) {
            Data::tables.erase(ft);
        }

        // remove from 'table_relations'
        vector<pair<string,string>> to_remove;
        for(auto relation : Data::table_relations) {
            if(relation.first.first == name ||
                        relation.second == name) {
                to_remove.push_back(relation.first);
            }
        }

        for(auto element : to_remove) {
            Data::table_relations.erase(element);
        }

        // remove from 'reference_isMultiple'
        to_remove.clear();
        for(auto reference : Data::reference_isMultiple) {
            if(reference.first.first == name) {
                to_remove.push_back(reference.first);
            }
        }

        for(auto element : to_remove) {
            Data::reference_isMultiple.erase(element);
        }
    }

    Data::apply_buffer.clear();
}

