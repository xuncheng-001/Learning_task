# 调拨盘offset

- 步兵拨盘拨动一个小弹丸需要转动0.7rad（0.69rad）
- 正常流程，关自起，用rqt控制打弹，让弹链充满子弹
- 用手拨动拨盘让他紧凑，开plotjuggler看trigger的位置，记录然后取反给urdf
- 看弹链里面子弹的松紧程度，或者打弹看会不会双发
- shooter.transmission.urdf.xacro