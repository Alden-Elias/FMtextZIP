## FMtextZIP

本程序是基于哈夫曼编码的文本压缩程序，代码较短故实现的功能也比较有限，仅支持简单的文本压缩与解压功能和伪加密功能

### 基本使用

在命令行窗口下运行，总共支持三个参数：

“-h”获取帮助

“-zip”压缩：其后跟随“fileName”待压缩文件的文件名 “passwd”可选项表示加密秘钥

“-unzip”解压：其后跟随“fileName”已压缩文件的文件名 “passwd”可选项表示解密秘钥；

用例：

`./compressText.exe -zip ./fileName.txt passwd`  

`./compressText.exe -unzip ./fileName.txt.cod passwd`

