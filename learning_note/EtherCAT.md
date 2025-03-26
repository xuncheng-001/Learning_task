# EtherCAT

一种用于工业自动化领域的实时以太通信协议（还不太懂）

```
ethercat_master:
  time_step: 0.001

ethercat_devices:
  - type: Rm
    name: RmSlave1
    configuration_file: device_configurations/standard.yaml
    ethercat_bus: enp89s0
    ethercat_address: 1
```





形如这样，EtherCAT有主站和从站两种设备

其中

```
ethercat_master:
```

下面是对主站的参数配置

```
	time_step: 0.001
```

表示时间布长，也就是说每0.001秒，主站就会和所有从站设备进行一次数据交换



则类似的

```
ethercat_devices:
```

就是对从站的配置

- type和name分别是从站的类型和名字
- configuration_file:后面是设备的配置文件路径。
- ethercat_bus:是设备连接到的网络接口
- ethercat_address:是指定设备的EtherCAT的地址，每个从站都要有一个唯一的地址，像上文中的1代表设备是网络上的第一个从站设备



##### EtherCAT的双全工特性

> EtherCAT作为一种工业以太网总线，充分利用了以太网的全双功特性



全双工：数据可以同时发送和接受，不存在冲突，提高通信效力





EtherCAT使用主从通信