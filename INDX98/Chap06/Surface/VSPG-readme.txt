[2025-08-17] 
When using VSPU-CopyOrClean.bat, remember to add "%ProjectDir%\lake.bmp" to AGILE_COPY_PATTERNS.

Like this:

	set AGILE_COPY_PATTERNS="%ExeDllDir%\%TargetFilenam%" "%ProjectDir%\lake.bmp"
