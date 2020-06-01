
containps.dll: dlldata.obj contain_p.obj contain_i.obj
	link /dll /out:containps.dll /def:containps.def /entry:DllMain dlldata.obj contain_p.obj contain_i.obj kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib 

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL $<

clean:
	@del containps.dll
	@del containps.lib
	@del containps.exp
	@del dlldata.obj
	@del contain_p.obj
	@del contain_i.obj
