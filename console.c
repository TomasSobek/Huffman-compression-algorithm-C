#include "console.h"
#include "sockets.h"
#include "compression/compression.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 256

static char *dynamic_fgets(void) {
    char *buffer = NULL;
    size_t size = 0;
    getline(&buffer, &size, stdin);

    if (buffer) {
        size_t length = strlen(buffer);
        if (length > 0 && buffer[length - 1] == '\n') {
            buffer[length - 1] = '\0'; // Remove newline character
        }
    }

    return buffer;
}

static int file_exists(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

static char *get_valid_file_path(void) {
    char *path;
    do {
        printf("Enter file path: ");
        path = dynamic_fgets();

        if (!path) {
            printf("Error reading input.\n");
            continue;
        }

        if (!file_exists(path)) {
            printf("File not found: %s\n", path);
            free(path);
            path = NULL;
        }
    } while (!path);
    return path;
}

static char* replace_filename(const char *file_path, const char *new_filename) {
    // Find the last '/' or '\' in the path
    char *last_slash = strrchr(file_path, '/');
    if (last_slash == NULL) {
        last_slash = strrchr(file_path, '\\');
    }

    char *modified_path = NULL;

    if (last_slash != NULL) {
        // Calculate the position of the last slash in the string
        size_t last_slash_position = last_slash - file_path;

        // Allocate memory for the modified path
        modified_path = (char*)malloc(BUFFER_SIZE);

        if (modified_path != NULL) {
            // Copy the original path into the modified path
            strcpy(modified_path, file_path);

            // Copy the new filename into the path after the last slash
            strcpy(modified_path + last_slash_position + 1, new_filename);
        }
    }
    else {
        // If no slash or backslash is found, return a copy of the new filename
        modified_path = strdup(new_filename);
    }

    return modified_path;
}

void console_main(void) {
    while (1) {
        char mode;
        printf("Choose a mode:\n");
        printf("'r', Receiver Mode\n");
        printf("'s', Sender Mode\n");
        printf("'h', Huffman Mode\n");
        // printf("'c', Commands executing mode\n");
        printf("'e', Exit\n");
        printf("Enter your choice: ");

        scanf(" %c", &mode);
        getchar();

        if (mode == 'r') {
            receiver_mode();
        } else if (mode == 's') {
            sender_mode();
        } else if (mode == 'h') {
            huffman_mode();
        } else if (mode == 'e') {
            printf("Exiting the program.\n");
            break;
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }
 }

// basic console modes
void receiver_mode(void) {
    unsigned int port;
    char *fp;
    // char* file_path = "/home/sobek3/client2_files/receiv.txt";
    printf("Select a port: ");
    scanf("%d", &port);
    getchar();

    if (port < 1000 || port > 65535) {
        printf("Not a valid port (1000 < port < 65535). Your option: %d\n", port);
        return;
    }

    fp = get_valid_file_path();

    receive_file_tcp(port, fp);
    char *decompressed_file_path = replace_filename(fp, "decompressed.f");
    char file_name_buffer[BUFFER_SIZE];
    strcpy(file_name_buffer, decompressed_file_path);
    huffman_decode(fp, decompressed_file_path);
    free(fp);
    free(decompressed_file_path);
}

void sender_mode(void) {
    unsigned int port;
    char *fp;
    //char* file_path = "/home/sobek3/client1_files/send.txt";
    printf("Select a port: ");
    scanf(" %d", &port);
    getchar();

    if (port < 1000 || port > 65535) {
        printf("Not a valid port (1000, port, 65535). Your option: %d\n", port);
        return;
    }

    fp = get_valid_file_path();

    char *compressed_file_path = replace_filename(fp, "compressed.z");
    char file_name_buffer[BUFFER_SIZE];
    strcpy(file_name_buffer, compressed_file_path);
    huffman_encode(fp, file_name_buffer);
    send_file_tcp("localhost", port, file_name_buffer);
    free(fp);
    free(compressed_file_path);
}

void huffman_mode(void) {
    printf("You are entering Huffman compression mode.\n");
    char *fp;
    fp = get_valid_file_path();
    char *compressed_file_path = replace_filename(fp, "hf_compressed.z");
    char *decompressed_file_path = replace_filename(fp, "hf_decompressed.f");
    huffman_encode(fp,
                   compressed_file_path);
    huffman_decode(compressed_file_path,
                   decompressed_file_path);

    // cleanup
    free(fp);
    free(compressed_file_path);
    free(decompressed_file_path);
}

// linux commands functions
static void execute_command(const char *command);
static void ls_command(void);
static void mkdir_command(void);
static void rmdir_command(void);
static void rmfile_command(const char* file_path) {
    if (remove(file_path) == 0) {
        printf("File was removed successfully.\n");
    } else {
        perror("Error removing file.\n");
    }
}