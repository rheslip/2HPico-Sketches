# sysx2bank utility for the 2HPico PlaitsFM sketch - Feb 19/2026 

The easiest way to use this tool is to create a directory and drop this utility and 8 Yamaha DX7 .syx bank files into it. Run the utility and it will create a file "banks.cc" which you copy and paste into your PlaitsFM sketch folder. Recompile PlaitsFM to use the new banks/patches.

It only handles .syx files that have 32 patches which is the most common way for DX7 patches to be distributed. Take care to have no less than 8 .syx files i.e. one for each 6op engine in the PlaitsFM sketch. Any more than 8 will be ignored, less will not compile and link correctly.

The "banks.cc" file has the source file name and patch names in it. If you want to put banks/patches in a specific order you can cut and paste with an editor.