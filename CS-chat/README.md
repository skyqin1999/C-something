## winsocket编程

使用一个中继服务器（公网ip、win系统）桥接两个客户端，由于不会使用listen多个连接，使用两个端口连接两个客户端


## 错误

1. 两方的消息是同步的，会出现对方发过来的文字覆盖我我正在编辑发给对方的内容的情况。

2. 服务器、客户端异常关闭情况没有考虑

## 效果展示

![本地展示](https://skyqin1999.oss-cn-beijing.aliyuncs.com/Github/C-something/CS-chat/20201108.png)


