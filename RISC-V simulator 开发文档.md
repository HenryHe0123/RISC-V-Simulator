### RISC-V simulator 开发文档

#### Tomasulo 算法

<img src="C:\Users\HONOR\Downloads\OoO.jpeg" alt="OoO" style="zoom: 50%;" />

<img src="C:\Users\HONOR\Downloads\Arch.jpeg" alt="Arch" style="zoom:80%;" />

#### 主体逻辑说明

**发射(Issue)**

在未stall且ROB未满的情况下，由Instruction Unit从RAM中读取指令并调用Decoder解码，向ROB申请新位置。

若为REG类指令，新建RSEntry并发射到RS上；若为MEM/END类指令，直接写到ROB上；若为BRANCH指令，新建RSEntry把计算任务发射到RS上，预测更新pc（其他指令也要正常更新pc）

在发射到RS的过程中，如果发现RS已满，则暂停等待（pop新申请的ROBEntry）

**执行(Execute)**

如果操作数均准备好了，直接执行，可能占用多个时钟周期，如果在同一个时钟周期内有多个指令可以执行，一般策略是随机挑选。如果任一一个操作数没有准备好，监控CDB获取计算好的操作数，**此步骤检查RAW hazard.** 

对于store指令，此步骤仅仅计算effective address.

对于load指令，除了计算effective address，还需要确保当前ROB中的Store指令没有相同的Destination时，才会去内存/cache中读数据，这里避免了针对同一effective address的RAW hazard.

