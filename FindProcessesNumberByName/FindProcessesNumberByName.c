#define _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>

#define NAME_LENGTH 100

size_t FindProcessesNumberByName(const char* to_find, const char* to_scan) {
    struct dirent** name_list;
    size_t number = 0;
    int res = scandir(to_scan, &name_list, NULL, alphasort);
    if (res < 0) return number;
    res--;
    while (res >= 0) {
        if (strcmp(name_list[res]->d_name, to_find) == 0) ++number;
        if (isdigit(name_list[res]->d_name[0])) {
            char procname[PATH_MAX];
            char name[NAME_LENGTH + 1];
            char buf[BUFSIZ * 2];
            snprintf(procname, sizeof(procname), "/proc/%u/status", atoi(name_list[res]->d_name));
            FILE* file = fopen(procname, "r");
            if (file == NULL) return 0;
            int res = fread(buf, sizeof(char), BUFSIZ * 2 - 1, file);
            buf[res++] = '\0';
            fclose(file);
            char* name_loc = strstr(buf, "Name:");
            if (name_loc == NULL) return 0;
            sscanf(name_loc, "Name:%s \n", name);
            if (strcmp(to_find, name) == 0) ++number;
        }
        free(name_list[res]);
        res--;
    }
    free(name_list);
    return number;
}

int main(int argc, char** argv) {
    if (argc != 2) { perror("One argument is required!"); }
    else { printf("%zu\n", FindProcessesNumberByName(argv[1], "/proc/")); }
    return 0;
}