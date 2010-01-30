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
<member name="M:Disc.Dispose" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="29">
<summary>
Destructor. Disposes of the Disc object
</summary>
</member>
<member name="M:Disc.GetLastError" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="46">
<summary>
Gets the last error message that occured
<returns>The most recent error message</returns>
</summary>
</member>
<member name="M:Disc.Load(System.Boolean)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="56">
<summary>
Opens the Wii ISO
<param name="readOnly">Whether or not the ISO should be opened in read only mode. Opening in read only mode will prevent the image file from being locked to other applications but any write actions to the image will fail</param>
<returns>True if the ISO was successfully opened. Otherwise false</returns>
</summary>
</member>
<member name="M:Disc.CloseFile" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="192">
<summary>
Closes the Wii ISO
<returns>True if the ISO was successfully closed. Otherwise false</returns>
</summary>
</member>
<member name="M:Disc.Read(System.Byte*,System.UInt32,System.UInt64,System.Boolean)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="215">
<summary>
Reads data from the iso into the buffer pointer
<param name="buffer">A pointer to a char buffer that the data will be stored in</param>
<param name="size">The amount of data to read in bytes</param>
<param name="offset">The offset at which to being the read operation</param>
<param name="markUsed">Indicates whether the clusters being read should be marked as used (eg not empty). Defaults to True</param>
<returns>The number of bytes read. Returns -1 if an error occurs</returns>
</summary>
</member>
<member name="M:Disc.ReadFromPartition(System.Byte*,System.UInt32,System.UInt32,System.UInt64)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="279">
<summary>
Reads data from the specified partition, decrypting it if necessary
<param name="buffer">A pointer to a char buffer that the data will be stored in</param>
<param name="size">The amount of data to read in bytes</param>
<param name="partNo">The partition to read from</param>
<param name="offset">The offset (from the beginning of the partition) at which to being the read operation</param>
<returns>The number of bytes read. Returns -1 if an error occurs</returns>
</summary>
</member>
<member name="M:Disc.DecryptPartitionBlock(System.UInt32,System.UInt32)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="326">
<summary>
Reads and decrypts a block of data from the specified partition
<param name="partNo">The partition to read from</param>
<param name="block">The block number to read</param>
<returns>The 0 on success. Returns -1 if an error occurs</returns>
</summary>
</member>
<member name="M:Disc.MarkAsUsed(System.UInt64,System.UInt64)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="361">
<summary>
Marks the clusters between nOffset and nOffset + nSize as used (eg not empty)
<param name="offset">The starting position</param>
<param name="size">The amount of data in bytes to mark as used (will round to the nearset sector)</param>
<returns>The number of clusters marked as used. 1 Cluster = 32768 bytes (32k)</returns>
</summary>
</member>
<member name="M:Disc.MarkAsUsedCrypto(System.UInt64,System.UInt64,System.UInt64,System.Boolean)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="387">
<summary>
Marks the clusters within a partition between nOffset and nOffset + nSize as used (eg not empty). Takes into account whether the data is encrypted and if so accomodates for the extra 1024 bytes of crypto data per cluster
<param name="partOffset">The starting position</param>
<param name="offset">The amount of data in bytes to mark as used (will round to the nearset sector)</param>
<param name="size">The amount of data in bytes to mark as used (will round to the nearset sector)</param>
<returns>The number of clusters marked as used. 1 Cluster = 32768 bytes (32k)</returns>
</summary>
</member>
<member name="T:part_header" decl="true" source="z:\development\wiidl\wiidlbase\disc.cpp" line="444">
<summary>
Parses the raw header data in inputData in a part_header structure pointed to by header
<param name="inputData">Pointer to the data to be parsed</param>
<returns>Pointer to a part_header structure</returns>
</summary>
</member>
<member name="M:Disc.LoadKey(System.Boolean)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="484">
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
<member name="M:Disc.ParseImage" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="563">
<summary>
Parses the image file data into the image structure
<returns>0 = Success, -1 = Failure</returns>
</summary>
</member>
<member name="M:Disc.ParseFst(System.Byte*,System.SByte!System.Runtime.CompilerServices.IsSignUnspecifiedByte!System.Runtime.CompilerServices.IsConst*,partition_folder*,System.UInt32,tree*,System.UInt32)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="743">
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
<member name="M:Disc.ParsePartitions" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="811">
<summary>
Deprecated - Files are now children of the Folders collection
Creates a partition_file struct and adds it to the Files collection of the partition pointed to by part
Note that this method does NOT add a new file into the image
<param name="fileName">The name of the file</param>
<param name="part">The partition to add the file to</param>
<param name="offset">The offset of the file within the partition</param>
<param name="size">The size of the file</param>
<returns>Pointer to a part_header structure</returns>
</summary>
<summary>
Creates a partition_folder struct and adds it to the Folders collection of the partition pointed to by part
Note that this method does NOT add a new folder into the image
<param name="folderName">The name of the file</param>
<param name="part">The partition to add the folder to</param>
<param name="size">The size of the file</param>
<returns>Pointer to a part_header structure</returns>
</summary>
<summary>
Parses the partition information into the Partitions structure
<returns>The number of partitions loaded</returns>
</summary>
</member>
<member name="M:Disc.TmdLoad(System.UInt32)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="1007">
<summary>
Loads the TMD (Title Metadata) info for the give partition number
<param name="partNo">The partition number to load the TMD from</param>
</summary>
</member>
<member name="M:Disc.ExtractFile(System.SByte!System.Runtime.CompilerServices.IsSignUnspecifiedByte!System.Runtime.CompilerServices.IsConst*,PartitionFile*,System.Boolean)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="1127">
<summary>
Extracts a file to disk
<param name="destFilename">The filename and path to extract to</param>
<param name="file">A reference to the file object to extract</param>
<param name="decrypt">Whether or not the file should be decrypted when extracted</param>
</summary>
</member>
<member name="M:Disc.ReplaceFile(System.SByte!System.Runtime.CompilerServices.IsSignUnspecifiedByte!System.Runtime.CompilerServices.IsConst*,PartitionFile*,System.Boolean)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="1211">
<summary>
Replaces a file on the disc with the file given in inputFilename
<param name="inputFilename">The path and filename of the file to write to the image</param>
<param name="PartitionFile">The file to replace</param>
<param name="encrypt">Whether or not the file should be encrypted when written to the image</param>
</summary>
</member>
<member name="M:Disc.WriteData(System.Int32,System.UInt64,System.UInt64,System.Byte*,_iobuf*)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="1679">
<summary>
Heavily optimised file write routine so that the minimum number of
SHA calculations have to be performed                             
We do this by writing in 1 clustergroup per write and calculate the
Offset to write the data in the minimum number of chunks           
A bit like lumpy chunk packer from the Atari days...
<param name="partNo">The The partition number the file should be written to</param>
<param name="offset">The offset at which to write the file</param>
<param name="in">The data to write</param>
<param name="fIn">The file from which to read data that will be written (optional)</param>
</summary>
</member>
</members>
</doc>
