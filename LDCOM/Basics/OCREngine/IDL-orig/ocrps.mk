ocrps.dll: dlldata.obj ocr_p.obj ocr_i.obj
  link /dll /out:ocrps.dll /def:ocrps.def \
  /entry:DllMain dlldata.obj ocr_p.obj ocr_i.obj \
  kernel32.lib rpcrt4.lib 

.c.obj:
  cl /c /DWIN32 /D_WIN32_WINNT=0x0500 /DREGISTER_PROXY_DLL $<
