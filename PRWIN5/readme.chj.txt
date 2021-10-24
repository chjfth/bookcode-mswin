[2021-10-24] 例子导入方法备忘：

Petzold 本书的大多例子都是一个 .c 加 一个 .rc 构成。对于此类例子可以如下处理：

以 Chap10\IconDemo 为例，

[1]	创建空白的 VS2010 solution，于 $\Chap10\PRWIN5-Chap10.sln 。

或者拷贝现成的 $\Chap03\PRWIN5-Chap03.sln 来用，拷贝后记得改名成 PRWIN5-Chap10.sln，打开 sln 后删除里头的无效工程。

[2] 拷贝 
	$\Chap03\HelloWinD\HelloWin.vcxproj
成为
	$\Chap10\IconDemo\IconDemo.vcxproj

[3] 将 IconDemo 的相关源文件拷贝到 $\Chap10\IconDemo 中，包括 .rc 和 .ico （若有的话）。

	IconDemo.c 改名为 IconDemo.cpp 。
	
	由于 IconDemo.c 里头的缩进很变态（5 字符缩进），稍后最好利用 Visual Assist X 的功能，cut/paste 一下，让 VAX 帮我们调整缩进格式。
	提示：VAX 中若是 Ctrl+A 全选后 cut/paste 可能不起作用，因此，不要全文选中，而是跳过头几行的注释，这样才能起效。
	
[4] 手工编辑 IconDemo.vcxproj ，修改两处：

第一处，接近开头部位， RootNamespace 值改成 IconDemo 。

  <PropertyGroup Label="Globals">
    <RootNamespace>HelloWin</RootNamespace>
  </PropertyGroup>

第二处，接近末尾部位，将 .rc 和 .ico 的名字改成 IconDemo 的。

  <ItemGroup>
    <ResourceCompile Include="App.rc" />
  </ItemGroup>
  <ItemGroup>
    <None Include="1.ico" />
  </ItemGroup>

当然，若新工程没有 .rc 和 .ico，删除相应的 <ItemGroup> 即可。

[5] 趁此时进行 git add 动作，因为尚未产生垃圾文件。等会儿用 Visual Studio 打开 sln 就会产生垃圾文件。

[6] 让 VSIDE 打开 PRWIN5-Chap10.sln，将 IconDemo.vcxproj 加入。


Q: 这么干比“让 VSIDE 打开 Petzold 的 dsw/dsp 自动升级”好在哪里？
===============================================================
A: 好在 HelloWin.vcxproj 是我手工打造过的，
* 直接提供了 x86 和 x64 两种 BuildConf （Debug/Release 都已设成 Unicode 变体）。
* 已经集成了 VSPG 。
* vcxproj 文件内容极度简洁。


Q: 对于需要 Unicode/ANSI 双栖编译的工程怎么办？
=============================================
A: 拿 $\Chap17\PickFont\PickFont.vcxproj 作为模板进行手工修改。

