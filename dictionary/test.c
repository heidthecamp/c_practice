#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "dictionary.h"

#define TEST_SIZE 10000

double timespec_difference(struct timespec * before, struct timespec * after)
{
    double before_sec = ((double)before->tv_sec + (double)before->tv_nsec / 1000000000.0);
    double after_sec = ((double)after->tv_sec + (double)after->tv_nsec / 1000000000.0);
    return after_sec - before_sec;
}

int main(int argc, char ** argv)
{
    srand(time(0));

    if (!dictionary_init()) {
        dictionary_free();
    }

    const char * best_case[TEST_SIZE];
    const char * worst_case[TEST_SIZE];
    const char * random_case[TEST_SIZE];
    const char * miss_case[TEST_SIZE];

    const char * not_a_word = "abcdefghijklmnopqrstuvwxyz";
    size_t word_count = dictionary_word_count();
    for (size_t i = 0; i < TEST_SIZE; ++i) {
        // First n words
        best_case[i] = dictionary_get_word(i);

        // Last n words
        worst_case[i] = dictionary_get_word(word_count - i - 1);

        // Random n words
        size_t index = rand() % word_count;
        random_case[i] = dictionary_get_word(index);

        // Miss n words
        miss_case[i] = not_a_word;
    }

    const char * test_names[] = {
        "test_best", "test_worst", "test_random", "test_miss",
    };

    const char ** test_lists[] = {
        best_case, worst_case, random_case, miss_case,
    };

    size_t num_tests = sizeof(test_names) / sizeof(char *);

    printf("Testing a tree search\n");
    printf("Testing with %d words\n", TEST_SIZE);

    for (size_t test = 0; test < num_tests; ++test) {
        struct timespec before;
        clock_gettime(CLOCK_MONOTONIC, &before);

        const char ** test_list = test_lists[test];
        for (size_t i = 0; i < TEST_SIZE; ++i) {
            dictionary_lookup(test_list[i]);
        }

        struct timespec after;
        clock_gettime(CLOCK_MONOTONIC, &after);

        double difference = timespec_difference(&before, &after);
        printf("Test %s took %.3fs\n", test_names[test], difference);
    }

    dictionary_free();

    return 0;
}