#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int id;
    char group[4];
    int days_since_last_donation;
} Donor;

typedef struct {
    Donor *data;
    int size;
    int capacity;
} DonorList;

int add_donor(DonorList *list, int id, const char *group, int days) {
    if (list->size == list->capacity) {
        int new_cap = (list->capacity == 0) ? 16 : list->capacity * 2;
        Donor *p = realloc(list->data, (size_t)new_cap * sizeof(Donor));
        if (!p) return 0;
        list->data = p;
        list->capacity = new_cap;
    }
    list->data[list->size].id = id;
    strncpy(list->data[list->size].group, group, 3);
    list->data[list->size].group[3] = '\0';
    list->data[list->size].days_since_last_donation = days;
    list->size++;
    return 1;
}

int main(void) {
    const int TOTAL_DONORS = 500000; /* 100x scale across 25 hospitals */
    const char *groups[] = {"O+", "O-", "A+", "A-", "B+", "B-", "AB+", "AB-"};

    /* Create sample district data file */
    FILE *fp = fopen("district_donors.txt", "w");
    if (!fp) { perror("fopen write"); return 1; }
    for (int i = 0; i < TOTAL_DONORS; i++) {
        fprintf(fp, "%d %s %d\n", 100000 + i, groups[i % 8], (i * 7) % 150);
    }
    fclose(fp);

    /* Measure load + filter report */
    clock_t start = clock();
    fp = fopen("district_donors.txt", "r");
    if (!fp) { perror("fopen read"); return 1; }

    DonorList list = {NULL, 0, 0};
    int id, days;
    char grp[4];
    while (fscanf(fp, "%d %3s %d", &id, grp, &days) == 3) {
        if (!add_donor(&list, id, grp, days)) {
            fprintf(stderr, "Memory allocation failed!\n");
            free(list.data);
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);

    /* Generate nightly eligibility report (>= 83 days now -> >= 90 days within next 7 days) */
    FILE *rep = fopen("nightly_eligible_report.txt", "w");
    int eligible_count = 0;
    for (int i = 0; i < list.size; i++) {
        if (list.data[i].days_since_last_donation >= 83) {
            fprintf(rep, "%d,%s,%d\n", list.data[i].id, list.data[i].group,
                    list.data[i].days_since_last_donation);
            eligible_count++;
        }
    }
    fclose(rep);
    clock_t end = clock();

    double elapsed_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
    size_t allocated_bytes = (size_t)list.capacity * sizeof(Donor);
    size_t used_bytes = (size_t)list.size * sizeof(Donor);

    printf("Loaded Records   : %d\n", list.size);
    printf("Eligible Donors  : %d\n", eligible_count);
    printf("Heap Allocated   : %.2f MB (Capacity: %d)\n", allocated_bytes / (1024.0 * 1024.0), list.capacity);
    printf("Heap Actually Used: %.2f MB\n", used_bytes / (1024.0 * 1024.0));
    printf("Total Execution  : %.2f ms\n", elapsed_ms);

    free(list.data);
    return 0;
}
