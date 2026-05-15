# SNEK Odium
This project is licensed under the SNEK Common Source License 2.0 (S-CSL-2.0), please refer to LICENSE to better understand the terms and conditions regarding the usage of this software.

DISCLAIMER: Yes, this is to be compiled using gcc MSYS2 not MSVC.

SNEK Odium is The SNEK Initiatives attempt at creating a "secure" file storage system. This is a alpha version so do not excpect too much from it for now, and obviously we are not liable to any file corruption or inproper restoration that might occur, so don't use it on anything important for now. We also would be EXTREMELY gratefull for any and all feedback.

## So what does this do?

Odium takes a file and splits it into multiple encrypted fragments using a key based transformation and a "randomized" structure. It stores these fragments in generated directories along with a manifest that contains the information needed to rebuild the original file. The tool can later restore the original file from the shards when the correct key is provided.

We recommend navigating the tool using th built in TUI we made but for those who prefer making their own ui:

## API
The software also has a API surface you can use in your apps.

Flags:
Currently only 2 flags exist:
- `--shard` shards the file
- `--restr` restores a sharded file
shrimple, really.

Usage examples:
```sh
odium --shard exam.txt 1234
odium --restr eade07b853b2f23fg7dd24d7b5394e3d 1234
```

## Compilation

You can compile it using MSYS2 UCRT64 after installing gcc using the following command:

```sh
gcc odium.c -o odium -s -O2
```

we also recommend stripping it later:

```sh
strip odium.exe
```

### Long live freeware, ATroubledSnake.