#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int id;
    char name[32];
    char group[4];
    int age;
    char phone[16];
    char last_date[12];
} DonorRecord;

double get_time_ms(clock_t start, clock_t end) {
    return ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
}

long get_file_size(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return -1;
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fclose(fp);
    return sz;
}

void benchmark_size(int n) {
    DonorRecord *arr = malloc(n * sizeof(DonorRecord));
    for (int i = 0; i < n; i++) {
        arr[i].id = 10000 + i;
        snprintf(arr[i].name, 32, "Donor_%d", i);
        strcpy(arr[i].group, "O+");
        arr[i].age = 20 + (i % 40);
        strcpy(arr[i].phone, "9876543210");
        strcpy(arr[i].last_date, "2026-01-15");
    }

    /* 1. Text Write & Read */
    clock_t t0 = clock();
    FILE *ft = fopen("donors.txt", "w");
    for (int i = 0; i < n; i++) {
        fprintf(ft, "%d|%s|%s|%d|%s|%s\n", arr[i].id, arr[i].name,
                arr[i].group, arr[i].age, arr[i].phone, arr[i].last_date);
    }
    fclose(ft);
    clock_t t1 = clock();

    ft = fopen("donors.txt", "r");
    DonorRecord temp;
    while (fscanf(ft, "%d|%31[^|]|%3[^|]|%d|%15[^|]|%11s\n",
                  &temp.id, temp.name, temp.group, &temp.age, temp.phone, temp.last_date) == 6);
    fclose(ft);
    clock_t t2 = clock();

    /* 2. Binary Write & Read */
    clock_t b0 = clock();
    FILE *fb = fopen("donors.bin", "wb");
    fwrite(arr, sizeof(DonorRecord), n, fb);
    fclose(fb);
    clock_t b1 = clock();

    fb = fopen("donors.bin", "rb");
    fread(arr, sizeof(DonorRecord), n, fb);
    fclose(fb);
    clock_t b2 = clock();

    printf("N = %-7d | Text: %6ld B, W: %6.2f ms, R: %6.2f ms | Bin: %7ld B, W: %6.2f ms, R: %6.2f ms\n",
           n, get_file_size("donors.txt"), get_time_ms(t0, t1), get_time_ms(t1, t2),
           get_file_size("donors.bin"), get_time_ms(b0, b1), get_time_ms(b1, b2));

    /* B3.3: Update 50,000th record when N == 100,000 */
    if (n == 100000) {
        /* Binary in-place update */
        clock_t u_bin0 = clock();
        fb = fopen("donors.bin", "r+b");
        fseek(fb, 49999L * sizeof(DonorRecord), SEEK_SET);
        fread(&temp, sizeof(DonorRecord), 1, fb);
        temp.age = 99;
        fseek(fb, -(long)sizeof(DonorRecord), SEEK_CUR);
        fwrite(&temp, sizeof(DonorRecord), 1, fb);
        fclose(fb);
        clock_t u_bin1 = clock();

        /* Text full rewrite update */
        clock_t u_txt0 = clock();
        ft = fopen("donors.txt", "r");
        int count = 0;
        while (fscanf(ft, "%d|%31[^|]|%3[^|]|%d|%15[^|]|%11s\n",
                      &arr[count].id, arr[count].name, arr[count].group,
                      &arr[count].age, arr[count].phone, arr[count].last_date) == 6) {
            count++;
        }
        fclose(ft);
        arr[49999].age = 99;
        ft = fopen("donors.txt", "w");
        for (int i = 0; i < count; i++) {
            fprintf(ft, "%d|%s|%s|%d|%s|%s\n", arr[i].id, arr[i].name,
                    arr[i].group, arr[i].age, arr[i].phone, arr[i].last_date);
        }
        fclose(ft);
        clock_t u_txt1 = clock();

        printf("\nUpdate 50,000th record -> Binary (fseek): %.4f ms | Text (rewrite): %.2f ms\n",
               get_time_ms(u_bin0, u_bin1), get_time_ms(u_txt0, u_txt1));
    }

    free(arr);
}

int main(void) {
    int sizes[] = {1000, 10000, 100000};
    for (int i = 0; i < 3; i++) {
        benchmark_size(sizes[i]);
    }
    return 0;
}
