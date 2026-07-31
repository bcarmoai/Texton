# Microsoft Developer Studio Project File - Name="Texton" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Texton - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Texton.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Texton.mak" CFG="Texton - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Texton - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Texton - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Texton - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glaux.lib vtkFiltering.lib vtkGraphics.lib vtkHybrid.lib vtkImaging.lib vtkIO.lib vtkRendering.lib vtkCommon.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Texton - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glaux.lib vtkFiltering.lib vtkGraphics.lib vtkHybrid.lib vtkImaging.lib vtkIO.lib vtkRendering.lib vtkCommon.lib /nologo /subsystem:windows /profile /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "Texton - Win32 Release"
# Name "Texton - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Border.cpp
# End Source File
# Begin Source File

SOURCE=.\CannyDetector.cpp
# End Source File
# Begin Source File

SOURCE=.\CannyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CMRStreamlines.cpp
# End Source File
# Begin Source File

SOURCE=.\Condensation.cpp
# End Source File
# Begin Source File

SOURCE=.\Data3DMRI.cpp
# End Source File
# Begin Source File

SOURCE=.\DataMRI.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg7DLoadOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgArcFit.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgContourOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFindVortex.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgHough.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMoreVtx.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRestoration.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRestorROI.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSaveVtkArrows.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgStreamlines.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgStrmZoom.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTasks.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTextBox.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTracking.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgVolumeDicom.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgVolView.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgVoorhees.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgVortexVtk.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgZoomedSlice.cpp
# End Source File
# Begin Source File

SOURCE=.\FlowAbstraction.cpp
# End Source File
# Begin Source File

SOURCE=.\FlowCluster.cpp
# End Source File
# Begin Source File

SOURCE=.\HCluster.cpp
# End Source File
# Begin Source File

SOURCE=.\HeartCFDCell.cpp
# End Source File
# Begin Source File

SOURCE=.\HeartCFDVertex.cpp
# End Source File
# Begin Source File

SOURCE=.\HoughPlotter.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageColour.cpp
# End Source File
# Begin Source File

SOURCE=.\ImagePgm.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageRGB.cpp
# End Source File
# Begin Source File

SOURCE=.\Int.cpp
# End Source File
# Begin Source File

SOURCE=.\IntArray.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\nrutil.cpp
# End Source File
# Begin Source File

SOURCE=.\NumrecEigenvalues.cpp
# End Source File
# Begin Source File

SOURCE=.\NumrecOptimiser.cpp
# End Source File
# Begin Source File

SOURCE=.\Plane.cpp
# End Source File
# Begin Source File

SOURCE=.\Point3D.cpp
# End Source File
# Begin Source File

SOURCE=.\PointFloat3D.cpp
# End Source File
# Begin Source File

SOURCE=.\PointFloatArray.cpp
# End Source File
# Begin Source File

SOURCE=.\Rect3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Rnd_no_gen.cpp
# End Source File
# Begin Source File

SOURCE=.\SortedList.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\test3d.cpp
# End Source File
# Begin Source File

SOURCE=.\Texton.cpp
# End Source File
# Begin Source File

SOURCE=.\Texton.rc
# End Source File
# Begin Source File

SOURCE=.\TextonDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\TextonView.cpp
# End Source File
# Begin Source File

SOURCE=.\ThresDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Vector3D.cpp
# End Source File
# Begin Source File

SOURCE=.\VoorheesBlobs.cpp
# End Source File
# Begin Source File

SOURCE=.\VtkUtilities.cpp
# End Source File
# Begin Source File

SOURCE=.\WrapperIftw.cpp
# End Source File
# Begin Source File

SOURCE=.\WrapperLoG.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\3d.h
# End Source File
# Begin Source File

SOURCE=.\Border.h
# End Source File
# Begin Source File

SOURCE=.\CannyDetector.h
# End Source File
# Begin Source File

SOURCE=.\CannyDlg.h
# End Source File
# Begin Source File

SOURCE=.\CMRStreamlines.h
# End Source File
# Begin Source File

SOURCE=.\Condensation.h
# End Source File
# Begin Source File

SOURCE=.\Data3DMRI.h
# End Source File
# Begin Source File

SOURCE=.\DataMRI.h
# End Source File
# Begin Source File

SOURCE=.\Dlg7DLoadOptions.h
# End Source File
# Begin Source File

SOURCE=.\DlgArcFit.h
# End Source File
# Begin Source File

SOURCE=.\DlgContourOptions.h
# End Source File
# Begin Source File

SOURCE=.\DlgFindVortex.h
# End Source File
# Begin Source File

SOURCE=.\DlgHough.h
# End Source File
# Begin Source File

SOURCE=.\DlgMoreVtx.h
# End Source File
# Begin Source File

SOURCE=.\DlgRestoration.h
# End Source File
# Begin Source File

SOURCE=.\DlgRestorROI.h
# End Source File
# Begin Source File

SOURCE=.\DlgSaveVtkArrows.h
# End Source File
# Begin Source File

SOURCE=.\DlgStreamlines.h
# End Source File
# Begin Source File

SOURCE=.\DlgStrmZoom.h
# End Source File
# Begin Source File

SOURCE=.\DlgTasks.h
# End Source File
# Begin Source File

SOURCE=.\DlgTextBox.h
# End Source File
# Begin Source File

SOURCE=.\DlgTracking.h
# End Source File
# Begin Source File

SOURCE=.\DlgVolumeDicom.h
# End Source File
# Begin Source File

SOURCE=.\DlgVolView.h
# End Source File
# Begin Source File

SOURCE=.\DlgVoorhees.h
# End Source File
# Begin Source File

SOURCE=.\DlgVortexVtk.h
# End Source File
# Begin Source File

