#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NOTE: In this file, you will encounter a few standard library functions you may not have seen
 * yet, like strcpy and strcmp. We encourage you to read the documentation for these functions
 * to understand what they do. For example, to pull up the documentation for `strcpy`, run:
 *
 * $ man strcpy
 *
 * Good luck!
 */

struct airport {
    char name[4];
    struct airport *next;
};

// There are no bugs in this function as far as we know.
struct airport *create_from_array(const char **array, int length) {
    struct airport *first = NULL;

    for (int i = length - 1; i >= 0; --i) {
        struct airport *new = malloc(sizeof(struct airport));
        strncpy(new->name, array[i], 4);
        new->next = first;
        first = new;
    }

    return first;
}

void bad_free_path_1(struct airport *path) {
    struct airport *ptr = path;
    while (ptr != NULL) {
        // We need to free the original node that ptr points to
        // but we overwrite ptr AND read the original node in `ptr = ptr->next`
        // so to make sure we free it after reading it, we need a separate variable to track it
        struct airport *to_delete = ptr;
        ptr = ptr->next;
        free(to_delete);
    }
}

int bad_remove_element(struct airport **path, char *name) {
    struct airport *ptr = *path;
    // We need to store the previous node's address or else we won't be able to easily retrieve it.
    // Alternatively, you could "look ahead" by writing something like
    //   `strcmp(ptr->next->name, name) == 0` in a `while (ptr->next != NULL)` loop.
    struct airport *prev = NULL;
    while (ptr != NULL) {
        if (strcmp(ptr->name, name) == 0) {
            // The first element is a special case.
            // We need to (1) re-point `*path` and (2) skip the "->next" assignment.
            struct airport *node_to_delete = ptr;
            if (prev == NULL) {
                // First element
                *path = ptr->next;
            } else {
                prev->next = ptr->next;
            }
            // This free call was missing.
            free(node_to_delete);
            return 1;
        }
        prev = ptr;
        ptr = ptr->next;
    }

    return 0;
}

int bad_insert_element_at_pos(struct airport **path, char *name, int pos) {
    struct airport *ptr = *path;

    struct airport *new_airport = malloc(sizeof(struct airport));
    strcpy(new_airport->name, name);

    int i = 1;
    if (pos == 0) {
        // This following line needs to be first or else new_airport would point to itself
        new_airport->next = *path;
        *path = new_airport;
        return 1;
    }
    while (ptr != NULL) {
        if (i == pos) {
            // We should put new_airport right after *ptr
            new_airport->next = ptr->next;
            ptr->next = new_airport;
            return 1;
        }
        i++;
        ptr = ptr->next;
    }
    return 0;
}

void print_list(struct airport *curr) {
    if (curr && curr->next) {
        printf("This airport is %s, and the next airport is %s\n", curr->name, curr->next->name);
    } else if (curr) {
        printf("This airport is %s, and it is the last airport\n", curr->name);
    }
    if (curr) {
        print_list(curr->next);
    }
}

/* Write your tests below!
 *
 * As explained in the lab instructions, "assert" checks if the argument is true (nonzero).
 * If the argument is true, the function returns, and nothing special happens.
 * If the argument is false, the function prints an "Assertion failed" error and stops the program.
 * We use "assert" function calls to verify the outcome of a functionality being tested.
 *   By the way, not all tests need to have an assert function call.
 * Use our example for inspiration, and write a few more tests to expose more bugs!
 */

void test_free_path_1() {
    const char *path_array[] = {"SAN", "LAS", "DTW"};
    struct airport *path = create_from_array(path_array, 3);

    print_list(path);
    assert(strcmp(path->name, "SAN") == 0);

    bad_free_path_1(path);
}

// Here are some tests for the various cases of deletion
void test_delete_first_element() {
    const char *path_array[] = {"BOS", "DCA", "ATL"};
    struct airport *path = create_from_array(path_array, 3);

    assert(bad_remove_element(&path, "BOS") == 1);
    assert(strcmp(path->name, "DCA") == 0);
    assert(strcmp(path->next->name, "ATL") == 0);
    assert(path->next->next == NULL);

    bad_free_path_1(path);
}

