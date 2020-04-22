/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "frame.h"
#include "utils.h"
#include "constants.h"

ID3v2_frame* parse_frame(FILE *file, int skip, int offset, int version)
{
    ID3v2_frame* frame = new_frame();
    
    // Parse frame header
    fseek(file, skip + offset, SEEK_SET);
    fread(frame->frame_id, ID3_FRAME_ID, 1, file);
    // Check if we are into padding
    if(memcmp(frame->frame_id, "\0\0\0\0", 4) == 0)
    {
        free(frame);
        return NULL;
    }

    char buf[ID3_FRAME_SIZE];
    fread(buf, ID3_FRAME_SIZE, 1, file);

    frame->size = btoi(buf, ID3_FRAME_SIZE, 0);
    if(version == ID3v24)
    {
        frame->size = syncint_decode(frame->size);
    }

    fread(frame->flags, ID3_FRAME_FLAGS, 1, file);

    // Load frame data
    frame->offset = ftell(file);
    
    return frame;
}

int get_frame_type(char* frame_id)
{
    switch(frame_id[0])
    {
        case 'T':
            return TEXT_FRAME;
        case 'C':
            return COMMENT_FRAME;
        case 'A':
            return APIC_FRAME;
        default:
            return INVALID_FRAME;
    }
}

ID3v2_frame_text_content* parse_text_frame_content(FILE *file, ID3v2_frame* frame)
{
    ID3v2_frame_text_content* content;
    if(frame == NULL)
    {
        return NULL;
    }

    fseek(file, frame->offset, SEEK_SET);
    content = new_text_content(frame->size);
    fread(&(content->encoding), ID3_FRAME_ENCODING, 1, file);
    content->size = frame->size - ID3_FRAME_ENCODING;
    fread(content->data, content->size, 1, file);

    return content;
}

ID3v2_frame_comment_content* parse_comment_frame_content(FILE *file, ID3v2_frame* frame)
{
    ID3v2_frame_comment_content *content;
    if(frame == NULL)
    {
        return NULL;
    }
    
    content = new_comment_content(frame->size);
    
    fseek(file, frame->offset, SEEK_SET);
    fread(&(content->text->encoding), ID3_FRAME_ENCODING, 1, file);
    content->text->size = frame->size - ID3_FRAME_ENCODING - ID3_FRAME_LANGUAGE - ID3_FRAME_SHORT_DESCRIPTION;
    fread(content->language, ID3_FRAME_LANGUAGE, 1, file);
    // Ignore short description
    fseek(file, frame->offset + ID3_FRAME_ENCODING + ID3_FRAME_LANGUAGE + 1, SEEK_SET);
    fread(content->text->data, content->text->size, 1, file);
    
    return content;
}

ID3v2_frame_apic_content* parse_apic_frame_content(FILE *file, ID3v2_frame* frame)
{
    ID3v2_frame_apic_content *content;

    if(frame == NULL)
    {
        return NULL;
    }
    
    content = new_apic_content();
    
    fseek(file, frame->offset, SEEK_SET);
    fread(&(content->encoding), ID3_FRAME_ENCODING, 1, file);

    char buf[30];
    int i = 0;
    char ch;
    do {
        fread(&ch, 1, 1, file);
        buf[i] = ch;
        i++;
    } while (ch != '\0' && i < 30);

    content->mime_type = malloc(i);
    strcpy(content->mime_type, buf);

    fread(&(content->picture_type), 1, 1, file);

    //content->description = &frame->data[++i];
    int seek = ftell(file);
    i = 0;
    if (content->encoding == 0x01 || content->encoding == 0x02) {
            /* skip UTF-16 description */
            char buf[2];
            do {
                fread(buf, 2, 1, file);
                i += 2;
            } while (buf[0] && buf[1]);
    }
    else {
            /* skip UTF-8 or Latin-1 description */
            char buf[1];
            do {
                fread(buf, 1, 1, file);
                i += 1;
            } while (buf[0] && buf[1]);
    }
    fseek(file, seek, SEEK_SET);
    content->description = malloc(i);
    fread(content->description, i, 1, file);

    seek = ftell(file);
    content->picture_size = frame->size - (seek - frame->offset);
    content->offset = ftell(file);

    return content;
}
