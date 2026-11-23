#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *VALID_GROUPS[] = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"};

int is_valid_group(const char *grp) {
    for (int i = 0; i < 8; i++) {
        if (strcmp(grp, VALID_GROUPS[i]) == 0) return 1;
    }
    return 0;
}

void test_safe_input(void) {
    char name[16];
    printf("Enter a long donor name (>15 chars): ");
    if (fgets(name, sizeof(name), stdin) != NULL) {
        /* Strip trailing newline or flush remaining characters from stdin */
        size_t len = strlen(name);
        if (len > 0 && name[len - 1] == '\n') {
            name[len - 1] = '\0';
        } else {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        printf("Safely stored truncated name: '%s'\n", name);
    }
}

void test_file_errors(void) {
    FILE *fp = fopen("non_existent_donors.txt", "r");
    if (fp == NULL) {
        perror("Expected error opening missing file");
    } else {
        fclose(fp);
    }
}

int main(int argc, char *argv[]) {
    /* 1. Command-line validation */
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage:\n  %s find <BLOOD_GROUP>\n  %s issue <BLOOD_GROUP> <UNITS>\n", argv[0], argv[0]);
        return 1;
    }

    if (!is_valid_group(argv[2])) {
        fprintf(stderr, "Error: '%s' is not a valid blood group.\n", argv[2]);
        return 1;
    }

    printf("Valid command: action='%s', group='%s'\n", argv[1], argv[2]);

    /* 2. File error reporting with perror */
    test_file_errors();

    /* 3. Safe string input */
    test_safe_input();

    return 0;
}
