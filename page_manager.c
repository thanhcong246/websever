// page_manager.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h> 

#define BUFFER_SIZE 1024
#define SERVER_PORT 8080
#define SERVER_ADDRESS "127.0.0.1"

// Declare the function prototype
int is_file_blocked(const char *url);

void display_menu() {
    printf("\nPage Manager Menu:\n");
    printf("1. Add Page\n");
    printf("2. Edit Page\n");
    printf("3. Delete Page\n");
    printf("4. Block Page\n");
    printf("5. Unblock Page\n");
    printf("6. Exit\n");
}

void add_page() {
    char title[BUFFER_SIZE];
    char content[BUFFER_SIZE];

    printf("Enter the title of the page: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = '\0';  // Remove newline character

    printf("Enter the content of the page (HTML): ");
    fgets(content, sizeof(content), stdin);
    content[strcspn(content, "\n")] = '\0';  // Remove newline character

    // Create an HTML file with the given title and content
    char file_path[BUFFER_SIZE * 2];  // Increase buffer size
    snprintf(file_path, sizeof(file_path), "/home/connguyen/Desktop/HTTP-Server/%s.html", title);

    FILE *file = fopen(file_path, "w");
    if (file == NULL) {
        perror("Error creating the HTML file");
        return;
    }

    fprintf(file, "<html>\n<head>\n<title>%s</title>\n</head>\n<body>\n%s\n</body>\n</html>\n", title, content);

    fclose(file);
    printf("Page added successfully!\n");
}

// Edit page function
void edit_page() {
    char title[BUFFER_SIZE];
    char new_content[BUFFER_SIZE];

    printf("Enter the title of the page you want to edit: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = '\0';  // Remove newline character

    // Create the path to the HTML file
    char file_path[BUFFER_SIZE * 2];
    snprintf(file_path, sizeof(file_path), "/home/connguyen/Desktop/HTTP-Server/%s.html", title);

    // Check if the file exists
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Error opening the HTML file");
        return;
    }
    fclose(file);

    // Display the current content of the page
    printf("\nCurrent content of the page:\n");
    file = fopen(file_path, "r");
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }
    fclose(file);

    // Enter new content for the page
    printf("\nEnter the new content of the page (HTML): ");
    fgets(new_content, sizeof(new_content), stdin);
    new_content[strcspn(new_content, "\n")] = '\0';  // Remove newline character

    // Open the file for writing the new content
    file = fopen(file_path, "w");
    if (file == NULL) {
        perror("Error opening the HTML file for editing");
        return;
    }

    // Write the new content to the file
    fprintf(file, "%s", new_content);

    fclose(file);
    printf("Page content edited successfully!\n");
}


// Delete page function
void delete_page() {
    char title[BUFFER_SIZE];

    printf("Enter the title of the page you want to delete: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = '\0';  // Remove newline character

    // Create the path to the HTML file
    char file_path[BUFFER_SIZE * 2];
    snprintf(file_path, sizeof(file_path), "/home/connguyen/Desktop/HTTP-Server/%s.html", title);

    // Check if the file exists
    if (remove(file_path) == 0) {
        printf("Page deleted successfully!\n");
    } else {
        perror("Error deleting the page");
    }
}


// Function to block a page
void block_page() {
    char title[BUFFER_SIZE];

    printf("Enter the title of the page you want to block: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = '\0';  // Remove newline character

    // Create the path to the HTML file
    char file_path[BUFFER_SIZE * 2];
    snprintf(file_path, sizeof(file_path), "/home/connguyen/Desktop/HTTP-Server/%s.html", title);

    // Check if the file exists
    if (access(file_path, F_OK) == -1) {
        printf("Error: Page not found. Cannot block non-existing page.\n");
        return;
    }

    // Check if the file is already blocked
    if (is_file_blocked(file_path)) {
        printf("Error: This page is already blocked.\n");
        return;
    }

    // Open the blocklist file for writing
    FILE *blocklist = fopen("/home/connguyen/Desktop/HTTP-Server/blocklist.txt", "a");
    if (blocklist == NULL) {
        perror("Error opening the blocklist file");
        return;
    }

    // Write the full URL to the blocklist
    fprintf(blocklist, "%s.html\n", title);

    fclose(blocklist);
    printf("Page blocked successfully!\n");
}

// Check if a file is blocked
int is_file_blocked(const char *file_path) {
    // Open the blocklist file for reading
    FILE *blocklist = fopen("/home/connguyen/Desktop/HTTP-Server/blocklist.txt", "r");
    if (blocklist == NULL) {
        perror("Error opening the blocklist file");
        return 0;  // Assume not blocked in case of an error
    }

    char buffer[1024];  // Adjust buffer size as needed
    while (fgets(buffer, sizeof(buffer), blocklist) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline character
        if (strcmp(file_path, buffer) == 0) {
            fclose(blocklist);
            return 1;  // File is blocked
        }
    }

    fclose(blocklist);
    return 0;  // File is not blocked
}


// Function to unblock a page
void unblock_page() {
    char title[BUFFER_SIZE];

    printf("Enter the title of the page you want to unblock: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = '\0';  // Remove newline character

    // Create the full URL to the HTML file
    char full_url[BUFFER_SIZE * 2];
    snprintf(full_url, sizeof(full_url), "%s.html", title);

    // Open the blocklist file for reading
    FILE *blocklist = fopen("/home/connguyen/Desktop/HTTP-Server/blocklist.txt", "r");
    if (blocklist == NULL) {
        perror("Error opening the blocklist file");
        return;
    }

    // Create a temporary file to store the unblocked entries
    FILE *temp_blocklist = fopen("/home/connguyen/Desktop/HTTP-Server/temp_blocklist.txt", "w");
    if (temp_blocklist == NULL) {
        perror("Error opening the temporary blocklist file");
        fclose(blocklist);
        return;
    }

    char buffer[1024];  // Adjust buffer size as needed
    int found = 0;

    // Iterate through the blocklist file
    while (fgets(buffer, sizeof(buffer), blocklist) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline character

        // Compare the full URL in the blocklist with the user-input title
        if (strcmp(full_url, buffer) == 0) {
            found = 1;
        } else {
            // Write non-matching entries to the temporary blocklist
            fprintf(temp_blocklist, "%s\n", buffer);
        }
    }

    fclose(blocklist);
    fclose(temp_blocklist);

    // Remove the original blocklist file
    if (remove("/home/connguyen/Desktop/HTTP-Server/blocklist.txt") != 0) {
        perror("Error removing the blocklist file");
        return;
    }

    // Rename the temporary blocklist file to the original blocklist file
    if (rename("/home/connguyen/Desktop/HTTP-Server/temp_blocklist.txt", "/home/connguyen/Desktop/HTTP-Server/blocklist.txt") != 0) {
        perror("Error renaming the temporary blocklist file");
        return;
    }

    if (found) {
        printf("Page unblocked successfully!\n");
    } else {
        printf("Page not found in the blocklist.\n");
    }
}


// Main function
int main() {
    while (1) {
        display_menu();

        int choice;
        printf("Enter your choice: ");
        scanf("%d", &choice);

        // Clear input buffer
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                add_page();
                break;
            case 2:
                edit_page();
                break;
            case 3:
                delete_page();
                break;
            case 4:
                block_page();
                break;
            case 5:
                unblock_page();
                break;
            case 6:
                printf("Exiting Page Manager.\n");
                exit(EXIT_SUCCESS);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

