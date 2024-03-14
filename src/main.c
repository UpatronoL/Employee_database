#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "common.h"
#include "file.h"
#include "parse.h" 

void printf_usage(char * argv[]){
    printf("Usage: %s -n -f <filepath>\n", argv[0]);
    printf("\t -n - create new database file\n");
    printf("\t -f - (required) path to database file\n");
}

int main(int argc, char * argv[]) {

    bool newfile = false;
    char * filepath = NULL;
    char * addstring = NULL;
    char * nameToRemove = NULL;
    bool list = false;
    bool removestring = false;
    int c;

    int dbfd = -1;
    struct dbheader_t *dbhdr = NULL;
    struct employee_t *employees = NULL;

    while((c = getopt(argc, argv, "nf:ar:l")) != -1) {
        switch(c){
            case 'n':
                newfile = true;
                break;
            case 'f':
                filepath = optarg;
                break;
            case 'a':
                addstring = optarg;
                break;
            case 'l':
                list = true;
                break;
            case 'r':
                nameToRemove = optarg;
                remove = true;
                break;
            case '?':
                printf("Unknown option -%c\n", c);
                break;
            default:
                return -1;
        }
    }

    if(filepath == NULL) {
        printf("Filepath is a required argument\n");
        printf_usage(argv);
        return 0;
    }
    

    if(newfile){
        dbfd = create_db_file(filepath);
        if(dbfd == STATUS_ERROR) {
            printf("unable to create database file\n");
            return -1;
        }

        if(create_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
            printf("Failed to create db header");
            return -1;
        }
    }
    else {
            dbfd = open_db_file(filepath);
            if(dbfd == STATUS_ERROR) {
                printf("unable to open database file\n");
                return -1;
             }

            if(validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
                printf("Failed to validate database\n");
                return -1;
            }
    }

    if(read_empoyees(dbfd, dbhdr, &employees) != STATUS_SUCCESS) {
        printf("Failed to read employees\n");
        return 0;
    }

    if(addstring) {
        employees = realloc(employees, ++dbhdr->count*(sizeof(struct employee_t)));
        add_employee(dbhdr, employees, addstring);
    }

    if(removestring) {
    }

    if(list) {
        list_employees(dbhdr, employees);
    }

    output_file(dbfd, dbhdr, employees);

    return 0;
}
