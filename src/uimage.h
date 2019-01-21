#ifndef UIMAGE_H
#define UIMAGE_H

struct uimage_header;

void *process_uimage_header(struct uimage_header *header,
			    void **exec_addr, unsigned int data_size);

#endif
