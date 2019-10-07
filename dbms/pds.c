#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include "pds.h"
#include "bst.h" 

struct PDS_RepoInfo repo_handle;

int pds_open( char *repo_name, int rec_size ){ 
    if(repo_handle.repo_status == PDS_REPO_OPEN) repo_handle.repo_status = PDS_REPO_ALREADY_OPEN;
    strcpy(repo_handle.pds_name, repo_name);
    char temp_name1[30];
    char temp_name2[30];
    strcpy(temp_name1,repo_handle.pds_name);
    strcpy(temp_name2,repo_handle.pds_name); 
    repo_handle.pds_data_fp = (FILE*)fopen(strcat(temp_name1, ".dat"), "ab+ ");
    repo_handle.pds_ndx_fp = (FILE*)fopen(strcat(temp_name2, ".ndx"), "ab+ ");
    if(repo_handle.pds_data_fp != NULL && repo_handle.pds_ndx_fp != NULL){
        repo_handle.repo_status = PDS_REPO_OPEN;
        repo_handle.rec_size = rec_size;
        repo_handle.pds_bst = NULL;
        //Build BST and store in pds_bst by reading index entries
        pds_load_ndx();
        fclose(repo_handle.pds_ndx_fp);
        return PDS_SUCCESS;
    }
    else{
        printf("Error opening file\n");
        return PDS_FILE_ERROR;
    }
}

int pds_load_ndx(){
    FILE *fp = repo_handle.pds_ndx_fp;
    if(fp != NULL){
        fseek(fp, 0, SEEK_SET);
        while(!feof(fp)){
            struct PDS_NdxInfo* index = (struct PDS_NdxInfo*)malloc(sizeof(struct PDS_NdxInfo));
            fread(index, sizeof(struct PDS_NdxInfo), 1, fp);
            bst_add_node(&repo_handle.pds_bst, index->key, index);
        }
        return PDS_SUCCESS;
    }
    return PDS_FILE_ERROR;
}

int put_rec_by_key(int key, void *rec){
    if(repo_handle.repo_status != PDS_REPO_OPEN){
        printf("File is not open\n");
        return PDS_ADD_FAILED;
    }
    fseek(repo_handle.pds_data_fp, 0, SEEK_END);
    int offset = ftell(repo_handle.pds_data_fp);
    struct PDS_NdxInfo* index = (struct PDS_NdxInfo*)malloc(sizeof(struct PDS_NdxInfo));
    index->key = key;
    index->offset = offset;
    index->is_deleted = 0;
    int status = bst_add_node(&repo_handle.pds_bst, key, index);
    if(status == BST_NOT_FOUND){
        return PDS_ADD_FAILED;
    }
    fwrite(&key, sizeof(int), 1, repo_handle.pds_data_fp);
    fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
    return PDS_SUCCESS;         
}

int get_rec_by_ndx_key(int key, void *rec){
    if(repo_handle.repo_status != PDS_REPO_OPEN){
        return PDS_FILE_ERROR;
    }
    struct BST_Node *bst_node = bst_search(repo_handle.pds_bst, key);
    if(bst_node != NULL){
        struct PDS_NdxInfo *index = (struct PDS_NdxInfo*)malloc(sizeof(struct PDS_NdxInfo));
        index = bst_node->data;
        int temp_key;
        if(index != NULL && index->is_deleted == 0){
            int offset = index->offset;
            fseek(repo_handle.pds_data_fp, offset, SEEK_SET);
            fread(&temp_key, sizeof(int), 1, repo_handle.pds_data_fp);
            if(temp_key != key){
                return PDS_REC_NOT_FOUND;
            }
            fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
            return PDS_SUCCESS;
        }
    }
    return PDS_REC_NOT_FOUND;
}

