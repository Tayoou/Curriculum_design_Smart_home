#  **智慧家居系统** 

#### 介绍
Curriculum_design_Smart_home是一个基于STM32F10X的多人协作智慧家居项目
该项目的传感器架构均通过ESP8266WiFi模块连接

#### 系统架构
系统分为传感器层、MCU层、控制层。
传感器层使用无线通信模块
控制层采用普通的有限驱动
项目周期内可能会开发安卓APP
实现对MCU的远程控制及信息读取

#### 常用的git指令

1.  将本地仓库推送到远程仓库：
git push origin master
如果在push的过程中出现错误：errot:Updates were rejected because the remote contains work that you donot have locally.(拒绝更新,因为远程仓库包含本地没有的文件)：
git pull origin master

2.  从远程仓库克隆：
git clone git@gitee.com:用户名/仓库名.git
