#include "shared.h"


string table_name_from_compound(string compound) {

    // navigate to  table
    // example:
    // target_owner_father_house  ->  House
    auto names = seperate_by_underlines(compound);
    string temp = Data::from_table_names[names[0]];

    for(auto it = names.begin()+1; it != names.end(); it++) {
        try {
            temp = Data::table_relations.at(make_pair(temp, *it));
        }
        catch(exception e) {
            temp = "Error:{table '" + temp +
                        "' does not have the reference '" + *it + "'}";
            break;
        }
        if(temp == "") {
            temp = "Error:{reference (" + temp + ", " + *it + ") is empty}";
        }
    }

    return temp;
}



vector<string> seperate_by_underlines(string input) {

    vector<string> res;
    string temp;
    for(auto it = input.begin(); it != input.end(); it++) {
        if(*it == '_') {
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
