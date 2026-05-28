# SNEK Odium
This project is licensed under the SNEK Common Source License 2.0 (S-CSL-2.0), please refer to LICENSE to better understand the terms and conditions regarding the usage of this software.

DISCLAIMER: Yes, this is to be compiled using gcc MSYS2 not MSVC.

SNEK Odium is The SNEK Initiatives attempt at creating a "secure" file storage system. This is now a beta version so you can finally excpect something from it, and obviously we are not liable to any file corruption or inproper restoration that might occur, so don't use it on anything important for now. We also would be EXTREMELY gratefull for any and all feedback.

## Updates
- Release version 2.0.0:
    - Logging system with 5 severity levels
    - Configuration management with INI files
    - File hashing, MD5 and SHA256, for integrity verification
    - Compression, supports NONE/DEF/GZ/ZST/LZ
    - Progress tracking and eta calculations
    - Binary manifest database for tracking shards
    - Batch file processing
    - API improvements
    - Archive support (tar, gz, zip, 7z, rar)
    - TUI improvements

## So what does this do?

Odium takes a file and splits it into multiple encrypted fragments using a key based transformation and a "randomized" structure. It stores these fragments in generated directories along with a manifest that contains the information needed to rebuild the original file. The tool can later restore the original file from the shards when the correct key is provided.

We recommend navigating the tool using th built in TUI we made but for those who prefer making their own ui:

## API
The software has a CLI with the following commands:

Main:
- `--shard` shards the file with encryption key
- `--restore` restores a sharded file using the key
- `--verify` verifies file integrity (MD5/SHA256)
- `--list` lists all manifest entries
- `--batch-shard` batch shard multiple files
- `--batch-restore` batch restore multiple shards
- `--genkey` generates a random encryption key
- `--info` shows system and config info
- `--version` shows version info
- `--help` shows this help message

Options:
- `--config` loads a configuration file
- `--verbose` increases verbosity
- `--threads` sets number of parallel threads
- `--no-verify` skips verification
- `--no-compress` disables compression

Usage examples:
```sh
odium --shard exam.txt 1234
odium --restore cry/shardid 1234 output.txt
odium --verify output.txt
odium --batch-shard 1234 --threads 4
odium --list
odium --genkey
```

## Compilation

You can compile it using MSYS2 UCRT64 after installing gcc using the following command:

```sh
gcc -Wall -std=c99 -O2 -o odium main.c utils.c archive.c crypto.c ui.c log.c cfg.c has.c cmp.c prg.c man.c bat.c cli.c
```

or just use the Makefile:

```sh
make
```

we also recommend stripping it later:

```sh
strip odium.exe
```

### Long live freeware, ATroubledSnake.