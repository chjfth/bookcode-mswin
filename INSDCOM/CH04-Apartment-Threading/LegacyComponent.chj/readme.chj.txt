[2020-06-25] Chj:

This example tests the behavior of so-called inproc Legacy Component, that is, a component that does NOT have ThreadingModel regitem in the registry. 

According to the book author, such component will always be created in the "main STA", that is, the first STA created for the process. 

The first STA is not established until some thread in the process for the first time calls CoCreateInstanceEx(COINIT_APARTMENTTHREADED).

I use new CLSID/IID for this example, so that we can try it side-by-side with other examples.
