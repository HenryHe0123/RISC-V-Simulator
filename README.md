# RISC-V simulator

本项目是一个用C++模拟电路的RISC-V处理器，支持简化的RISCV-32I指令集。CPU架构采用了（经过简化的）Tomasulo算法和基于局部分支预测的两级自适应预测器，同时也提供了一级双模态（Bimodal）预测器和默认跳转的静态预测器接口供使用。

### 模拟架构图

<img src="Arch.jpeg" alt="Arch" style="zoom:60%;" />

其中，P代表Predictor，IU代表Instruction Unit，RS代表Reversed Station，ROB代表Reorder Buffer，CDB代表Common Data Bus，黑色箭头方向代表一方有权对另一方进行访问修改或发送数据。反之如ROB没有到RS的黑色箭头，则代表ROB并不能直接修改RS，但这其实可以迂回地通过CDB广播来实现，CDB（不只一个）可以认为是被IU/RS/ROB共享的全局变量。

特别的，作为简化，（Tomasulo算法原本可能需要的）Load/Store Buffer被集成到了ROB中，ALU被设计为每个RSEntry都独立配备，Decoder也被认为是一个全局共享的电路模块，但其核心功能decode函数仅供IU使用。

### 主体逻辑说明

作为简化，本项目仅设置了三个流水线Issue/Execute/Commit，但Issue其实可以被进一步地分拆为Fetch和Decode，Commit也可以被分拆为Write Back和Retirement，具体这里略去不表。下面是三个流水线的具体模拟逻辑：

#### 发射(Issue|Instruction Unit)

在issue未stall/halt（halt时检查jalrBus是否在广播）且ROB未满的情况下，由IU从RAM中读取指令并调用Decoder解码，向ROB申请新位置。

若为REG/MEM类指令，修改可能需要的RegisterFile的dependency（要在RSEntry读取之后，防止a=a+1的错误），新建RSEntry并发射到RS上（特殊指令可能不需要）；若为BRANCH指令，新建RSEntry把计算任务发射到RS上，预测更新pc（其他指令也要正常更新pc）；读到END指令时，将其直接发射到ROB上并stall issue。

特别的，对于jalr指令，向RS发射pc计算任务，在ROB上添加reg修改指令并halt issue，等到RS计算完成传到ROB后，检查opt为jalr再重启issue并更新pc（专门开一条信道jalrBus从ROB传回IU，此时不需要更改ROBvalue，但要把状态改成ready）。这里我们认为pc+4是可以直接得到的（可以在硬件上专门设计一个组合逻辑电路），因此jal指令其实可以不进入RS而直接ready。

在发射到RS的过程中，如果发现RS已满，则暂停等待，pop新申请的ROBEntry。

#### 执行(Execute|Reversed Station)

检查所有unempty的entry，如果unready就检查接收ROB的CDB（ready本质上是一个Q1&Q2的组合逻辑电路）；如果ready但unbusy就把它推入ALU计算（busy = true）；如果ready and busy意味着计算完成，将计算结果（模拟中真正调用ALU）广播，更新ROB上的value&ready以及可能的predict，并对寄存器类指令修改RS里所有的dependency，特别的，如果是jalr指令，更新ROB的ready并在jalrBus上广播。

#### 提交(Commit|Reorder Buffer)

每次try commit队头最早的指令，

若predict_accuracy = false（此时应该已经ready），全局报错（广播predictBus**并立刻刷新**），在当前周期结束flush时清空RS&ROB以及Register的dependency（其实是清空并刷新），修改pc为正确分支，重新开始（注意如果此时若issue halt/stall则要reset）；

若ready = false，return等待ready；若ready = true但LS不为0，LS减1，对于Load操作，若现在truly ready，则**立刻执行访存操作**；

若ready = true&&LS = 0，正式提交，真实修改Register File或RAM（若是修改Reg还要CDB广播），指令出队，若为HALT指令则修改全局变量结束程序。

### 分支预测性能

本项目共实现了三种分支预测器（一个预测器抽象类的三个派生类），分别为

- 默认taken的静态预测器
- 一级双模态（Bimodal）预测器（二位饱和计数器，取址位宽13，2KB）
- 基于局部分支预测的两级自适应预测器（二位BHR，取址位宽13，10KB）

评测结果如下：

| 测试点         | 默认预测器 |           | 一级预测器 |           | 二级预测器 |            | 加权       |
| -------------- | ---------- | --------- | ---------- | --------- | ---------- | :--------: |----------|
| testcase       | cycle      | accurarcy | cycle      | accurarcy | cycle      | accurarcy  | weight   |
| array_test1    | 314        | 45.45%    | 302        | 54.55%    | 304        |   45.45%   | 2.2095   |
| array_test2    | 358        | 50%       | 358        | 57.69%    | 356        |   42.31%   | 2.3498   |
| basicopt1      | 1035678    | 41.02%    | 819305     | 82.40%    | 768606     |   92.23%   | 29.9011  |
| bulgarian      | 755442     | 49.36%    | 661627     | 90.06%    | 660891     |   90.36%   | 27.4815  |
| expr           | 847        | 62.16%    | 70         | 84.68%    | 793        |   76.58%   | 3.3947   |
| gcd            | 845        | 37.50%    | 731        | 67.50%    | 724        |   68.33%   | 3.3916   |
| hanoi          | 389594     | 50.02%    | 383584     | 61.10%    | 366384     |   80.76%   | 22.9835  |
| lvalue2        | 77         | 33.33%    | 71         | 66.67%    | 71         |   66.67%   | 0.9623   |
| magic          | 1067824    | 46.86%    | 992664     | 75.79%    | 978564     |   80.11%   | 30.1459  |
| manyarguments  | 103        | 20%       | 91         | 60%       | 83         |    80%     | 1.1857   |
| multiarray     | 2965       | 81.48%    | 2970       | 83.33%    | 2989       |   72.22%   | 5.3791   |
| naive          | 39         | 100%      | 39         | 100%      | 39         |    100%    | 0.4990   |
| pi             | 183405098  | 57.73%    | 153845437  | 82.40%    | 151658749  |   84.16%   | 114.3732 |
| qsort          | 2528889    | 69.99%    | 2382607    | 88.09%    | 2362557    |   91.23%   | 37.8779  |
| queens         | 1238079    | 36.78%    | 1123379    | 71.87%    | 1134400    |   73.31%   | 31.3570  |
| statement_test | 1852       | 40.59%    | 1708       | 60.89%    | 1654       |   66.83%   | 4.5601   |
| superloop      | 1697868    | 12.71%    | 615903     | 93.82%    | 594073     |   95.19%   | 34.0974  |
| tak            | 3001267    | 25.00%    | 2960535    | 73.81%    | 2954617    |   76.88%   | 39.6223  |
| 加权平均准确率 |            | 46.61%    |            | 80.28%    |            | **83.67%** |          |

其中加权公式为 $w=cycle^{\frac{1}{4}}-2$，cycle统一用的是默认taken时所需要的cpu cycle。 加权公式的选取主要考虑了两方面的因素，一是减小cycle过小时预测准确率不可靠的影响，二是避免pi程序cycle过大而在加权平均中占据绝对主导地位。

加权平均结果显示，二级预测器的准确率最高，一级预测器略逊，而两者的性能都显著优于默认预测器，这也符合我们的预期。~~笔者还偷偷用此加权平均公式对某助教的两级自适应预测器的预测率进行计算，满意地发现其结果83.11%略小于83.67%.~~

