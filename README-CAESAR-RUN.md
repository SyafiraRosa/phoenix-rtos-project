# Caesar Cipher Servers for Phoenix RTOS

## Overview

This project implements two user-space servers for the Phoenix RTOS operating system that provide Caesar cipher encoding and decoding functionality. The servers use a constant shift value of 5 and communicate with client applications through the Phoenix RTOS message-based IPC mechanism.

### What is Caesar Cipher?

The Caesar cipher is a simple substitution cipher where each letter in the plaintext is shifted a fixed number of positions down the alphabet. With a shift of 5:
- `A` → `F`, `B` → `G`, ..., `V` → `A`, `W` → `B`, `X` → `C`, `Y` → `D`, `Z` → `E`
- Lowercase letters are shifted similarly: `a` → `f`, `b` → `g`, etc.
- Non-alphabetic characters (numbers, punctuation, spaces) remain unchanged

Reference: https://en.wikipedia.org/wiki/Caesar_cipher

## What Was Implemented

Two independent servers were created in the Phoenix RTOS project:

1. **caesar_encode** - Encodes plaintext using Caesar cipher (shift forward by 5)
2. **caesar_decode** - Decodes ciphertext using Caesar cipher (shift backward by 5)

### Key Features

- **Constant shift value**: 5 (as specified in requirements)
- **Case preservation**: Uppercase letters remain uppercase, lowercase remain lowercase
- **Non-alphabetic preservation**: Numbers, spaces, punctuation unchanged
- **Buffer size**: 4096 bytes per server
- **Device interface**: `/dev/caesar_encode` and `/dev/caesar_decode`
- **Phoenix RTOS IPC**: Uses message-based communication (msgRecv/msgRespond)

## File Structure

```
phoenix-rtos-project/
└── _user/
    ├── caesar_encode/
    │   ├── main.c          # Encode server implementation
    │   └── Makefile        # Build configuration
    └── caesar_decode/
        ├── main.c          # Decode server implementation
        └── Makefile        # Build configuration
```

### Implementation Details

Both servers follow the Phoenix RTOS server pattern:

1. **Port Creation**: Each server creates a unique port using `portCreate()`
2. **Device Registration**: Registers device files in `/dev` using `create_dev()`
3. **Message Loop**: Continuously receives and processes messages via `msgRecv()`
4. **Message Handling**: Supports `mtOpen`, `mtClose`, `mtRead`, `mtWrite` message types
5. **Response**: Sends responses back to clients using `msgRespond()`

#### Encode Server (`caesar_encode`)

- **Function**: `caesar_encode_char()` - Shifts each letter forward by 5 positions
- **Algorithm**: `new_char = 'A' + ((old_char - 'A' + 5) % 26)` for uppercase
- **Workflow**:
  1. Client writes plaintext to `/dev/caesar_encode`
  2. Server encodes the text and stores in internal buffer
  3. Client reads encoded text from `/dev/caesar_encode`

#### Decode Server (`caesar_decode`)

- **Function**: `caesar_decode_char()` - Shifts each letter backward by 5 positions
- **Algorithm**: `new_char = 'A' + ((old_char - 'A' - 5 + 26) % 26)` for uppercase
- **Workflow**:
  1. Client writes ciphertext to `/dev/caesar_decode`
  2. Server decodes the text and stores in internal buffer
  3. Client reads decoded text from `/dev/caesar_decode`

## Building the Servers

### Prerequisites

- Phoenix RTOS development environment set up
- Access to the `syafirarosa/scheduling-syscalls` branch

### Build Steps

1. **Navigate to the project directory**:
   ```bash
   cd phoenix-rtos-project
   ```

2. **Ensure you're on the correct branch**:
   ```bash
   git checkout syafirarosa/scheduling-syscalls
   ```

3. **Build the entire project** (includes Caesar servers):
   ```bash
   ./phoenix-rtos-build/build.sh clean all
   ```

   Or build just the user applications:
   ```bash
   cd _user
   make
   ```

4. **Build individual servers** (optional):
   ```bash
   cd _user/caesar_encode
   make
   
   cd ../caesar_decode
   make
   ```

The servers will be compiled as `caesar_encode` and `caesar_decode` binaries.

## Running the Servers

### Starting the Servers

1. **Boot Phoenix RTOS** (using QEMU or target hardware)

2. **Start the encode server**:
   ```bash
   /usr/bin/caesar_encode &
   ```
   
   Expected output:
   ```
   caesar_encode: Server started at /dev/caesar_encode
   ```

3. **Start the decode server**:
   ```bash
   /usr/bin/caesar_decode &
   ```
   
   Expected output:
   ```
   caesar_decode: Server started at /dev/caesar_decode
   ```

### Using the Servers

#### Example 1: Encoding Text

```bash
# Write plaintext to encode server
echo "HELLO WORLD" > /dev/caesar_encode

# Read encoded text
cat /dev/caesar_encode
```

