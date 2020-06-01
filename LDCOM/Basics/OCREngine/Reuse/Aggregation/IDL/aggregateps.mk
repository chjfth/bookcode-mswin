
aggregate.dll: dlldata.obj aggregate_p.obj aggregate_i.obj
	link /dll /out:aggregateps.dll /def:aggregateps.def /entry:DllMain dlldata.obj aggregate_p.obj aggregate_i.obj kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib 

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL $<

clean:
	@del aggregateps.dll
	@del aggregateps.lib
	@del aggregateps.exp
	@del dlldata.obj
	@del aggregate_p.obj
	@del aggregate_i.obj
