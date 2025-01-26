# File Formats

## File Names

All file names follow this structure:

**PREFIXTYPE_DATE.FILETYPE**

**PREFIX**: The filePrefix from the configuration file (or "test_unit_" if not defined).

**TYPE**: Empty for binary files. Otherwise "metrics" or "cars" for CSV files.

**DATE**: UTC time when the file was created in, e.g. "2025-01-21_13-58-05"

**FILETYPE**: "bin" for the binary file, "csv" for CSV files.

## Raw Binary File

Current file format version: 3

### Header

* 2 bytes (uint16): File Format Version
* 4 bytes (time_t): Timestamp of file creation
* 2 bytes (uint16): number of fft bins
* 1 byte  (uint_8): size of each frequency value (1 or 4 bytes) -> D_SIZE
* 1 byte  (bool): use iq
* 2 bytes (uint16): sample rate
* 4 bytes (uint32): Teensy Unique Device Id

### Chunk Data

For each FFT a single chunk of data is written.

* 4 bytes (uint32): Milliseconds the sensor has been running
* 4 bytes (uint32): number of values to read next -> NUM
* if in 8bit mode (Header:D_SIZE is 1): NUM bytes (uint_8): frequency values
* if not in 8bit mode (Header:D_SIZE is 4): 4 x NUM bytes (NUM x float): frequency values
* 4 bytes (0xffffffff): chunk end marker; all ones; NaN as float

### Error Correction

None.

When reading, check for the end marker at the end of chunk data. If it is not all 1s, discard the last chunk and
search for the next end marker. Start reading again after that one.

## CSV Files

The CSV files contain "header" lines (with a leading "//" comment mark) where additional information is stored. This includes data like the Teensy id, current file format version and some parameters used during the signal detection (like the used thresholds or smoothing factors).

After that, like in any CSV file, you find the header information. These fields mostly map 1:1 to variables used during
detection.

Please see FileIO.cpp for up do date information.
