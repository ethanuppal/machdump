# machdump

![machdump in action](/img/example.png)

`machdump` is a tool to verbatim dump Mach-O object files for low-level debugging.

> NOTE: `machdump` only compiles on MacOS where the Mach-O headers are defined.

Simply give it one or more mach-o files on the command and it will dump each. It also responds to the universal options `--help` and `--version`.

## Usage

You can easily use this by cloning and making:
```bash
git clone https://github.com/ethanuppal/machdump
cd machdump
make
```
You can then install machdump with:
```bash
sudo cp machdump /usr/local/bin
```
