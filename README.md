# GetTopURL
Getting top 100 URL from a log file

1, 编译使用 g++ ，加入C++11 选项
   g++ -std=c++11 -O 2 -o get_topn get_topN.cpp

2, 用法
   ./get_topn <file_name> <bucket_count> <top number>
   ./get_topn ./urls 128 100
   1) urls: 第一个参数为记录url的文件名称
   2) 128: hash桶个数。如果内存不够，则使用更多的hash桶。例如，100G url文件，内存1G，则桶设为128则可满足最坏的情况
   3）100：求出排名前100的url，可以设定前多少个
   
3，输出：
   程序将输出如下结果：
   ranking 1 count:10000 usrl:https://github.com/hexinmin
   ranking 2 count:10000 usrl:https://github.com/hexinmin1
   ranking 2 count:10000 usrl:https://github.com/hexinmin1
   ranking 3 count:10000 usrl:https://github.com/hexinmin3
   ranking 4 count:10000 usrl:https://github.com/hexinmin4
   ......
   排名如何有并列的情况，则将出现过的url都输出
