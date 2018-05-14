# weex_js_engine
Weex JavaScript Runtime based on  google v8 JavaScript engine and apple javascriptCore!
 
## How to build
### Download NDK and Set Environment PATH  
1. Download NDK(version at least 'r13b')   
[NDK download address](http://developer.android.com/tools/sdk/ndk/index.html)
2. Add NDK folder to PATH   
add this line to your profile script:`export PATH=[Your NDK Folder]/:"$PATH"`
3.install cmake (version at least 3.9.0)
4.install Ninja
5.install gperf (apt-get install gperf)
### Compile JSC
Excute `cd [Your Source Folder]/weex_v8core/  &&  run build.jsc.sh`
 