void test_delete_only_element() {
    const char *path_array[] = {"SAN"};
    struct airport *path = create_from_array(path_array, 1);

    assert(bad_remove_element(&path, "SAN") == 1);
    assert(path == NULL);

    bad_free_path_1(path);
}

void test_delete_last_element() {
    const char *path_array[] = {"SAN", "SFO", "SEA"};
    struct airport *path = create_from_array(path_array, 3);

    assert(bad_remove_element(&path, "SEA") == 1);
    assert(strcmp(path->name, "SAN") == 0);
    assert(strcmp(path->next->name, "SFO") == 0);
    assert(path->next->next == NULL);

    bad_free_path_1(path);
}

void test_delete_middle_element() {
    const char *path_array[] = {"SAN", "SFO", "SEA"};
    struct airport *path = create_from_array(path_array, 3);

    assert(bad_remove_element(&path, "SFO") == 1);
    assert(strcmp(path->name, "SAN") == 0);
    assert(strcmp(path->next->name, "SEA") == 0);
    assert(path->next->next == NULL);

    bad_free_path_1(path);
}

void test_delete_missing_element() {
    const char *path_array[] = {"SAN", "LAX", "PHX", "DAL"};
    struct airport *path = create_from_array(path_array, 4);

    assert(bad_remove_element(&path, "MYF") == 0);

    bad_free_path_1(path);
}

// Here are some tests for the various cases of insertion
void test_insert_at_front() {
    const char *path_array[] = {"SAN", "SEE", "CRQ"};
    struct airport *path = create_from_array(path_array, 3);

    assert(bad_insert_element_at_pos(&path, "MYF", 0) == 1);

    assert(strcmp(path->name, "MYF") == 0);
    assert(strcmp(path->next->name, "SAN") == 0);
    assert(strcmp(path->next->next->name, "SEE") == 0);

    bad_free_path_1(path);
}

void test_insert_at_end() {
    const char *path_array[] = {"SAN", "SEE", "CRQ"};
    struct airport *path = create_from_array(path_array, 3);

    assert(bad_insert_element_at_pos(&path, "MYF", 3) == 1);

    assert(strcmp(path->name, "SAN") == 0);
    assert(strcmp(path->next->name, "SEE") == 0);
    assert(strcmp(path->next->next->name, "CRQ") == 0);
    assert(strcmp(path->next->next->next->name, "MYF") == 0);
    assert(path->next->next->next->next == NULL);

    bad_free_path_1(path);
}

void test_insert_from_empty() {
    struct airport *path = NULL;

    assert(bad_insert_element_at_pos(&path, "MYF", 0) == 1);
    assert(strcmp(path->name, "MYF") == 0);
    assert(path->next == NULL);

    bad_free_path_1(path);
}

void test_insert_in_middle() {
    const char *path_array[] = {"SAN", "SEE", "CRQ"};
    struct airport *path = create_from_array(path_array, 3);

    assert(bad_insert_element_at_pos(&path, "MYF", 1) == 1);

    assert(strcmp(path->name, "SAN") == 0);
    assert(strcmp(path->next->name, "MYF") == 0);
    assert(strcmp(path->next->next->name, "SEE") == 0);
    assert(strcmp(path->next->next->next->name, "CRQ") == 0);
    assert(path->next->next->next->next == NULL);

    bad_free_path_1(path);
}

// A lot of these tests have repetitive parts, such as building the list and verifying
// the contents in the list. You could build a function like `create_from_array`, such as
// `assert_matches_array`, to make the tests more succinct.

int main(void) {
    test_free_path_1();
    test_delete_first_element();
    test_delete_only_element();
    test_delete_last_element();
    test_delete_middle_element();
    test_delete_missing_element();
    printf("Delete tests passed\n");
    test_insert_at_front();
    test_insert_at_end();
    test_insert_from_empty();
    test_insert_in_middle();
    printf("Insert tests passed\n");
}
