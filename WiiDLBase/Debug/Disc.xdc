<?xml version="1.0"?>
<doc>
<members>
<member name="T:Disc" decl="false" source="z:\development\wiidl\wiidlbase\disc.h" line="40">
<summary>
Represents a Wii disc image (ISO) and contains most of the methods for working with the image
</summary>
</member>
<member name="T:Utils" decl="false" source="z:\development\wiidl\wiidlbase\utils.h" line="6">
<summary>
Contains utility functions
</summary>
</member>
<member name="M:Disc.#ctor(System.SByte!System.Runtime.CompilerServices.IsSignUnspecifiedByte*)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="11">
<summary>
Constructor. Creates a Disc object
<param name="IsoFilename">The path of the Wii ISO file that the Disc object will access</param>
</summary>
</member>
<member name="M:Disc.Dispose" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="27">
<summary>
Destructor. Disposes of the Disc object
</summary>
</member>
<member name="M:Disc.GetLastError" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="44">
<summary>
Gets the last error message that occured
<returns>The most recent error message</returns>
</summary>
</member>
<member name="M:Disc.Load(System.Boolean)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="54">
<summary>
Opens the Wii ISO
<param name="readOnly">Whether or not the ISO should be opened in read only mode. Opening in read only mode will prevent the image file from being locked to other applications but any write actions to the image will fail</param>
<returns>True if the ISO was successfully opened. Otherwise false</returns>
</summary>
</member>
<member name="M:Disc.CloseFile" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="189">
<summary>
Closes the Wii ISO
<returns>True if the ISO was successfully closed. Otherwise false</returns>
</summary>
</member>
<member name="M:Disc.Read(System.Byte*,System.UInt32,System.UInt64,System.Boolean)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="212">
<summary>
Reads data from the iso into the buffer pointer
<param name="buffer">A pointer to a char buffer that the data will be stored in</param>
<param name="size">The amount of data to read in bytes</param>
<param name="offset">The offset at which to being the read operation</param>
<param name="markUsed">Indicates whether the clusters being read should be marked as used (eg not empty). Defaults to True</param>
<returns>The number of bytes read. Returns -1 if an error occurs</returns>
</summary>
</member>
<member name="M:Disc.ReadFromPartition(System.Byte*,System.UInt32,System.UInt32,System.UInt64)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="276">
<summary>
Reads data from the specified partition, decrypting it if necessary
<param name="buffer">A pointer to a char buffer that the data will be stored in</param>
<param name="size">The amount of data to read in bytes</param>
<param name="partNo">The partition to read from</param>
<param name="offset">The offset (from the beginning of the partition) at which to being the read operation</param>
<returns>The number of bytes read. Returns -1 if an error occurs</returns>
</summary>
</member>
<member name="M:Disc.DecryptPartitionBlock(System.UInt32,System.UInt32)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="323">
<summary>
Reads and decrypts a block of data from the specified partition
<param name="partNo">The partition to read from</param>
<param name="block">The block number to read</param>
<returns>The 0 on success. Returns -1 if an error occurs</returns>
</summary>
</member>
<member name="M:Disc.MarkAsUsed(System.UInt64,System.UInt64)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="358">
<summary>
Marks the clusters between nOffset and nOffset + nSize as used (eg not empty)
<param name="offset">The starting position</param>
<param name="size">The amount of data in bytes to mark as used (will round to the nearset sector)</param>
<returns>The number of clusters marked as used. 1 Cluster = 32768 bytes (32k)</returns>
</summary>
</member>
<member name="M:Disc.MarkAsUsedCrypto(System.UInt64,System.UInt64,System.UInt64,System.Boolean)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="384">
<summary>
Marks the clusters within a partition between nOffset and nOffset + nSize as used (eg not empty). Takes into account whether the data is encrypted and if so accomodates for the extra 1024 bytes of crypto data per cluster
<param name="partOffset">The starting position</param>
<param name="offset">The amount of data in bytes to mark as used (will round to the nearset sector)</param>
<param name="size">The amount of data in bytes to mark as used (will round to the nearset sector)</param>
<returns>The number of clusters marked as used. 1 Cluster = 32768 bytes (32k)</returns>
</summary>
</member>
<member name="T:part_header" decl="true" source="z:\development\wiidl\wiidlbase\disc.cpp" line="422">
<summary>
Parses the raw header data in inputData in a part_header structure pointed to by header
<param name="inputData">Pointer to the data to be parsed</param>
<returns>Pointer to a part_header structure</returns>
</summary>
</member>
<member name="M:Disc.LoadKey(System.Boolean)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="462">
<summary>
Loads the the correct common key for the image. The old WiiScrubber
used to load the key from a key.bin file, however since there is no
strong evidence that distrbuting the key is illegal, and since everyone
has it now anyway, it is embedded in this method. Just to be on the safe
side, the actual key is not embeded, instead XORd values are stored
<param name="inputData">Pointer to the data to be parsed</param>
<returns>Pointer to a part_header structure</returns>
</summary>
</member>
<member name="M:Disc.ParseImage" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="541">
<summary>
Parses the image file data into the image structure
<returns>0 = Success, -1 = Failure</returns>
</summary>
</member>
<member name="M:Disc.ParseFst(System.Byte*,System.SByte!System.Runtime.CompilerServices.IsSignUnspecifiedByte!System.Runtime.CompilerServices.IsConst*,System.SByte!System.Runtime.CompilerServices.IsSignUnspecifiedByte!System.Runtime.CompilerServices.IsConst*,System.UInt32,tree*,System.UInt32)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="715">
<summary>
Parses a partition's fst.bin file as passed into fst and adds all of it's files into the partitions Files collection
<param name="fst">Pointer to the fst.bin data</param>
<param name="names">??</param>
<param name="currentDir">The current directory. usually "\"</param>
<param name="i">??</param>
<param name="tree">??</param>
<param name="tree">??</param>
<param name="partNo">The partiton number to parse</param>
<returns>The number of directories or files parsed</returns>
</summary>
</member>
<member name="M:Disc.AddFileToPart(System.SByte!System.Runtime.CompilerServices.IsSignUnspecifiedByte!System.Runtime.CompilerServices.IsConst*,System.SByte!System.Runtime.CompilerServices.IsSignUnspecifiedByte!System.Runtime.CompilerServices.IsConst*,partition*,System.UInt64,System.UInt64)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="781">
<summary>
Creates a partition_file struct and adds it to the Files collection of the partition pointed to by part
Note that this method does NOT add a new file into the image
<param name="fileName">The name of the file</param>
<param name="part">The partition to add the file to</param>
<param name="offset">The offset of the file within the partition</param>
<param name="size">The size of the file</param>
<returns>Pointer to a part_header structure</returns>
</summary>
</member>
<member name="M:Disc.ParsePartitions" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="807">
<summary>
<returns>The number of partitions loaded</returns>
</summary>
</member>
<member name="M:Disc.TmdLoad(System.UInt32)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="957">
<summary>
<param name="partNo">The partition number to load the TMD from</param>
</summary>
</member>
</members>
</doc>