int get_rec_by_non_ndx_key(
    void *key,  			/* The search key */
    void *rec,  			/* The output record */
    int (*matcher)(void *rec, void *key), /*Function pointer for matching*/
    int *io_count  		/* Count of the number of records read */){
        if(repo_handle.repo_status != PDS_REPO_OPEN){
            return PDS_FILE_ERROR;
        }
        int temp;
        (*io_count) = 0;
        FILE *fp = repo_handle.pds_data_fp;
        fseek(fp, 0, SEEK_SET);
        while(fread(&temp, sizeof(int), 1, fp)){
            fread(rec, repo_handle.rec_size, 1, fp);
            (*io_count)++;
            if(matcher(rec, key) == 0){ 
                struct BST_Node *bst_node = (struct BST_Node*)malloc(sizeof(struct BST_Node));
                bst_node = bst_search(repo_handle.pds_bst, temp);
                if(bst_node != NULL){
                    struct PDS_NdxInfo* index = bst_node->data;
                    if(index-> is_deleted != 1){
                        return PDS_SUCCESS;
                    }
                }
            }
        }
        return PDS_REC_NOT_FOUND;
}

int modify_rec_by_key( int key, void *rec ){
    if(repo_handle.repo_status != PDS_REPO_OPEN){
        return PDS_FILE_ERROR;
    }
    struct BST_Node *bst_node = (struct BST_Node*)malloc(sizeof(struct BST_Node));
    int temp = (int)key;
    bst_node = bst_search(repo_handle.pds_bst, temp);
    if(bst_node != NULL){
        struct PDS_NdxInfo *index;
        index = bst_node->data;
        if(index != NULL){
            int offset;
            offset = index->offset;
            fseek(repo_handle.pds_data_fp, offset, SEEK_SET);
            fwrite(&key, sizeof(int), 1, repo_handle.pds_data_fp);
            fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
            return PDS_SUCCESS;
        }                
    }
    return PDS_MODIFY_FAILED;
}

int delete_rec_by_ndx_key( int  key){
    if(repo_handle.repo_status != PDS_REPO_OPEN){
        return PDS_FILE_ERROR;
    }
    // struct BST_Node *bst_node = (struct BST_Node*)malloc(sizeof(struct BST_Node));
    // bst_node = bst_search(repo_handle.pds_bst, key);
    struct BST_Node *bst_node = bst_search(repo_handle.pds_bst, key);
    if(bst_node != NULL){
        struct PDS_NdxInfo *index;
        index = bst_node->data;
        if(index != NULL){
            if(index->is_deleted == 1){
                return PDS_DELETE_FAILED;
            }
            index->is_deleted = 1;
            return PDS_SUCCESS;
        }        
    }
    return PDS_DELETE_FAILED;

}

int preorder(struct BST_Node* node, FILE *fp){
    if(node == NULL){
        return 1;
    }
    struct PDS_NdxInfo *index = node->data;
    if(index->is_deleted == 1){
        preorder(node->left_child, fp);
        preorder(node->right_child, fp);
        return PDS_SUCCESS;
    }
    fwrite(node->data, sizeof(struct PDS_NdxInfo), 1, fp);
    preorder(node->left_child, fp);
    preorder(node->right_child, fp);
    return PDS_SUCCESS;
}

int pds_close(){
    if(repo_handle.repo_status != PDS_REPO_OPEN){
        return PDS_FILE_ERROR;
    }
    else{ 
        char temp_name[30];
        strcpy(temp_name,repo_handle.pds_name);
        strcat(temp_name, ".ndx");
        repo_handle.pds_ndx_fp = (FILE*)fopen(temp_name, "wb");
        fseek(repo_handle.pds_ndx_fp, 0 ,SEEK_SET);
        struct BST_Node* node = repo_handle.pds_bst;
        //Preorder traversal function
        int status = preorder(node, repo_handle.pds_ndx_fp);
        if(status){
            return PDS_NDX_SAVE_FAILED;
        }
        repo_handle.repo_status = PDS_REPO_CLOSED;
        fclose(repo_handle.pds_data_fp);
        fclose(repo_handle.pds_ndx_fp);
        bst_destroy(repo_handle.pds_bst);
        return PDS_SUCCESS;
    }
}

