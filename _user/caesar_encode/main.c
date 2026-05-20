/*
 * Phoenix-RTOS
 *
 * Caesar Cipher Encode Server
 *
 * Server that encodes text using Caesar cipher with shift=5
 *
 * Copyright 2024 Phoenix Systems
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

/* Message handling */
#include <sys/msg.h>

/* create_dev() */
#include <posix/utils.h>

#define BUFFER_SIZE 4096
#define CAESAR_SHIFT 5

/* Buffer to store encoded text */
static char encoded_buffer[BUFFER_SIZE];
static size_t encoded_length = 0;


/**
 * Encode a single character using Caesar cipher with shift=5
 *
 * @param c Character to encode
 * @return Encoded character
 */
static char caesar_encode_char(char c)
{
	if (c >= 'A' && c <= 'Z') {
		/* Uppercase letter: shift forward by 5, wrap around */
		return 'A' + ((c - 'A' + CAESAR_SHIFT) % 26);
	}
	else if (c >= 'a' && c <= 'z') {
		/* Lowercase letter: shift forward by 5, wrap around */
		return 'a' + ((c - 'a' + CAESAR_SHIFT) % 26);
	}
	else {
		/* Non-alphabetic character: no change */
		return c;
	}
}


/**
 * Encode a buffer of text using Caesar cipher
 *
 * @param input Input text buffer
 * @param output Output buffer for encoded text
 * @param len Length of input text
 */
static void caesar_encode_buffer(const char *input, char *output, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		output[i] = caesar_encode_char(input[i]);
	}
}


static int server_handleOpen(oid_t *oid)
{
	/* Allow open() on our interface */
	(void)oid;

	/* Clear the buffer on open */
	encoded_length = 0;
	memset(encoded_buffer, 0, BUFFER_SIZE);

	return 0;
}


static int server_handleClose(oid_t *oid)
{
	/* Allow close() on our interface */
	(void)oid;
	return 0;
}


static ssize_t server_handleRead(oid_t *oid, void *data, size_t len, off_t offset)
{
	/* Return encoded text to the user */
	(void)oid;

	if (offset < 0) {
		return -EINVAL;
	}

	if ((size_t)offset >= encoded_length) {
		/* Reading past the end of encoded data */
		return 0;
	}

	/* Calculate how much data we can return */
	size_t available = encoded_length - (size_t)offset;
	size_t to_read = (len < available) ? len : available;

	/* Copy encoded data to user buffer */
	memcpy(data, encoded_buffer + offset, to_read);

	return (ssize_t)to_read;
}


static ssize_t server_handleWrite(oid_t *oid, const void *data, size_t len, off_t offset)
{
	/* Receive plaintext from user and encode it */
	(void)oid;
	(void)offset;

	if (len == 0) {
		return 0;
	}

	if (len > BUFFER_SIZE) {
		/* Input too large for our buffer */
		return -ENOMEM;
	}

	/* Encode the input data */
	caesar_encode_buffer((const char *)data, encoded_buffer, len);
	encoded_length = len;

	printf("caesar_encode: Encoded %zu bytes\n", len);

	return (ssize_t)len;
}


__attribute__((noreturn)) static void server_msgLoop(oid_t *oid)
{
	msg_t msg;
	msg_rid_t rid;

	for (;;) {
		/* Receive the next message */
		int err = msgRecv(oid->port, &msg, &rid);
		if (err < 0) {
			if (err == -EINTR) {
				/* Interrupted by signal, try again */
				continue;
			}
			else {
				/* Fatal error */
				fprintf(stderr, "caesar_encode: msgRecv returned %d (%s)\n",
					err, strerror(err));
				exit(EXIT_FAILURE);
			}
		}

		/* Process the message */
		switch (msg.type) {
			case mtOpen:
				msg.o.io.err = server_handleOpen(&msg.i.openclose.oid);
				break;

			case mtClose:
				msg.o.io.err = server_handleClose(&msg.i.openclose.oid);
				break;

			case mtRead:
				msg.o.io.err = server_handleRead(&msg.i.io.oid,
					msg.o.data, msg.o.size, msg.i.io.offs);
				break;

			case mtWrite:
				msg.o.io.err = server_handleWrite(&msg.i.io.oid,
					msg.i.data, msg.i.size, msg.i.io.offs);
				break;

			default:
				/* Unsupported message type */
				msg.o.io.err = -ENOSYS;
				break;
		}

		/* Respond to the message */
		msgRespond(oid->port, &msg, rid);
	}
}


int main(void)
{
	oid_t oid;

	/* Initialize */
	oid.id = 0;
	encoded_length = 0;
	memset(encoded_buffer, 0, BUFFER_SIZE);

	/* Create the port */
	if (portCreate(&oid.port) < 0) {
		fprintf(stderr, "caesar_encode: portCreate failed\n");
		return EXIT_FAILURE;
	}

	/* Register device in /dev */
	if (create_dev(&oid, "caesar_encode") < 0) {
		fprintf(stderr, "caesar_encode: create_dev failed\n");
		return EXIT_FAILURE;
	}

	printf("caesar_encode: Server started at /dev/caesar_encode\n");

	/* Start message loop */
	server_msgLoop(&oid);

	/* Never reached */
	return EXIT_FAILURE;
}
