/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SeamansDataLoader.c
 * Author: tommy
 *
 * Created on October 1, 2020, 10:40 AM
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "DataLoader.h"


struct test_scenario* load_test_data(char* filename)
{
    struct test_scenario* ts = (struct test_scenario*)malloc(sizeof(struct test_scenario));
    FILE* fileRead = fopen(filename, "r");
    fscanf(fileRead, "%d", &ts->page_count);
    fscanf(fileRead, "%d", &ts->frame_count);
    fscanf(fileRead, "%d", &ts->refstr_len);
    for (int i=0; i<ts->refstr_len; i++)
    {
        fscanf(fileRead, "%d", &ts->refstr[i]);
    }
    fclose(fileRead);
    return ts;
}

