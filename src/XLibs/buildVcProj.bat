:: %1 - ProjectFileBase

set LibName=%XLibs%\VC7.1\%1


::BuildConsole %1.vcproj /cfg="Debug|Win32" /Rebuild /NoLogo /CL_REM="/MTd" /CL_ADD="/MLd" /LINK_ADD="/OUT:"%LibName%Dbg.lib""
BuildConsole %1.vcproj /cfg="Debug|Win32" /Rebuild /NoLogo 				 /LINK_ADD="/OUT:"%LibName%MtDbg.lib""
BuildConsole %1.vcproj /cfg="Debug|Win32" /Rebuild /NoLogo /CL_REM="/MTd" /CL_ADD="/MDd" /LINK_ADD="/OUT:"%LibName%DllDbg.lib""
BuildConsole %1.vcproj /cfg="Debug|Win32" /Clean /NoLogo 

::BuildConsole %1.vcproj /cfg="Release|Win32" /Rebuild /NoLogo /CL_REM="/MT" /CL_ADD="/ML" /LINK_ADD="/OUT:"%LibName%.lib""
@REM BuildConsole %1.vcproj /cfg="Release|Win32" /Rebuild /NoLogo 				 /LINK_ADD="/OUT:"%LibName%Mt.lib""
@REM BuildConsole %1.vcproj /cfg="Release|Win32" /Rebuild /NoLogo /CL_REM="/MT" /CL_ADD="/MD" /LINK_ADD="/OUT:"%LibName%Dll.lib""
@REM BuildConsole %1.vcproj /cfg="Release|Win32" /Clean /NoLogo 