SOURCE=.\DlgZoomedSlice.h
# End Source File
# Begin Source File

SOURCE=.\FlowAbstraction.h
# End Source File
# Begin Source File

SOURCE=.\FlowCluster.h
# End Source File
# Begin Source File

SOURCE=.\HCluster.h
# End Source File
# Begin Source File

SOURCE=.\HeartCFDCell.h
# End Source File
# Begin Source File

SOURCE=.\HeartCFDVertex.h
# End Source File
# Begin Source File

SOURCE=.\HoughPlotter.h
# End Source File
# Begin Source File

SOURCE=.\ImageColour.h
# End Source File
# Begin Source File

SOURCE=.\ImagePgm.h
# End Source File
# Begin Source File

SOURCE=.\ImageRGB.h
# End Source File
# Begin Source File

SOURCE=.\imglib.h
# End Source File
# Begin Source File

SOURCE=.\Int.h
# End Source File
# Begin Source File

SOURCE=.\IntArray.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\nrutil.h
# End Source File
# Begin Source File

SOURCE=.\NumrecEigenvalues.h
# End Source File
# Begin Source File

SOURCE=.\NumrecOptimiser.h
# End Source File
# Begin Source File

SOURCE=.\Plane.h
# End Source File
# Begin Source File

SOURCE=.\Point3D.h
# End Source File
# Begin Source File

SOURCE=.\PointFloat3D.h
# End Source File
# Begin Source File

SOURCE=.\PointFloatArray.h
# End Source File
# Begin Source File

SOURCE=.\Rect3D.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Rnd_no_gen.h
# End Source File
# Begin Source File

SOURCE=.\SortedList.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TemplateMatcher.h
# End Source File
# Begin Source File

SOURCE=.\Texton.h
# End Source File
# Begin Source File

SOURCE=.\TextonDoc.h
# End Source File
# Begin Source File

SOURCE=.\TextonView.h
# End Source File
# Begin Source File

SOURCE=.\ThresDlg.h
# End Source File
# Begin Source File

SOURCE=.\Vector3D.h
# End Source File
# Begin Source File

SOURCE=.\VoorheesBlobs.h
# End Source File
# Begin Source File

SOURCE=.\VtkUtilities.h
# End Source File
# Begin Source File

SOURCE=.\WrapperIftw.h
# End Source File
# Begin Source File

SOURCE=.\WrapperLoG.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\arrow.cur
# End Source File
# Begin Source File

SOURCE=.\arrowcop.cur
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\soton.ico
# End Source File
# Begin Source File

SOURCE=.\res\Texton.ico
# End Source File
# Begin Source File

SOURCE=.\res\Texton.rc2
# End Source File
# Begin Source File

SOURCE=.\res\TextonDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\zoom.cur
# End Source File
# Begin Source File

SOURCE=.\res\zoom_ico.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\log.txt
# End Source File
# Begin Source File

SOURCE=.\MACROS.INC
# End Source File
# Begin Source File

SOURCE=.\matlab_strms.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section Texton : {FDB643E9-D931-4890-95AD-57093090D93C}
# 	2:15:FlowCluster.cpp:FlowCluster.cpp
# 	2:18:CLASS: FlowCluster:FlowCluster
# 	2:19:Application Include:texton.h
# 	2:13:FlowCluster.h:FlowCluster.h
# End Section
# Section Texton : {D6D896C6-EBA0-43B9-8F8E-9DB79E037F42}
# 	2:15:CLASS: HCluster:HCluster
# 	2:10:HCluster.h:HCluster.h
# 	2:12:HCluster.cpp:HCluster.cpp
# 	2:19:Application Include:texton.h
# End Section
# Section Texton : {71ED08B4-2F2B-443E-9B5B-0C3FBAA4A8E7}
# 	2:22:CLASS: FlowAbstraction:FlowAbstraction
# 	2:17:FlowAbstraction.h:FlowAbstraction.h
# 	2:19:FlowAbstraction.cpp:FlowAbstraction.cpp
# 	2:19:Application Include:texton.h
# End Section
# Section Texton : {103C166C-98B6-4324-BF0C-7B7B36F63C65}
# 	2:16:CLASS: CVector2D:CVector2D
# 	2:10:Vector2D.h:Vector2D.h
# 	2:12:Vector2D.cpp:Vector2D.cpp
# 	2:19:Application Include:texton.h
# End Section
# Section Texton : {37606116-8D87-4B57-9A47-645B83768648}
# 	2:7:Plane.h:Plane.h
# 	2:19:Application Include:texton.h
# 	2:13:CLASS: CPlane:CPlane
# 	2:9:Plane.cpp:Plane.cpp
# End Section
# Section Texton : {30D9A35E-4039-4EC5-A0B1-6F20EC3F19A4}
# 	2:19:FlowCompression.cpp:FlowCompression.cpp
# 	2:22:CLASS: FlowCompression:FlowCompression
# 	2:19:Application Include:texton.h
# 	2:17:FlowCompression.h:FlowCompression.h
# End Section
# Section Texton : {6D4ECFDD-6077-4DB0-8F4C-31A30A71D834}
# 	2:14:SortedList.cpp:SortedList.cpp
# 	2:18:CLASS: CSortedList:CSortedList
# 	2:16:CLASS: CListNode:CListNode
# 	2:12:SortedList.h:SortedList.h
# 	2:19:Application Include:texton.h
# End Section
# Section Texton : {D3B87FE4-22F0-403F-B8CE-259FEB7C8647}
# 	2:16:CLASS: CVector3D:CVector3D
# 	2:10:Vector3D.h:Vector3D.h
# 	2:12:Vector3D.cpp:Vector3D.cpp
# 	2:19:Application Include:texton.h
# End Section
