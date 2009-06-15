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
<returns>True if the ISO was successfully opened. Otherwise false</returns>
</summary>
</member>
<member name="M:Disc.Read(System.Byte*,System.UInt32,image_file*,System.UInt64,System.Boolean)" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="131">
<summary>
Reads data from the iso into the buffer pointer
<returns>The number of bytes read. Returns -1 if an error occurs</returns>
</summary>
</member>
<member name="M:Disc.Close" decl="false" source="z:\development\wiidl\wiidlbase\disc.cpp" line="164">
<summary>
Closes the Wii ISO
<returns>True if the ISO was successfully closed. Otherwise false</returns>
</summary>
</member>
</members>
</doc>
