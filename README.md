# id3v2lib-e

id3v2lib is a library written in C to read and edit id3 tags from mp3 files. It is focused on the ease of use. As it is a personal project, the software is provided "as is" without warranty of any kind.

The id3v2lib-e is a fork of the original id3v2lib adapted to be used in memory limited embedded devices, such as microcontrollers. All editing functions are removed, and it no longer caches the tag content in the memory, instead it only caches minimal meta data for fast link list traversing and read the actual content from filesystem upon request. This greatly reduces the memory requirement from around 3x the tag size (~1MB assuming 640x640 JPEG cover art) to < 1 KB.

It is licensed under BSD License. See LICENSE for details.

## What it can do?

id3v2lib-e can read id3 v2.3 and v2.4 tags from mp3 files. It can read a small subset of tags by default. Specifically, the most used tags:

* Title
* Album
* Artist
* Album Artist
* Comment
* Genre
* Year
* Track
* Disc Number
* Album Cover

However, it can be extended, in a very easy manner, to read other id3 tags. 

## Building id3v2lib-e

By default, cross-compiling is not configured in the Makefile. The easiest way of integrating this into your ARM, RISC-V or other processor based project is simply copy all files into your project.

## Usage

You only have to include the main header of the library:
```C
#include <id3v2lib.h>
	
int main(int argc, char* argv[])
{
	// Etc..
}
```
And when compiling, link against the static library:

	$ gcc -o example example.c -lid3v2

## Main functions

### File functions

This functions interacts directly with the file to edit. This functions are:

* `ID3v2_tag* load_tag(const char* filename)`

Note this function also saves the file pointer into the tag struct and can be accessed through tag->file.

### Tag functions

This functions interacts with the tags in the file. They are classified in three groups:

#### Getter functions

Retrieve information from a tag, they have the following name pattern:

* `tag_get_[frame]` where frame is the name of the desired field from which to obtain information. It can be one of the previously mentioned tags. 

## Examples

#### Load tags

```C
ID3v2_tag* tag = load_tag("file.mp3"); // Load the full tag from the file
if(tag == NULL)
{
	tag = new_tag();
}
	
// Load the fields from the tag
ID3v2_frame* artist_frame = tag_get_artist(tag->file, tag); // Get the full artist frame
// We need to parse the frame content to make readable
ID3v2_frame_text_content* artist_content = parse_text_frame_content(artist_frame); 
printf("ARTIST: %s\n", artist_content->data); // Show the artist info
	
ID3v2_frame* title_frame = tag_get_title(tag->file, tag);
ID3v2_frame_text_content* title_content = parse_text_frame_content(title_frame);
printf("TITLE: %s\n", title_content->data);

ID3v2_frame* cover_frame = tag_get_album_cover(tag);
ID3v2_frame_apic_content *cover_content = parse_apic_frame_content(tag->file, frame);
printf("Image type: %s, offset in original file: %d\n", cover_content->mime_type, cover_content->offset);

```

## Projects

If your project is using this library, let me know it and I will put it here.
	
## Copyright

Copyright (c) 2013 Lorenzo Ruiz. Copyright (c) 2020 Wenting Zhang. See LICENSE for details.
	
## Questions?

If you have any questions, please feel free to ask me. I will try to answer it ASAP.
