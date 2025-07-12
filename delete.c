#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "row.h"
#include "page.h"
#include "table.h"
#include "btree.h"

int remove_bit(uint8_t *bitmap, int i);
int check_bit(uint8_t *bitmap, int i);

void delete_row(Table* table, int id) 
{
    uint32_t page_num;
    uint16_t row_num;
    if(!btree_find(&table->btree_meta, id, &page_num, &row_num)){
        printf("Row with ID %d not found.\n", id);
        return;
    }
    
    // Load page if not in memory
    Page* page = table_get_page(table, page_num);
    if(page == NULL || check_bit(page->bitmap, row_num) == 0){
        printf("Row with ID %d not found in memory.\n", id);
        return;
    }
    
    // Clear the row data
    memset(&page->rows[row_num], 0, sizeof(Row));

    if(remove_bit(page->bitmap, row_num) == -1){
        printf("Error removing bit from bitmap at index %d.\n", row_num);
        return;
    }
    
    page->num_rows--;
    
    // Write updated page back to disk
    table_write_page(table, page_num, page);
    
    // Delete from B-Tree index
    if (!btree_delete(&table->btree_meta, id)) {
        printf("Warning: Failed to delete from B-Tree index.\n");
    }
    
    printf("Row with ID %d deleted successfully.\n", id);
}
