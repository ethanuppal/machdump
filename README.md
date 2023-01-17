# machdump

![machdump in action](/img/example.png)

`machdump` is a tool to verbatim dump Mach-O object files for low-level debugging. Mach-O is the executable file format on Apple software.

> NOTE: `machdump` only compiles on MacOS where the Mach-O headers are defined. If you can acquire those headers from Apple Open Source, you may be able to compile on other platforms.

Simply give it one or more Mach-O files on the command and it will dump each. It also responds to the universal options `--help` and `--version`.

## Usage

First, clone the repository and build the executable.
```bash
git clone https://github.com/ethanuppal/machdump
cd machdump
make
```
Then, install machdump with:
```bash
sudo cp machdump /usr/local/bin
```

## Similar Projects

There is a similar project under an identical name, which I found after I had already named this: https://github.com/GeoSn0w/MachDump. As of writing this, another person has created a parser in Zig very recently: https://gpanders.com/blog/exploring-mach-o-part-1/.