**Expected output**: `MJQQT BTWQI`

#### Example 2: Decoding Text

```bash
# Write ciphertext to decode server
echo "MJQQT BTWQI" > /dev/caesar_decode

# Read decoded text
cat /dev/caesar_decode
```

**Expected output**: `HELLO WORLD`

#### Example 3: Round-trip Test

```bash
# Encode then decode
echo "Phoenix RTOS" > /dev/caesar_encode
cat /dev/caesar_encode > /tmp/encoded.txt
cat /tmp/encoded.txt
# Output: Umjjsne WYTX

cat /tmp/encoded.txt > /dev/caesar_decode
cat /dev/caesar_decode
# Output: Phoenix RTOS
```

#### Example 4: Testing with Mixed Content

```bash
echo "Test123 with numbers!" > /dev/caesar_encode
cat /dev/caesar_encode
```

**Expected output**: `Yjxy123 bnym szrgjwx!`

Note: Numbers and punctuation remain unchanged, only letters are shifted.

## Expected Output and Behavior

### Encode Server

| Input | Output | Explanation |
|-------|--------|-------------|
| `HELLO` | `MJQQT` | Each letter shifted forward by 5 |
| `hello` | `mjqqt` | Lowercase preserved |
| `ABC xyz` | `FGH cde` | Case and space preserved |
| `Test123!` | `Yjxy123!` | Numbers/punctuation unchanged |
| `VWXYZ` | `ABCDE` | Wraps around at end of alphabet |

### Decode Server

| Input | Output | Explanation |
|-------|--------|-------------|
| `MJQQT` | `HELLO` | Each letter shifted backward by 5 |
| `mjqqt` | `hello` | Lowercase preserved |
| `FGH cde` | `ABC xyz` | Case and space preserved |
| `Yjxy123!` | `Test123!` | Numbers/punctuation unchanged |
| `ABCDE` | `VWXYZ` | Wraps around at start of alphabet |

### Server Logs

When processing messages, servers print diagnostic information:

```
caesar_encode: Encoded 11 bytes
caesar_decode: Decoded 11 bytes
```

## Testing the Implementation

### Manual Testing

1. **Basic functionality test**:
   ```bash
   echo "ABCDEFGHIJKLMNOPQRSTUVWXYZ" > /dev/caesar_encode
   cat /dev/caesar_encode
   # Expected: FGHIJKLMNOPQRSTUVWXYZABCDE
   ```

2. **Round-trip test**:
   ```bash
   ORIGINAL="The quick brown fox jumps over the lazy dog"
   echo "$ORIGINAL" > /dev/caesar_encode
   cat /dev/caesar_encode > /dev/caesar_decode
   RESULT=$(cat /dev/caesar_decode)
   
   if [ "$ORIGINAL" = "$RESULT" ]; then
       echo "PASS: Round-trip successful"
   else
       echo "FAIL: Round-trip failed"
   fi
   ```

3. **Edge cases**:
   ```bash
   # Test wrap-around
   echo "XYZ" > /dev/caesar_encode
   cat /dev/caesar_encode  # Expected: CDE
   
   # Test decode wrap-around
   echo "ABC" > /dev/caesar_decode
   cat /dev/caesar_decode  # Expected: VWX
   ```

### Automated Testing

Create a test script (`test_caesar.sh`):

```bash
#!/bin/sh

test_encode() {
    INPUT="$1"
    EXPECTED="$2"
    echo "$INPUT" > /dev/caesar_encode
    RESULT=$(cat /dev/caesar_encode)
    
    if [ "$RESULT" = "$EXPECTED" ]; then
        echo "PASS: encode('$INPUT') = '$RESULT'"
        return 0
    else
        echo "FAIL: encode('$INPUT') = '$RESULT', expected '$EXPECTED'"
        return 1
    fi
}

test_decode() {
    INPUT="$1"
    EXPECTED="$2"
    echo "$INPUT" > /dev/caesar_decode
    RESULT=$(cat /dev/caesar_decode)
    
    if [ "$RESULT" = "$EXPECTED" ]; then
        echo "PASS: decode('$INPUT') = '$RESULT'"
        return 0
    else
        echo "FAIL: decode('$INPUT') = '$RESULT', expected '$EXPECTED'"
        return 1
    fi
}

# Run tests
test_encode "HELLO" "MJQQT"
test_encode "hello" "mjqqt"
test_encode "XYZ" "CDE"
test_decode "MJQQT" "HELLO"
test_decode "CDE" "XYZ"
```

## Future Improvements and Research Directions

### 1. **Configurable Shift Value**

**Current**: Shift is hardcoded to 5  
**Improvement**: Allow dynamic shift configuration via ioctl or special device file

```c
// Example: ioctl to set shift value
ioctl(fd, CAESAR_SET_SHIFT, 13);  // ROT13
```

**Research value**: Study how configuration affects server performance and security

