# Learnings During the Journey

How to setup c++ in vscode:
https://code.visualstudio.com/docs/cpp/config-mingw

Build??
For a multi-configuration generator (typically the case on Windows):
cmake -S . -B build
cmake --build build --config Release
from this page:
https://github.com/SFML/cmake-sfml-project/blob/master/README.md

x86_64-w64-mingw32/13.2.0

> set PATH=%PATH%;your_mingw_folder\bin
> cmake -G"MinGW Makefiles" ./build

https://terminalroot.com/how-to-compile-your-games-in-sfml-with-gcc-mingw-on-windows/

https://learn.microsoft.com/sv-se/cpp/build/cmake-projects-in-visual-studio?view=msvc-170&viewFallbackFrom=vs-2019

protobuf demo:
https://github.com/lytsing/protobuf-demo/tree/master


Lighting using: 
https://en.sfml-dev.org/forums/index.php?topic=27631.0
https://github.com/MiguelMJ/Candle

## Issues
Current linking issue, after trying to add gRPC to the project build:

  [2889/2890] Building CXX object CMakeFiles\CMakeSFMLProject.dir\src\WizardServer.cpp.obj
  [2890/2890] Linking CXX executable bin\CMakeSFMLProject.exe
  FAILED: bin/CMakeSFMLProject.exe 
  C:\WINDOWS\system32\cmd.exe /C "cd . && "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\CMakeSFMLProject.dir --rc=C:\PROGRA~2\WI3CF2~1\10\bin\100226~1.0\x64\rc.exe --mt=C:\PROGRA~2\WI3CF2~1\10\bin\100226~1.0\x64\mt.exe --manifests  -- C:\PROGRA~1\MICROS~2\2022\COMMUN~1\VC\Tools\MSVC\1439~1.335\bin\Hostx64\x64\link.exe /nologo @CMakeFiles\CMakeSFMLProject.rsp  /out:bin\CMakeSFMLProject.exe /implib:CMakeSFMLProject.lib /pdb:bin\CMakeSFMLProject.pdb /version:0.0 /machine:x64 /INCREMENTAL:NO /subsystem:console  && cd ."
  LINK: command "C:\PROGRA~1\MICROS~2\2022\COMMUN~1\VC\Tools\MSVC\1439~1.335\bin\Hostx64\x64\link.exe /nologo @CMakeFiles\CMakeSFMLProject.rsp /out:bin\CMakeSFMLProject.exe /implib:CMakeSFMLProject.lib /pdb:bin\CMakeSFMLProject.pdb /version:0.0 /machine:x64 /INCREMENTAL:NO /subsystem:console /MANIFEST:EMBED,ID=1" failed (exit code 1120) with the following output:
C:\Users\kemil\Code\babas_are_us\build\x64-Release\WizardServer.cpp.obj : error LNK2001: unresolved external symbol "private: virtual class std::basic_string_view<char,struct std::char_traits<char> > __cdecl absl::lts_20240116::flags_internal::FlagImpl::Name(void)const " (?Name@FlagImpl@flags_internal@lts_20240116@absl@@EEBA?AV?$basic_string_view@DU?$char_traits@D@std@@@std@@XZ)
C:\Users\kemil\Code\babas_are_us\build\x64-Release\WizardServer.cpp.obj : error LNK2001: unresolved external symbol "private: virtual bool __cdecl absl::lts_20240116::flags_internal::FlagImpl::ValidateInputValue(class std::basic_string_view<char,struct std::char_traits<char> >)const " (?ValidateInputValue@FlagImpl@flags_internal@lts_20240116@absl@@EEBA_NV?$basic_string_view@DU?$char_traits@D@std@@@std@@@Z)
C:\Users\kemil\Code\babas_are_us\build\x64-Release\WizardServer.cpp.obj : error LNK2001: unresolved external symbol "private: virtual bool __cdecl absl::lts_20240116::flags_internal::FlagImpl::ParseFrom(class std::basic_string_view<char,struct std::char_traits<char> >,enum absl::lts_20240116::flags_internal::FlagSettingMode,enum absl::lts_20240116::flags_internal::ValueSource,class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > &)" (?ParseFrom@FlagImpl@flags_internal@lts_20240116@absl@@EEAA_NV?$basic_string_view@DU?$char_traits@D@std@@@std@@W4FlagSettingMode@234@W4ValueSource@234@AEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@6@@Z)
C:\Users\kemil\Code\babas_are_us\build\x64-Release\WizardServer.cpp.obj : error LNK2019: unresolved external symbol "bool __cdecl absl::lts_20240116::flags_internal::AbslParseFlag(class std::basic_string_view<char,struct std::char_traits<char> >,unsigned short *,class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > *)" (?AbslParseFlag@flags_internal@lts_20240116@absl@@YA_NV?$basic_string_view@DU?$char_traits@D@std@@@std@@PEAGPEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@5@@Z) referenced in function "void * __cdecl absl::lts_20240116::flags_internal::FlagOps<unsigned short>(enum absl::lts_20240116::flags_internal::FlagOp,void const *,void *,void *)" (??$FlagOps@G@flags_internal@lts_20240116@absl@@YAPEAXW4FlagOp@012@PEBXPEAX2@Z)
C:\Users\kemil\Code\babas_are_us\build\x64-Release\WizardServer.cpp.obj : error LNK2019: unresolved external symbol "void __cdecl absl::lts_20240116::cord_internal::InitializeCordRepExternal(class std::basic_string_view<char,struct std::char_traits<char> >,struct absl::lts_20240116::cord_internal::CordRepExternal *)" (?InitializeCordRepExternal@cord_internal@lts_20240116@absl@@YAXV?$basic_string_view@DU?$char_traits@D@std@@@std@@PEAUCordRepExternal@123@@Z) referenced in function "class absl::lts_20240116::Cord __cdecl absl::lts_20240116::MakeCordFromExternal<class `private: static class absl::lts_20240116::Cord __cdecl grpc::ProtoBufferReader::MakeCordFromSlice(struct grpc_slice)'::`2'::<lambda_1> >(class std::basic_string_view<char,struct std::char_traits<char> >,class `private: static class absl::lts_20240116::Cord __cdecl grpc::ProtoBufferReader::MakeCordFromSlice(struct grpc_slice)'::`2'::<lambda_1> &&)" (??$MakeCordFromExternal@V<lambda_1>@?1??MakeCordFromSlice@ProtoBufferReader@grpc@@CA?AVCord@lts_20240116@absl@@Ugrpc_slice@@@Z@@lts_20240116@absl@@YA?AVCord@01@V?$basic_string_view@DU?$char_traits@D@std@@@std@@$$QEAV<lambda_1>@?1??MakeCordFromSlice@ProtoBufferReader@grpc@@CA?AV201@Ugrpc_slice@@@Z@@Z)
  
C:\Users\kemil\Code\babas_are_us\build\x64-Release\bin\CMakeSFMLProject.exe : fatal error LNK1120: 5 unresolved externals


Look here:
https://github.com/abseil/abseil-cpp/issues/1383

Tic Tac Toe example
https://github.com/OBess/grpc-Tic-Tac-Toe/blob/main/server/src/server.cpp

