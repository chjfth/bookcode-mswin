import component.*;
//
//
// TestCOM
//
//
class TestCOM 
{
	public static void main(String str[])
	{
		ISum myRef = (ISum)new InsideDCOM();
		int result = myRef.Sum(5, 4);
		System.out.println("Java client: myRef.Sum(5, 4) returns " + result);
	}
}