### 2. **Extended Cipher Support**

**Improvements**:
- Vigenère cipher (polyalphabetic substitution)
- Atbash cipher (reverse alphabet)
- ROT13 (shift of 13, self-inverse)
- Affine cipher (ax + b mod 26)

**Research value**: Compare performance and security of different classical ciphers in embedded systems

### 3. **Streaming Mode**

**Current**: Buffer-based (4KB limit)  
**Improvement**: Support streaming for large files

```c
// Process data in chunks without buffering entire content
while (read(input_fd, chunk, CHUNK_SIZE) > 0) {
    write(encode_fd, chunk, CHUNK_SIZE);
    read(encode_fd, encoded_chunk, CHUNK_SIZE);
    write(output_fd, encoded_chunk, CHUNK_SIZE);
}
```

**Research value**: Analyze memory usage and throughput for large data processing

### 4. **Multi-threaded Server**

**Current**: Single-threaded message loop  
**Improvement**: Handle multiple clients concurrently

**Research value**: Study IPC performance under concurrent load in Phoenix RTOS

### 5. **Performance Benchmarking**

**Metrics to measure**:
- Throughput (bytes/second)
- Latency (message round-trip time)
- CPU usage
- Memory footprint
- Context switch overhead

**Research value**: Characterize microkernel IPC performance for text processing tasks

### 6. **Security Enhancements**

**Improvements**:
- Rate limiting (prevent DoS)
- Input validation (buffer overflow protection)
- Access control (restrict which processes can use servers)
- Audit logging (track all encode/decode operations)

**Research value**: Study security mechanisms in microkernel architectures

### 7. **Cryptographic Upgrades**

**Current**: Caesar cipher (trivially breakable)  
**Improvements**:
- AES encryption server
- RSA public-key encryption
- Hash function server (SHA-256)

**Research value**: Compare symmetric vs asymmetric crypto performance in embedded RTOS

### 8. **Error Handling and Resilience**

**Improvements**:
- Graceful handling of buffer overflow
- Recovery from corrupted messages
- Watchdog timer for hung operations
- Automatic restart on crash

**Research value**: Study fault tolerance in microkernel services

### 9. **Integration with File System**

**Improvement**: Transparent encryption/decryption layer

```bash
# Mount encrypted filesystem
mount -t caesarfs /dev/sda1 /mnt/encrypted

# Files automatically encrypted on write, decrypted on read
echo "secret" > /mnt/encrypted/file.txt
cat /mnt/encrypted/file.txt  # Returns "secret" (decrypted)
cat /dev/sda1  # Shows "xjhwjy" (encrypted)
```

**Research value**: Study filesystem-level encryption in microkernel OS

### 10. **Network Service**

**Improvement**: Expose servers over network (TCP/UDP)

```bash
# Client on remote machine
echo "HELLO" | nc phoenix-rtos-host 9000
# Returns: MJQQT
```

**Research value**: Analyze network protocol overhead in microkernel IPC

### 11. **Frequency Analysis Tool**

**Addition**: Server that performs cryptanalysis

```bash
cat ciphertext.txt > /dev/caesar_analyze
cat /dev/caesar_analyze
# Output: Detected shift: 5, Confidence: 98%
```

**Research value**: Study pattern recognition and statistical analysis in embedded systems

### 12. **Comparison with Monolithic Kernel**

**Research**: Implement same servers in Linux and compare:
- Performance (throughput, latency)
- Code complexity
- Security isolation
- Resource usage

**Value**: Quantify microkernel vs monolithic tradeoffs for text processing services

## Technical Notes

### Message Types Handled

- `mtOpen`: Initialize server connection, clear buffer
- `mtClose`: Close server connection
- `mtRead`: Return processed text to client
- `mtWrite`: Receive input text from client, process it

### Buffer Management

- Each server maintains a 4KB static buffer
- Buffer is cleared on `mtOpen`
- Write operations replace buffer content (not append)
- Read operations support offset-based access

### Error Handling

- `EINVAL`: Invalid offset
- `ENOMEM`: Input exceeds buffer size (>4KB)
- `ENOSYS`: Unsupported message type
- `EINTR`: Signal interruption (automatically retried)

## References

- [Caesar Cipher - Wikipedia](https://en.wikipedia.org/wiki/Caesar_cipher)
- [Phoenix RTOS Project](https://github.com/phoenix-rtos/phoenix-rtos-project)
- [Phoenix RTOS Documentation](https://phoenix-rtos.github.io/phoenix-rtos-doc/)
- [Syafira Rosa's Fork](https://github.com/SyafiraRosa/phoenix-rtos-project/tree/syafirarosa/scheduling-syscalls)

## License

This implementation follows the Phoenix RTOS license. See the LICENSE file in the Phoenix RTOS project root.

## Author

Implementation created for educational purposes as part of Phoenix RTOS system programming research.

---

**Last Updated**: 2026-05-20
