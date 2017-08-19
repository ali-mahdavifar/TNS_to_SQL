#include "shared.h"





string sql_result(string tns_command) {

    /* Example:
     *
     * ** input: **************
     *
     *
     * select price,
     *        owner.house.price
     *
     * from car
     *
     * where owner.father.house.price > 500 ;
     *
     *
     *
     * ** output: **************
     *
     *
     * select target.price
     *        target_owner_house.price
     *
     * from car_t as target
     * join person_t as target_owner    on    target.owner   =   target_owner.id
     * join house_t as target_owner_house    on    target_owner.house   =   target_owner_house.id
     * join person_t as target_owner_father    on    target_owner.father   =   target_owner_father.id
     * join house_t as target_owner_father_house    on    target_owner_father.house   =   target_owner_father_house.id
     *
     * where target_owner_father_house.price > 500 ;
     *
     */

    format_spaces(tns_command);

    string err = save_table_metadata(tns_command);
    if(err != "") {
        return "Error:{" + err + "}";
    }

    //
//    cout << "tns_command:" << endl;
//    cout << tns_command << "\n" << endl;
    //

    string res = generate_sql_command(tns_command);
    return res;
}


// the command can be a query, create, drop or apply
string generate_sql_command(string tns_command) {
    // if it's a create, add the sql create command
    // to Data::sql_create_buffer,
    // if it's an apply, flush thar buufer into
    // the result of this function.
    string res;

    if(is_create(tns_command)) {
        res = add_to_create_buffer(tns_command);
    }
    else if(is_apply(tns_command)) {
        res = flush_create_buffer();
    }
    else if(is_query(tns_command)) {
        res = generate_sql_query(tns_command);
    }

    return res;
}



bool is_query(string tns_command) {

    string temp;
    for(int i = 0; i < 6; i++) {
        temp += tns_command[i];
    }
    if(temp == "create" || temp == "apply;") {
        return false;  // is it's a 'create', nothing to do here
    }
    return true;
}


bool is_create(string tns_command) {

    string temp;
    for(int i = 0; i < 6; i++) {
        temp += tns_command[i];
    }
    if(temp == "create") {
        return true;
    }
    return false;
}


bool is_apply(string tns_command) {

    string temp;
    for(int i = 0; i < 6; i++) {
        temp += tns_command[i];
    }
    if(temp == "apply;") {
        return true;
    }
    return false;
}


