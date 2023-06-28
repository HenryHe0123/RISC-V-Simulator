### RISC-V simulator 开发文档

#### Tomasulo 算法

<img src="C:\Users\HONOR\Downloads\OoO.jpeg" alt="OoO" style="zoom: 50%;" />

<img src="C:\Users\HONOR\Downloads\Arch.jpeg" alt="Arch" style="zoom:80%;" />

#### 主体逻辑说明

**发射(Issue)**

在issue未stall/halt（halt时检查jalrBus是否在广播）且ROB未满的情况下，由Instruction Unit从RAM中读取指令并调用Decoder解码，向ROB申请新位置。

若为REG/MEM类指令，修改可能需要的RegisterFile的dependency（要在RSEntry读取之后，防止a=a+1的错误），新建RSEntry并发射到RS上（特殊指令可能不需要）；若为BRANCH指令，新建RSEntry把计算任务发射到RS上，预测更新pc（其他指令也要正常更新pc）；读到END指令时，将其直接发射到ROB上并stall issue。

特别的，对于jalr指令，向RS发射pc计算任务，在ROB上添加reg修改指令并halt issue，等到RS计算完成传到ROB后，检查opt为jalr再重启issue并更新pc（专门开一条信道jalrBus从ROB传回IU，此时不需要更改ROBvalue，但要把状态改成ready）。这里我们认为pc+4是可以直接得到的（可以在硬件上专门设计一个组合逻辑电路），因此jal指令其实可以不进入RS而直接ready。

在发射到RS的过程中，如果发现RS已满，则暂停等待，pop新申请的ROBEntry。

**执行(Execute)**

如果操作数均准备好了，直接执行，可能占用多个时钟周期，如果在同一个时钟周期内有多个指令可以执行，一般策略是随机挑选。如果任一一个操作数没有准备好，监控CDB获取计算好的操作数，**此步骤检查RAW hazard.** 

对于store指令，此步骤仅仅计算effective address.

对于load指令，除了计算effective address，还需要确保当前ROB中的Store指令没有相同的Destination时，才会去内存/cache中读数据，这里避免了针对同一effective address的RAW hazard.

**提交(Commit)**

每次提交一个，对寄存器修改指令，真实修改寄存器并放入
