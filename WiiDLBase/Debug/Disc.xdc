<?xml version="1.0"?>
<doc>
<members>
<member name="T:Disc" decl="false" source="z:\development\wiidl\wiidlbase\disc.h" line="38">
<summary>
Represents a Wii disc image (ISO) and contains most of the methods for working with the image
</summary>
</member>
<member name="T:Utils" decl="false" source="z:\development\wiidl\wiidlbase\utils.h" line="6">
<summary>
Contains utility functions
</summary>
</member>
<member name="M:Disc.#ctor(System.SByte!System.Runtime.CompilerServices.IsSignUnspecifiedByte*)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="8">
<summary>
Constructor. Creates a Disc object
<param name="IsoFilename">The path of the Wii ISO file that the Disc object will access</param>
</summary>
</member>
<member name="M:Disc.Dispose" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="18">
<summary>
Destructor. Disposes of the Disc object
</summary>
</member>
<member name="M:Disc.Open(System.Boolean)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="30">
<summary>
Opens the Wii ISO
<param name="readOnly">Whether or not the ISO should be opened in read only mode. Opening in read only mode will prevent the image file from being locked to other applications but any write actions to the image will fail</param>
<returns>True if the ISO was successfully opened. Otherwise false</returns>
</summary>
</member>
<member name="M:Disc.Close" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="135">
<summary>
Closes the Wii ISO
<returns>True if the ISO was successfully closed. Otherwise false</returns>
</summary>
</member>
<member name="M:Disc.Read(System.Byte*,System.UInt32,System.UInt64,System.Boolean)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="160">
<summary>
Reads data from the iso into the buffer pointer
<param name="buffer">A pointer to a char buffer that the data will be stored in</param>
<param name="size">The amount of data to read in bytes</param>
<param name="offset">The offset at which to being the read operation</param>
<param name="markUsed">Indicates whether the clusters being read should be marked as used (eg not empty). Defaults to True</param>
<returns>The number of bytes read. Returns -1 if an error occurs</returns>
</summary>
</member>
<member name="M:Disc.MarkAsUsed(System.UInt64,System.UInt64)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="215">
<summary>
Marks the clusters between nOffset and nOffset + nSize as used (eg not empty)
<param name="nOffset">The starting position</param>
<param name="nOffset">The amount of data in bytes to mark as used (will round to the nearset sector)</param>
<returns>The number of clusters marked as used. 1 Cluster = 32768 bytes (32k)</returns>
</summary>
</member>
</members>
</doc>
