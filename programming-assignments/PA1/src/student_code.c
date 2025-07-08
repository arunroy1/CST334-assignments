#include "student_code.h"
#include <string.h>
#include <stdlib.h>

/**
 * Initializes a String from a C-style string.
 */
String String__init(char* input_c_str) {
    String new_str;

    // Get size of the input string since we use it a few times throughout
    size_t length_of_string = strlen(input_c_str);

    // Set metadata associated with the string
    new_str.length = length_of_string;
    new_str.capacity = length_of_string + 1; // +1 for the null terminator

    // Copy over the data itself
    new_str.data = (char*)malloc(new_str.capacity);
    if (new_str.data) {
        memcpy(new_str.data, input_c_str, new_str.capacity);
    }

    // Return the string
    return new_str;
}

/**
 * Frees the memory used by the String's data.
 */
void String__delete(String* str) {
    free(str->data);
    str->data = NULL; // Prevent dangling pointer
    str->length = 0;
    str->capacity = 0;
}

/**
 * Returns the length of the string.
 */
size_t String__length(const String* str) {
    return str->length;
}

/**
 * Ensures the string has at least 'new_capacity' bytes of storage.
 * If the current capacity is already sufficient, this function does nothing.
 */
void String__reserve(String* str, size_t new_capacity) {
    // Only reallocate if the new capacity is greater than the current one.
    if (new_capacity > str->capacity) {
        char* new_data = (char*)realloc(str->data, new_capacity);
        if (new_data) {
            str->data = new_data;
            str->capacity = new_capacity;
        }
    }
}

/**
 * Resizes the string to a new length.
 * If new_size is smaller, the string is truncated.
 * If new_size is larger, the string is extended and padded with character 'c'.
 */
void String__resize(String* str, size_t new_size, const char c) {
    // If shrinking
    if (new_size < str->length) {
        str->length = new_size;
        str->data[str->length] = '\0';
    } 
    // If growing
    else if (new_size > str->length) {
        // Ensure there is enough capacity for the new size and null terminator.
        String__reserve(str, new_size + 1);

        // Fill the new space with the specified character 'c'.
        memset(str->data + str->length, c, new_size - str->length);
        
        str->length = new_size;
        str->data[str->length] = '\0';
    }
}

/**
 * Clears the string, making its length 0.
 * Capacity is not changed.
 */
void String__clear(String* str) {
    str->length = 0;
    if (str->capacity > 0) {
        str->data[0] = '\0';
    }
}

/**
 * Checks if the string is empty (length is 0).
 */
bool String__empty(String* str) {
    return str->length == 0;
}

/**
 * Returns the character at a specific index.
 * Returns null terminator '\0' if the index is out of bounds.
 */
char String__at(String* str, size_t index) {
    if (index >= str->length) {
        return '\0'; // Out of bounds
    }
    return str->data[index];
}

/**
 * Returns the last character of the string.
 * Returns null terminator '\0' if the string is empty.
 */
char String__back(String* str) {
    if (String__empty(str)) {
        return '\0';
    }
    return str->data[str->length - 1];
}

/**
 * Returns the first character of the string.
 * Returns null terminator '\0' if the string is empty.
 */
char String__front(String* str) {
    if (String__empty(str)) {
        return '\0';
    }
    return str->data[0];
}

/**
 * Appends one string to another.
 */
void String__append(String* str, const String* str_to_add) {
    size_t new_length = str->length + str_to_add->length;

    // Reserve memory if needed
    String__reserve(str, new_length + 1);

    // Copy the data from the second string to the end of the first
    memcpy(str->data + str->length, str_to_add->data, str_to_add->length);
    
    str->length = new_length;
    str->data[str->length] = '\0';
}

/**
 * Appends a single character to the end of the string.
 */
void String__push_back(String* str, const char char_to_add) {
    size_t new_length = str->length + 1;

    // If current capacity is not enough, double it or use required length
    if (new_length >= str->capacity) {
        String__reserve(str, str->capacity * 2);
    }

    str->data[str->length] = char_to_add;
    str->length = new_length;
    str->data[str->length] = '\0';
}

/**
 * Removes the last character from the string.
 */
void String__pop_back(String* str) {
    if (!String__empty(str)) {
        str->length--;
        str->data[str->length] = '\0';
    }
}

/**
 * Inserts a string into another at a specific index.
 */
void String__insert(String* str, const String* str_to_insert, size_t index) {
    if (index > str->length) {
        index = str->length; // Clamp index to the end of the string
    }

    size_t new_length = str->length + str_to_insert->length;
    String__reserve(str, new_length + 1);

    // Shift the existing content to the right to make space for the new string
    // Use memmove because source and destination buffers overlap
    memmove(str->data + index + str_to_insert->length, str->data + index, str->length - index);

    // Copy the new string into the created gap
    memcpy(str->data + index, str_to_insert->data, str_to_insert->length);

    str->length = new_length;
    str->data[str->length] = '\0';
}

/**
 * Erases a portion of the string.
 */
void String__erase(String* str, size_t pos, size_t len) {
    if (pos >= str->length) {
        return; // Nothing to erase
    }

    // Clamp length to not go past the end of the string
    if (pos + len > str->length) {
        len = str->length - pos;
    }

    // Shift content from after the erased section to the left
    // Use memmove for safety, although buffers don't overlap here
    memmove(str->data + pos, str->data + pos + len, str->length - pos - len);

    str->length -= len;
    str->data[str->length] = '\0';
}

/**
 * Replaces a portion of the string with another string.
 */
void String__replace(String* str, size_t pos, size_t len, const String* str_to_replace_with) {
    if (pos > str->length) {
        pos = str->length; // Clamp position
    }
    if (pos + len > str->length) {
        len = str->length - pos; // Clamp length
    }

    size_t new_length = str->length - len + str_to_replace_with->length;
    String__reserve(str, new_length + 1);

    // Shift the part of the string that comes after the replaced section
    memmove(str->data + pos + str_to_replace_with->length, str->data + pos + len, str->length - pos - len);

    // Copy the replacement string into the gap
    memcpy(str->data + pos, str_to_replace_with->data, str_to_replace_with->length);
    
    str->length = new_length;
    str->data[str->length] = '\0';
}

/**
 * Swaps the contents of two strings.
 * This is an efficient operation that just swaps the internal struct members.
 */
void String__swap(String* str1, String* str2) {
    String temp = *str1;
    *str1 = *str2;
    *str2 = temp;
}