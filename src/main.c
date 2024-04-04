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
    printf("\t -a - add employee to the database\n");
    printf("\t -l - list all the empoyees\n");
    printf("\t -r - removes empoyees with the same name\n");
}

int main(int argc, char * argv[]) {

    bool newfile = false;
    char * filepath = NULL;
    char * addstring = NULL;
    char * removestring = NULL;
    bool list = false;
    int c;

    int dbfd = -1;
    struct dbheader_t *dbhdr = NULL;
    struct employee_t *employees = NULL;

    while((c = getopt(argc, argv, "nf:a:r:l")) != -1) {
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
                removestring = optarg;
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

    if(addstring != NULL && removestring != NULL) {
        printf("can not add and remove the employees at the same time\n");
        printf_usage(argv);
        return -1;
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
        if(remove_employees(dbhdr, &employees, removestring) != STATUS_SUCCESS) {
            printf("Failed to remove employees\n");
            return -1;
        }
    }

    if(list) {
        list_employees(dbhdr, employees);
    }

    output_file(dbfd, dbhdr, employees);

    return 0;
}
