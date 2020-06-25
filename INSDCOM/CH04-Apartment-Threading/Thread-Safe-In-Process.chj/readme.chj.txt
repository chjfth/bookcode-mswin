[2020-06-25] Chj:

How to run this example?
========================

Open Thread-Safe.chj.sln with Visual Studio 2010, build the solution, and you will get output files in the following folders:

	$\__Debug\Win32
	$\__Debug\x64
	$\__Release\Win32
	$\__Release\x64

The all contain the same set of files, so pick one suitable set to use. For example, we pick $\__Debug\Win32 .

To install the COM component onto the system, we need to run(as Administrator):

	regsvr32 .\__Debug\Win32\Component.dll

Then, we test the component with:

	.\__Debug\Win32\Client.exe

Tweak the component behavior by changing component threading model(apartment).

The regsvr32 command above created this regkey for us:

[HKEY_CLASSES_ROOT\CLSID\{10004002-0000-0000-0000-000000000001}\InprocServer32]
ThreadingModel="Apartment"

We can change it to ThreadingModel="Free" or ThreadingModel="Both" to see the difference.

More details are recorded at https://www.evernote.com/l/ABVL36rYHw9NzI1bBnOmNQgEv8Heula2Jc8/

