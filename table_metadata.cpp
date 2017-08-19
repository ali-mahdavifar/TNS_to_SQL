#include "shared.h"


void fill_from_tables_names(vector<pair<string, string>> tokenized_from) {


    Data::from_table_names.clear();
    for(auto elem : tokenized_from) {
        Data::from_table_names[elem.first] = elem.second;
    }
}


string save_table_metadata(string tns_query) {

    format_spaces(tns_query);
    string err;
    // this method creates an effect 'only'
    // if the query is a create action
    if(tns_query == "apply;") {
        apply();
        return err;
    }

    string temp;
    for(auto it = tns_query.begin(); it != tns_query.begin()+6; it++) {
        temp += *it;
    }
    if(temp != "create") {
        return err;
    }

    // Extract table_name here...
    string table_name = extract_table_name_from_create(tns_query);
    Data::apply_buffer.insert(table_name);
    Data::tables.insert(table_name);

    vector<string> tokenized_create = tokenize_create(tns_query);
    // now 'tokenized_create' is seperated by comma

    // content in 'tokenized_create' can be declarations
    // of normal fields or references.
    // references can be multiple and their return
    // names are also references.

    // here we have nothing necessary to do with normal fields,
    // but they can be stored only for the sake of error detection.

    // for each line, check if it's a reference, if so, save
    // related metadata.

    for(auto elem : tokenized_create) {   // comma seperated
        if(is_reference_field(elem)) {
            err = save_reference_metadata(table_name, elem);
            if(err != "") {
                return err;
            }
        }
    }
    return err;
}


bool is_reference_field(string field_def) {

    // see if there is the word 'reference' in the sentence
    // that's enough
    stringstream ss(field_def);
    string temp;
    bool res = false;

    while(ss >> temp) {
        if(temp == "reference") {
            res = true;
            break;
        }
    }

    return res;
}


string save_reference_metadata(string table_name, string field_def) {    // field_def is a whole definition

    // we want:
    //      Data :: table_relations[make_pair(source_table, field_name)] -> destination_table
    //      Data :: reference_isMultiple[make_pair(source_table, field_name)] -> true/false

    // we have to extract three strings: source_table, field_name, destination_table
    // plus two booleans: isForwardMultiple, isBackwardMultiple.
    string err_message;

    string source_table = table_name;
    vector<string> space_seperated = seperate_by_spaces(field_def);
    //    'father reference to Person returned by multiple children'
    // or 'children multiple reference to Person returned by father'

    // everything before 'returned by' relates to the forward pass

    auto it = space_seperated.begin();
    string field_name = *it;
    it++;
    bool isForwardMultiple = false;
    bool isBackwardMultiple = false;

    if(*it == "multiple") {
        isForwardMultiple = true;
    }
    else if(*it != "reference") {
        set_red();
        err_message += "error at table_metadata :: line 212";
        set_white();
    }

    while(true) {
        if(*it == "to") {
            break;
        }
        it++;
    }

    it++;
    string destination_table = *it;

    Data::table_relations[make_pair(source_table, field_name)] = destination_table;
    Data::reference_isMultiple[make_pair(source_table, field_name)] = isForwardMultiple;

    // check if there is a backward pass
    it++;

    if(it == space_seperated.end()) {
        // time to return
        return err_message;
    }

    if(*it == "returned") {
        // right to go
    }
    else {
        set_red();
        err_message += "error at table_metadata :: line 247";
        set_white();
    }

    // do the backward pass
    string reverse_field_name;

    it++; // by
    it++;

    if(*it == "multiple") {
        isBackwardMultiple = true;
        it++;
        reverse_field_name = *it;
    }
    else {
        isBackwardMultiple = false;
        reverse_field_name = *it;
    }

    Data::table_relations[make_pair(destination_table, reverse_field_name)] = source_table;
    Data::reference_isMultiple[make_pair(destination_table, reverse_field_name)] = isBackwardMultiple;

    return err_message;
}


bool check_table_for_existence(string name) {

    bool res = true;
    auto it = Data::tables.find(name);
    if(it == Data::tables.end()) {
        res = false;
    }

    return res;
}









