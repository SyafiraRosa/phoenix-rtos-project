/*
 * Phoenix-RTOS
 *
 * Caesar Cipher Encoder Server
 *
 * This server encodes plain text using the Caesar cipher with a shift of 5.
 * Created by Syafira Rosa
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

/* Phoenix-RTOS messaging headers */
#include <sys/msg.h>

/* For create_dev() */
#include <posix/utils.h>

#define BUF_SIZE 4096
#define CIPHER_SHIFT 5

/* Internal storage for the cipher-text */
static char storage_buf[BUF_SIZE];
static size_t storage_len = 0;


/**
 * Apply Caesar cipher shift forward to a single character.
 */
static char shift_char_forward(char character)
{
	if (character >= 'A' && character <= 'Z') {
		/* Upper case shifting with alphabetical wrap-around */
		return 'A' + ((character - 'A' + CIPHER_SHIFT) % 26);
	}
	else if (character >= 'a' && character <= 'z') {
		/* Lower case shifting with alphabetical wrap-around */
		return 'a' + ((character - 'a' + CIPHER_SHIFT) % 26);
	}
	else {
		/* Do not modify numbers, spaces, and punctuation symbols */
		return character;
	}
}


/**
 * Process a whole stream of characters by applying the shift.
 */
static void process_caesar_encode(const char *source, char *destination, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		destination[i] = shift_char_forward(source[i]);
	}
}


static int handle_device_open(oid_t *device_oid)
{
	(void)device_oid;

	/* Reset buffers and counters when device is opened */
	storage_len = 0;
	memset(storage_buf, 0, BUF_SIZE);

	return 0;
}


static int handle_device_close(oid_t *device_oid)
{
	(void)device_oid;
	return 0;
}


static ssize_t handle_device_read(oid_t *device_oid, void *user_buffer, size_t requested_len, off_t read_offset)
{
	(void)device_oid;

	if (read_offset < 0) {
		return -EINVAL;
	}

	if ((size_t)read_offset >= storage_len) {
		/* No more data to read */
		return 0;
	}

	size_t remaining_bytes = storage_len - (size_t)read_offset;
	size_t bytes_to_copy = (requested_len < remaining_bytes) ? requested_len : remaining_bytes;

	/* Copy processed cipher-text to the requesting user space */
	memcpy(user_buffer, storage_buf + read_offset, bytes_to_copy);

	return (ssize_t)bytes_to_copy;
}


static ssize_t handle_device_write(oid_t *device_oid, const void *user_data, size_t data_len, off_t write_offset)
{
	(void)device_oid;
	(void)write_offset;

	if (data_len == 0) {
		return 0;
	}

	if (data_len > BUF_SIZE) {
		/* Limit inputs to prevent buffer overflows */
		return -ENOMEM;
	}

	/* Perform encryption and update buffer */
	process_caesar_encode((const char *)user_data, storage_buf, data_len);
	storage_len = data_len;

	printf("caesar_encode server: Successfully encrypted %zu bytes\n", data_len);

	return (ssize_t)data_len;
}


__attribute__((noreturn)) static void run_server_message_loop(oid_t *server_oid)
{
	msg_t message;
	msg_rid_t request_id;

	for (;;) {
		int status = msgRecv(server_oid->port, &message, &request_id);
		if (status < 0) {
			if (status == -EINTR) {
				/* Retry if interrupted by signal handler */
				continue;
			}
			else {
				/* Shutdown server on critical IPC failures */
				fprintf(stderr, "caesar_encode: Critical error in msgRecv: %d (%s)\n",
					status, strerror(status));
				exit(EXIT_FAILURE);
			}
		}

		/* Message processing dispatcher */
		switch (message.type) {
			case mtOpen:
				message.o.io.err = handle_device_open(&message.i.openclose.oid);
				break;

			case mtClose:
				message.o.io.err = handle_device_close(&message.i.openclose.oid);
				break;

			case mtRead:
				message.o.io.err = handle_device_read(&message.i.io.oid,
					message.o.data, message.o.size, message.i.io.offs);
				break;

			case mtWrite:
				message.o.io.err = handle_device_write(&message.i.io.oid,
					message.i.data, message.i.size, message.i.io.offs);
				break;

			default:
				message.o.io.err = -ENOSYS;
				break;
		}

		/* Dispatch final response back to client */
		msgRespond(server_oid->port, &message, request_id);
	}
}


int main(void)
{
	oid_t server_oid;

	/* Setup system structure */
	server_oid.id = 0;
	storage_len = 0;
	memset(storage_buf, 0, BUF_SIZE);

	/* Initialize IPC communications port */
	if (portCreate(&server_oid.port) < 0) {
		fprintf(stderr, "caesar_encode: Failed to create IPC port\n");
		return EXIT_FAILURE;
	}

	/* Map the handler to device nodes under /dev */
	if (create_dev(&server_oid, "caesar_encode") < 0) {
		fprintf(stderr, "caesar_encode: Failed to map device in filesystem\n");
		return EXIT_FAILURE;
	}

	printf("caesar_encode: Server is up at /dev/caesar_encode\n");

	/* Enter receiving loop */
	run_server_message_loop(&server_oid);

	return EXIT_FAILURE;
}
