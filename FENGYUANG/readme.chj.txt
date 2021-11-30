[2021-11-30] FENGYUANG 例子导入方法备忘：

==== 背景 ====

配书源码的原始工程文件是 Visual C++ 6 dsw/dsp ，为了迎合时代变迁，我将其升级位 Visual C++ 2010 sln/vcxproj。

在 VS2010 IDE 中导入 VC6 旧工程的途径无非是两条：
(1) 将 .dsw/.dsp 交于 VS2010 去自动升级。
(2) 基于 VS2010 的模板工程，将 VC6 工程的源码加入。

目前的状态是：

※ 对于 wingraph.lib 库工程，上个月已经用自动升级法导入，工作良好，不去动它了。此工程源文件特别多，因此自动升级比较方便。

※ 对于大把的 exe 工程，我决定采用模版法。理由是：
(1) 不同的 exe 工程文件之间，差别极其微小，多是一两个 .cpp 源文件名字的差别，因此，手工复制一份 .vcxproj，手工编辑很方便。
(2) 从 dsp 自动升级而来的 vcxproj，其中有很多冗余，甚至过时的（在 VS2019 中编译出错）的老旧编译链接开关。如此一来，自动升级后还要再手工调整那些开关也很费时间。想想看有那么多 exe 工程，费的时间多了。
(3) 手工编辑 vcxproj，可以消除 vcxproj 中很多重复的语句。比较 Debug 和 Release 段落，就有很多重复语句。
(4) 因为要对 exe 工程实施 VSPG，本来就逃不过手工编辑这一关的，那么干脆一开始就手工编辑好了。


==== 举例 ====

已有可编译的 $\Chapt_07\GDIObj\GDIObj.vcxproj ，现在要将其作为模板，来“导入” $\Chapt_14\CH14-FontEmbed\FontEmbed.vcxproj 。

[1] 将 GDIObj.vcxproj 复制到目标目录，并改名成 FontEmbed.vcxproj 。

[2] 文本编辑 FontEmbed.vcxproj ，将

    <RootNamespace>GDIObj</RootNamespace>
改成
    <RootNamespace>FontEmbed</RootNamespace>

[3] 将原始的

	FontEmbed.cpp
	FontEmbed.rc
	resource.h

拷入目标目录（FontEmbed.vcxproj 中已经在引用它们了）。

[4] 将 CH14-FontEmbed 加入 git 仓库。

FontEmbed.vcxproj 中残留的 gditable.cpp，无须存在，可以将其从工程中移除。

[5] 编译 BuildConf [Debug] ，应该能编译成功。 

[6] VSIDE 中打开 Configuration Manager，将 sln 级别的 [Unicode Debug] 对应到 FontEmbed.vcxproj 的 [Unicode Debug]，编译之。

编译有可能不成功，因为 Feng Yuan 原始代码中有好些字符串还没有用 _T() 包起来，趁此机会修正之。


==== 模板注意事项 ====

GDIObj.vcxproj 是高度手工定制化过的。体现在：

(1) 对于 [Debug] 和 [Unicode Debug] 的共同配置项，用了如下手写的 MSBuild Condition ：

  <PropertyGroup Condition="$(Configuration.EndsWith('Debug'))">
    <LinkIncremental>true</LinkIncremental>
  </PropertyGroup>

(2) 对于 [Unicode Debug] 和 [Unicode Release] 的共同配置项，用了如下手写的 MSBuild Condition ：

  <PropertyGroup Condition="$(Configuration.IndexOf('Unicode'))==0">
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>

这些手写配置在什么场合下会被 VSIDE 的 Saving 动作给破坏，未有定论。因此，强烈建议自己手工维护这些 .vcxproj 。反之这些 exe 工程的结构也很简单，手工操作并不难。

