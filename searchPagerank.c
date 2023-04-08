// Implementation of Simple Search Engine - Part 3 Assignment 
// COMP2521 - 05/02/2023 
// Author: Alexander Burns - z5118440
//
// Program overview: searchPagerank.c
// This program acts as a simple search engine, taking in search terms as 
// command-line arguments and finding pages with one or more matching search 
// terms. Once complete, pages are sorted in descending order of the number of 
// matching search terms and outputted to stdout. Pages with the same number 
// of matching search terms are sorted in descending order by their PageRank.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_OUTPUT_PAGES 30
#define MAX_LINE_LENGTH 1000
#define MAX_WORD_LENGTH 1000

struct Page {
    char *url;
    int matchCount;
    double pageRank;
};

int parseInvertedIndex(char *argv[], int argc, struct Page *pages);
int getPageRankData(struct Page *pages, int distinctUrlNum);
int comparePages(const void *a, const void *b);

int main(int argc, char **argv) {

    struct Page pages[MAX_WORD_LENGTH];

    int uniqueUrls = parseInvertedIndex(argv, argc, pages);
    if (uniqueUrls == -1) {
        fprintf(stderr, "Error opening invertedIndex.txt\n");
        for (int k = 0; k < uniqueUrls; k++) {
            free(pages[k].url);
        }
        return 1;
    }

    int status = getPageRankData(pages, uniqueUrls);
    if (status != 0) {
        fprintf(stderr, "Error opening pagerankList.txt\n");
        for (int k = 0; k < uniqueUrls; k++) {
            free(pages[k].url);
        }
        return 1;
    }
    
    // Use standard library function 'qsort' to sort pages in descending 
    // order of matchCount and PageRank:
    qsort(pages, uniqueUrls, sizeof(struct Page), comparePages);

    // Check if the value of uniqueUrls is greater than NUM_OUTPUT_PAGES. If it is,
    // only output top 30 elements of the sorted pages array. If less than 
    // NUM_OUTPUT_PAGES, loop through and print all elements.
    if (uniqueUrls > NUM_OUTPUT_PAGES) {
        for (int i = 0; i < NUM_OUTPUT_PAGES; i++) {
            printf("%s\n", pages[i].url);
        }
    } else {
        for (int i = 0; i < uniqueUrls; i++) {
        fprintf(stdout, "%s\n", pages[i].url);
        }
    }

    // Free the memory used for URLs
    for (int k = 0; k < uniqueUrls; k++) {
        free(pages[k].url);
    }

    return 0;
}

// Function description: parseInvertedIndex parses the inverted index file and 
// adds URLs with matched search terms to the pages array. If a given URL 
// already exists in the array, matchCount is incremented instead. 
// The function returns the number of distinct URLs.
int parseInvertedIndex(char *argv[], int argc, struct Page *pages) {
    int distinctUrlNum = 0;
    char line[MAX_LINE_LENGTH + 1];
    char searchTerm[MAX_WORD_LENGTH];

    for (int i = 1; i < argc; i++) {
        FILE* invertedIndexFile = fopen("invertedIndex.txt", "r");
        if (invertedIndexFile == NULL) {
            return -1;
        }
 
        while (fgets(line, MAX_LINE_LENGTH, invertedIndexFile) != NULL) {
            line[strcspn(line, "\n")] = 0;
            sscanf(line, "%s", searchTerm);
            if (strcmp(searchTerm, argv[i]) == 0) {
                char *token;
                token = strtok(line, " ");
                token = strtok(NULL, " ");
                
                while (token != NULL) {
                    int urlExists = 0;
                    int urlIndex = -1;
                    
                    for (int j = 0; j < distinctUrlNum; j++) {
                        if (strncmp(token, pages[j].url, strlen(pages[j].url)) == 0) {
                            urlExists = 1;
                            urlIndex = j;
                            break;
                        }
                    }

                    if (!urlExists) {
                        pages[distinctUrlNum].url = malloc(strlen(token) + 1);
                        strcpy(pages[distinctUrlNum].url, token);
                        pages[distinctUrlNum].matchCount = 1;
                        distinctUrlNum++;
                    } else {
                        pages[urlIndex].matchCount++;
                    }

                    token = strtok(NULL, " ");                    
                }
            }
        }
        fclose(invertedIndexFile);
    }
    
    return distinctUrlNum;
}

// Function description: This function retrieves the PageRank data from 
// pagerankList.txt and updates the matching pageRank field in struct Page. 
// Returns 0 if the function executes successfully, 1 if an error occurs while 
// opening the pagerankList.txt file
int getPageRankData(struct Page *pages, int distinctUrlNum) {
    char line[MAX_LINE_LENGTH + 1];
    char url[MAX_WORD_LENGTH];
    double pagerank;

    FILE* pagerankListFile = fopen("pagerankList.txt", "r");
        if (pagerankListFile == NULL) {
            return 1;
        }

    while (fgets(line, MAX_LINE_LENGTH, pagerankListFile) != NULL) {
            char *token = strtok(line, ",");
            strcpy(url, token);
            token = strtok(NULL, ",");
            token = strtok(NULL, ",");
            pagerank = atof(token);
            
            for (int i = 0; i < distinctUrlNum; i++) {
                if (strncmp(url, pages[i].url, strlen(url)) == 0) {
                    pages[i].pageRank = pagerank;
                    break;
                }
            }
        }   

    fclose(pagerankListFile);
    return 0;
}

// Function description: This function compares two pages based on matchCount 
// and pageRank, it is called and used for sorting with qsort in the main. 
// The last return statement is using a ternary operator to conditionally 
// return 1 or -1 based on the result of the comparison 
// (page2->pageRank - page1->pageRank) > 0. The structure of this function was
// taken from: https://www.geeksforgeeks.org/comparator-function-of-qsort-in-c/
int comparePages(const void *a, const void *b) {
    struct Page *page1 = (struct Page *) a;
    struct Page *page2 = (struct Page *) b;
    
    if (page1->matchCount != page2->matchCount) {
        return page2->matchCount - page1->matchCount;
    } else {
        return (page2->pageRank - page1->pageRank) > 0 ? 1 : -1;
    }
}
